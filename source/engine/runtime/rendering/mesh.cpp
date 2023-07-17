#include "mesh.h"
#include "camera.h"
#include "generator/generator.hpp"

#include <core/graphics/index_buffer.h>
#include <core/graphics/vertex_buffer.h>
#include <core/logging/logging.h>
#include <core/memory/checked_delete.h>

#include <algorithm>
#include <cmath>
#include <cstring>
//-----------------------------------------------------------------------------
// Local Module Level Namespaces.
//-----------------------------------------------------------------------------
// Settings for the mesh optimizer.
namespace MeshOptimizer
{
const float CacheDecayPower = 1.5f;
const float LastTriScore = 0.75f;
const float ValenceBoostScale = 2.0f;
const float ValenceBoostPower = 0.5f;
const std::int32_t MaxVertexCacheSize = 32;
};

mesh::mesh()
	: hardware_vb_(std::make_shared<gfx::vertex_buffer>())
	, hardware_ib_(std::make_shared<gfx::index_buffer>())
{
}

mesh::~mesh()
{
	dispose();
}

void mesh::dispose()
{
	// Iterate through the different subsets in the mesh and clean up
	for(auto subset : mesh_subsets_)
	{
		// Just perform a standard 'disconnect' in the
		// regular unload case.
		checked_delete(subset);
	}

	mesh_subsets_.clear();
	subset_lookup_.clear();
	data_groups_.clear();

	// Release bone palettes and skin data (if any)
	bone_palettes_.clear();
	skin_bind_data_.clear();

	// Clean up preparation data.
	if(preparation_data_.owns_source)
		checked_array_delete(preparation_data_.vertex_source);
	preparation_data_.vertex_source = nullptr;
	preparation_data_.source_format = {};
	preparation_data_.owns_source = false;
	preparation_data_.vertex_data.clear();
	preparation_data_.vertex_flags.clear();
	preparation_data_.vertex_records.clear();
	preparation_data_.triangle_data.clear();

	// Release mesh data memory
	checked_array_delete(system_vb_);
	checked_array_delete(system_ib_);

	triangle_data_.clear();

	// Release resources
	hardware_vb_.reset();
	hardware_ib_.reset();

	// Clear variables
	preparation_data_.vertex_source = nullptr;
	preparation_data_.owns_source = false;
	preparation_data_.source_format = {};
	preparation_data_.triangle_count = 0;
	preparation_data_.vertex_count = 0;
	preparation_data_.compute_normals = false;
	preparation_data_.compute_binormals = false;
	preparation_data_.compute_tangents = false;
	prepare_status_ = mesh_status::not_prepared;
	face_count_ = 0;
	vertex_count_ = 0;
	system_vb_ = nullptr;
	vertex_format_ = {};
	system_ib_ = nullptr;
	force_tangent_generation_ = false;
	force_normal_generation_ = false;
	force_barycentric_generation_ = true;

	// Reset structures
	bbox_.reset();
}

bool mesh::bind_skin(const skin_bind_data& bind_data)
{
	if(!bind_data.has_bones())
		return true;

	skin_bind_data::vertex_data_array_t vertex_table;
	bone_combination_map_t bone_combinations;

	// Get access to required systems and limits
	std::uint32_t palette_size = gfx::get_max_blend_transforms();

	// Destroy any previous palette entries.
	bone_palettes_.clear();

	skin_bind_data_.clear();
	skin_bind_data_ = bind_data;

	// If the mesh has already been prepared, roll it back.
	if(prepare_status_ == mesh_status::prepared)
		prepare_mesh(vertex_format_);

	face_influences used_bones;
	// Build a list of all bone indices and associated weights for each vertex.
	skin_bind_data_.build_vertex_table(preparation_data_.vertex_count, preparation_data_.vertex_records,
									   vertex_table);

	// Vertex based influences are no longer required. Clear them to save space.
	skin_bind_data_.clear_vertex_influences();

	// Now build a list of unique bone combinations that influences each face in
	// the mesh
	triangle_array_t& tri_data = preparation_data_.triangle_data;
	for(std::uint32_t i = 0; i < preparation_data_.triangle_count; ++i)
	{
		// Clear out any previous bone references and set up for new run.
		used_bones.bones.clear();

		// Collect all unique bone indices from each of the three face vertices.
		for(std::uint32_t j = 0; j < 3; ++j)
		{
			std::uint32_t vertex = tri_data[i].indices[j];

			const auto& data = vertex_table[vertex];
			// Add each influencing bone if unique.
			for(const auto& influence : data.influences)
			{
				used_bones.bones[static_cast<std::uint32_t>(influence)] =
					1; // Just set to any value, we're only using a key/value dictionary
					   // for acceleration
			}

		} // Next Vertex

		// Now that we have a unique list of bones that influence this face,
		// determine if any faces with identical influences already exist and
		// add it to the list. Alternatively this may be the first face with
		// these exact influences, in which case a new entry will be created.
		const auto tri_data_group_id = tri_data[i].data_group_id;
		auto it_combination = bone_combinations.find(bone_combination_key(&used_bones, tri_data_group_id));
		if(it_combination == bone_combinations.end())
		{
			auto* face_list_ptr = new std::vector<std::uint32_t>();
			auto* influences_ptr = new face_influences(used_bones);
			bone_combinations.insert(bone_combination_map_t::value_type(
				bone_combination_key(influences_ptr, tri_data_group_id), face_list_ptr));

			// Assign face to this combination.
			face_list_ptr->push_back(i);

		} // End if doesn't exist yet
		else
		{
			// Assign face to this combination.
			it_combination->second->push_back(i);

		} // End if already exists

	} // Next Face

	// We now have a complete list of the unique combinations of bones that
	// influence every face in the mesh. The next task is to combine these
	// unique lists into as few combined bone "palettes" as possible, containing
	// as many bones as possible. To do so, we must continuously search for
	// face influence combinations that will fit into any existing palettes.

	const auto materials_count = mesh_subsets_.size();
	for(auto data_group_id = 0u; data_group_id < materials_count; ++data_group_id)
	{

		// Keep searching until we have consumed all unique face influence
		// combinations.
		for(; !bone_combinations.empty();)
		{
			bone_combination_map_t::iterator it_combination;
			auto it_best_combination = bone_combinations.end();
			auto it_largest_combination = bone_combinations.end();
			int max_common = -1, max_bones = -1;
			int palette_id = -1;

			// Search face influences for the next best combination to add to a palette.
			// We search
			// for two specific properties; A) does it share a large amount in common
			// with an existing
			// palette and B) does this have the largest list of bones. We'll work out
			// which of these
			// properties we're interested in later.
			for(it_combination = bone_combinations.begin(); it_combination != bone_combinations.end();
				++it_combination)
			{
				face_influences* influences_ptr = it_combination->first.influences;
				const auto combination_group_id = it_combination->first.data_group_id;

				if(data_group_id != combination_group_id)
					continue;

				// std::vector<std::uint32_t>* face_list_ptr = it_combination->second;
				auto bones_size = static_cast<int>(influences_ptr->bones.size());
				// Record the combination with the largest set of bones in case we can't
				// find a suitable palette to insert any influence into at this point.

				if(bones_size > max_bones)
				{
					it_largest_combination = it_combination;
					max_bones = bones_size;
				} // End if largest

				// Test against each palette
				for(size_t i = 0; i < bone_palettes_.size(); ++i)
				{
					std::int32_t common_bones, additional_bones, remaining_space;
					auto& palette = bone_palettes_[i];

					if(data_group_id != palette.get_data_group())
						continue;
					// Also compute how the combination might fit into this palette if at
					// all
					palette.compute_palette_fit(influences_ptr->bones, remaining_space, common_bones,
												additional_bones);

					// Now that we know how many bones this batch of faces has in common
					// with the
					// palette, and how many new bones it will add, we can work out if it's
					// a good
					// idea to use this batch next (assuming it will fit at all).
					if(additional_bones <= remaining_space)
					{
						// Does it have the most in common so far with this palette?
						if(common_bones > max_common)
						{
							max_common = common_bones;
							it_best_combination = it_combination;
							palette_id = static_cast<int>(i);

						} // End if better choice

					} // End if smaller

				} // Next Palette

			} // Next influence combination

			// If nothing was found then we have run out of influences relevant to the
			// source material
			if(it_largest_combination == bone_combinations.end())
				break;

			// We should hopefully have selected at least one good candidate for
			// insertion
			// into an existing palette that we can now process. If not, we must create
			// a new palette into which we will store the combination with the largest
			// number of bones discovered.
			if(palette_id >= 0)
			{
				face_influences* influences_ptr = it_best_combination->first.influences;
				std::vector<std::uint32_t>* face_list_ptr = it_best_combination->second;

				// At least one good combination was found that can be added to an
				// existing palette.
				auto& palette = bone_palettes_[static_cast<std::size_t>(palette_id)];
				palette.assign_bones(influences_ptr->bones, *face_list_ptr);

				// We should now remove the selected combination.
				bone_combinations.erase(it_best_combination);
				checked_delete(influences_ptr);
				checked_delete(face_list_ptr);

			} // End if found fit
			else
			{
				const auto data_group = it_largest_combination->first.data_group_id;
				face_influences* influences_ptr = it_largest_combination->first.influences;
				std::vector<std::uint32_t>* face_list_ptr = it_largest_combination->second;

				// No combination of face influences was able to fit into an existing
				// palette.
				// We must generate a new one and store the largest combination
				// discovered.
				bone_palette new_palette(palette_size);
				new_palette.set_data_group(data_group);
				new_palette.assign_bones(influences_ptr->bones, *face_list_ptr);
				bone_palettes_.push_back(new_palette);

				// We should now remove the selected combination.
				bone_combinations.erase(it_largest_combination);
				checked_delete(influences_ptr);
				checked_delete(face_list_ptr);

			} // End if none found

		} // Next iteration
	}
	// Bone palettes are now fully constructed. mesh vertices must now be split as
	// necessary in cases where they are shared by faces in alternate palettes.
	for(size_t i = 0; i < bone_palettes_.size(); ++i)
	{
		auto& palette = bone_palettes_[i];
		std::vector<std::uint32_t>& faces = palette.get_influenced_faces();
		for(size_t j = 0, total = faces.size(); j < total; ++j)
		{

			// Assign face to correct data group.
			std::uint32_t face_index = faces[j];
			tri_data[face_index].data_group_id = palette.get_data_group();

			// Update vertex information
			for(std::uint32_t k = 0; k < 3; ++k)
			{
				auto& data = vertex_table[tri_data[face_index].indices[k]];

				const auto blend_idx = static_cast<int>(data.influences.size() - 1);
				// Record the largest blend index necessary for processing this palette.
				if(blend_idx > palette.get_maximum_blend_index())
					palette.set_maximum_blend_index(std::min<int>(3, blend_idx));

				// If this vertex has already been assigned to an alternative
				// palette, then it must be duplicated.
				if(data.palette != -1 && data.palette != static_cast<std::int32_t>(i))
				{
					auto new_index = static_cast<std::uint32_t>(vertex_table.size());

					// Split vertex
					skin_bind_data::vertex_data new_vertex(data);
					new_vertex.palette = static_cast<std::int32_t>(i);
					vertex_table.push_back(new_vertex);

					// Update triangle indices
					tri_data[face_index].indices[k] = new_index;

				} // End if already assigned
				else
				{
					// Assign to palette
					data.palette = static_cast<std::int32_t>(i);

				} // End if not assigned

			} // Next triangle vertex

		} // Next Face

		// We've finished with the palette's assigned face list.
		// This is only a temporary array.
		palette.clear_influenced_faces();

	} // Next Palette Entry

	// Vertex format must be adjusted to include blend weights and indices
	// (if they don't already exist).
	gfx::vertex_layout new_format(vertex_format_);
	gfx::vertex_layout original_format = vertex_format_;
	bool has_weights = new_format.has(gfx::attribute::Weight);
	bool has_indices = new_format.has(gfx::attribute::Indices);
	if(!has_weights || !has_indices)
	{
		new_format.m_hash = 0;
		if(!has_weights)
			new_format.add(gfx::attribute::Weight, 4, gfx::attribute_type::Float);
		if(!has_indices)
			new_format.add(gfx::attribute::Indices, 4, gfx::attribute_type::Float, false, true);

		new_format.end();
		// Add to format database.
		vertex_format_ = new_format;
		// Vertex format was updated.

	} // End if needs new format

	// Get access to final data offset information.
	std::uint16_t vertex_stride = vertex_format_.getStride();

	// Now we need to update the vertex data as required. It's possible
	// that the buffer also needs to grow if / when vertices were split.
	// First convert original data into the new format and also ensure
	// that it is large enough to contain our entire final data set.
	std::uint32_t original_vertex_count = static_cast<std::uint32_t>(preparation_data_.vertex_count);
	if(vertex_format_.m_hash != original_format.m_hash)
	{
		// Format has changed, run conversion.
		byte_array_t original_buffer(preparation_data_.vertex_data);
		preparation_data_.vertex_data.clear();
		preparation_data_.vertex_data.resize(vertex_table.size() * vertex_stride);
		preparation_data_.vertex_flags.resize(vertex_table.size());

		gfx::vertex_convert(vertex_format_, &preparation_data_.vertex_data[0], original_format,
							&original_buffer[0], original_vertex_count);

	} // End if convert
	else
	{
		// No conversion required, just add space for the new vertices.
		preparation_data_.vertex_data.resize(vertex_table.size() * vertex_stride);
		preparation_data_.vertex_flags.resize(vertex_table.size());

	} // End if !convert

	// Populate with newly constructed information.
	std::uint8_t* src_vertices_ptr = &preparation_data_.vertex_data[0];
	for(size_t i = 0; i < vertex_table.size(); ++i)
	{
		auto& data = vertex_table[i];

		//// TODO is this correct?
		if(data.palette < 0)
			continue;

		const auto& palette = bone_palettes_[static_cast<std::size_t>(data.palette)];

		// If this is a new vertex, duplicate data from original vertex (it will
		// have
		// already been converted to its final format by this point).
		if(i >= original_vertex_count)
		{
			// This is a new vertex. Duplicate data from original vertex (it will have
			// already been
			// converted to its final format by this point).
			memcpy(src_vertices_ptr + (i * vertex_stride),
				   src_vertices_ptr + (data.original_vertex * vertex_stride), vertex_stride);

			// Also duplicate additional vertex data.
			preparation_data_.vertex_flags[i] = preparation_data_.vertex_flags[data.original_vertex];

		} // End if new vertex

		// Assign bone indices (the index to the relevant entry in the palette,
		// not the main bone list index) and weights.
		math::vec4 blend_weights(0.0f, 0.0f, 0.0f, 0.0f);
		math::vec4 blend_indices(0.0f, 0.0f, 0.0f, 0.0f);

		// std::uint32_t blend_indices = 0;
		std::uint32_t max_bones = std::min<std::uint32_t>(4, std::uint32_t(data.influences.size()));
		for(std::uint32_t j = 0; j < max_bones; ++j)
		{
			// Store vertex indices and weights
			blend_indices[static_cast<math::vec4::length_type>(j)] = static_cast<float>(
				palette.translate_bone_to_palette(static_cast<std::uint32_t>(data.influences[j])));
			blend_weights[static_cast<math::vec4::length_type>(j)] = data.weights[j];

		} // Next Influence

		gfx::vertex_pack(math::value_ptr(blend_weights), false, gfx::attribute::Weight, vertex_format_,
						 src_vertices_ptr, std::uint32_t(i));

		gfx::vertex_pack(math::value_ptr(blend_indices), false, gfx::attribute::Indices, vertex_format_,
						 src_vertices_ptr, std::uint32_t(i));

	} // Next Vertex

	// Update vertex count to match final size.
	preparation_data_.vertex_count = static_cast<std::uint32_t>(vertex_table.size());

	vertex_table.clear();

	// Skin is now bound?
	return true;
}

bool mesh::bind_armature(std::unique_ptr<armature_node>& root)
{
	root_ = std::move(root);
	return true;
}

void mesh::set_subset_count(uint32_t count)
{
	if(count > 0)
		mesh_subsets_.resize(count);
}

bool mesh::prepare_mesh(const gfx::vertex_layout& format)
{
	// If we are already in the process of preparing, this is a no-op.
	if(prepare_status_ == mesh_status::preparing)
		return false;

	// Should we roll back an earlier call to 'endPrepare' ?
	if(prepare_status_ == mesh_status::prepared)
	{
		// Reset required values.
		preparation_data_.triangle_count = 0;
		preparation_data_.triangle_data.clear();
		preparation_data_.vertex_count = 0;
		preparation_data_.vertex_data.clear();
		preparation_data_.vertex_flags.clear();
		preparation_data_.vertex_records.clear();
		preparation_data_.compute_normals = false;
		preparation_data_.compute_binormals = false;
		preparation_data_.compute_tangents = false;

		// We can release the prior prepared triangle data early as this information
		// will be reconstructed from the existing mesh subset table.
		triangle_data_.clear();

		// Release prior hardware buffers if they were constructed.
		hardware_vb_.reset();
		hardware_ib_.reset();

		// Set the size of the preparation buffer so that we can add
		// the existing buffer data to it.
		auto new_stride = format.getStride();
		preparation_data_.vertex_data.resize(vertex_count_ * new_stride);
		preparation_data_.vertex_count = vertex_count_;

		// Create enough space in our triangle data array for
		// the number of faces that existed in the prior final buffer.
		preparation_data_.triangle_data.resize(face_count_);

		// Copy all of the vertex data back into the preparation
		// structures, converting if necessary.
		if(format.m_hash == vertex_format_.m_hash)
			memcpy(&preparation_data_.vertex_data[0], system_vb_, preparation_data_.vertex_data.size());
		else
			gfx::vertex_convert(format, &preparation_data_.vertex_data[0], vertex_format_, system_vb_,
								vertex_count_);

		// Clear out the vertex buffer
		checked_array_delete(system_vb_);
		vertex_count_ = 0;

		// Iterate through each subset and extract triangle data.
		for(auto subset : mesh_subsets_)
		{
			// Iterate through each face in the subset
			std::uint32_t* current_index_ptr = &system_ib_[(subset->face_start * 3)];
			for(std::uint32_t i = 0; i < subset->face_count; ++i, current_index_ptr += 3)
			{
				// Generate winding data
				triangle& tri = preparation_data_.triangle_data[preparation_data_.triangle_count++];
				tri.data_group_id = subset->data_group_id;
				memcpy(tri.indices, current_index_ptr, 3 * sizeof(std::uint32_t));

			} // Next Face

		} // Next subset

		// Release additional memory
		checked_array_delete(system_ib_);
		face_count_ = 0;

		// Determine which components the original vertex data actually contained.
		bool source_has_normals = vertex_format_.has(gfx::attribute::Normal);
		bool source_has_binormal = vertex_format_.has(gfx::attribute::Bitangent);
		bool source_has_tangent = vertex_format_.has(gfx::attribute::Tangent);

		// The 'preparation_data::vertexFlags' array contains a record of the above
		// for each vertex
		// that currently exists in the preparation buffer. This is required when
		// performing processes
		// such as the generation of vertex normals, etc.
		std::uint8_t vertex_flags = 0;
		if(source_has_normals)
			vertex_flags |= preparation_data::source_contains_normal;
		if(source_has_binormal)
			vertex_flags |= preparation_data::source_contains_binormal;
		if(source_has_tangent)
			vertex_flags |= preparation_data::source_contains_tangent;

		// Record the information.
		preparation_data_.vertex_flags.resize(preparation_data_.vertex_count);
		for(std::uint32_t i = 0; i < preparation_data_.vertex_count; ++i)
			preparation_data_.vertex_flags[i] = vertex_flags;

		// If skin binding data was available (i.e. this is a skinned mesh), reverse
		// engineer
		// the existing data into a newly populated skin binding structure.
		if(skin_bind_data_.has_bones())
		{
			// Clear out any previous influence data.
			skin_bind_data_.clear_vertex_influences();

			// First, retrieve the offsets to the weight and bone palette index data
			// stored in each vertex in the mesh.

			//::uint16_t vertex_stride = _vertex_format.getStride();
			std::uint8_t* vertex_data_ptr = &preparation_data_.vertex_data[0];

			// Search through each bone palette to get influence data.
			skin_bind_data::bone_influence_array_t& bones = skin_bind_data_.get_bones();
			for(const auto& palette : bone_palettes_)
			{
				// Find the subset associated with this bone palette.
				subset* subset_ptr = subset_lookup_[mesh_subset_key(palette.get_data_group())];

				// Process faces in this subset to retrieve referenced vertex data.
				std::int32_t max_blend_index = palette.get_maximum_blend_index();
				std::vector<std::uint32_t> bones_references = palette.get_bones();
				for(std::int32_t face = subset_ptr->face_start;
					face < (subset_ptr->face_start + static_cast<std::int32_t>(subset_ptr->face_count));
					++face)
				{
					triangle& tri = preparation_data_.triangle_data[static_cast<std::size_t>(face)];
					for(size_t i = 0; i < 3; ++i)
					{
						float weights[4];
						gfx::vertex_unpack(weights, gfx::attribute::Weight, vertex_format_, vertex_data_ptr,
										   tri.indices[i]);
						float indices[4];
						gfx::vertex_unpack(indices, gfx::attribute::Indices, vertex_format_, vertex_data_ptr,
										   tri.indices[i]);

						float* weights_ptr = weights;
						std::uint32_t ind = math::color::float4_to_u32(
							math::vec4{indices[0], indices[1], indices[2], indices[3]});
						auto* indices_ptr = reinterpret_cast<std::uint8_t*>(&ind);

						// Add influence data back to the referenced bones.
						for(std::int32_t j = 0; j <= max_blend_index; ++j)
						{
							if(indices_ptr[j] != 0xFF)
							{
								auto& bone = bones[bones_references[indices_ptr[j]]];
								bone.influences.emplace_back(tri.indices[i], weights_ptr[j]);

								// Prevent duplicate insertions
								indices_ptr[j] = 0xFF;

							} // End if valid

						} // Next blend reference

					} // Next triangle index

				} // Next face

			} // Next Palette

		} // End if is skin

		// Clean up heap allocated subset structures
		for(auto subset : mesh_subsets_)
		{
			checked_delete(subset);
		}
		// Reset prepared data arrays and variables.
		data_groups_.clear();
		mesh_subsets_.clear();
		subset_lookup_.clear();
		hardware_mesh_ = false;
		optimize_mesh_ = false;

	} // End if roll back an earlier prepare
	else if((prepare_status_ != mesh_status::preparing))
	{
		// Clear out anything which is currently loaded in the mesh.
		dispose();

	} // End if not rolling back or no need to roll back

	// We are in the process of preparing the mesh
	prepare_status_ = mesh_status::preparing;
	vertex_format_ = format;

	return true;
}

bool mesh::prepare_mesh(const gfx::vertex_layout& format, void* vertices_ptr, std::uint32_t vertex_count,
						const triangle_array_t& faces, bool hardware_copy /* = true */,
						bool weld /* = true */, bool optimize /* = true */)
{
	// Clear out anything which is currently loaded in the mesh.
	dispose();

	// We are in the process of preparing the mesh
	prepare_status_ = mesh_status::preparing;
	vertex_format_ = format;

	// Populate preparation structures.
	preparation_data_.triangle_count = static_cast<std::uint32_t>(faces.size());
	preparation_data_.triangle_data = faces;
	preparation_data_.vertex_count = vertex_count;

	// Copy vertex data.
	preparation_data_.vertex_data.resize(vertex_count * format.getStride());
	preparation_data_.vertex_flags.resize(vertex_count);
	memset(&preparation_data_.vertex_flags[0], 0, vertex_count);
	memcpy(&preparation_data_.vertex_data[0], vertices_ptr, vertex_count * format.getStride());

	// Generate the bounding box data for the new geometry.
	std::int32_t position_offset = format.getOffset(gfx::attribute::Position);
	auto stride = static_cast<std::int32_t>(format.getStride());
	if(format.has(gfx::attribute::Position))
	{
		std::uint8_t* src_ptr = reinterpret_cast<std::uint8_t*>(vertices_ptr) + position_offset;
		for(std::uint32_t i = 0; i < vertex_count; ++i, src_ptr += stride)
			bbox_.add_point(*(reinterpret_cast<math::vec3*>(src_ptr)));

	} // End if has position

	// Finish up
	return end_prepare(hardware_copy, weld, optimize);
}

bool mesh::set_vertex_source(void* source_ptr, std::uint32_t vertex_count,
							 const gfx::vertex_layout& source_format)
{
	// We can only do this if we are in the process of preparing the mesh
	if(prepare_status_ != mesh_status::preparing)
	{
		APPLOG_ERROR("Attempting to set a mesh vertex source without first calling "
					 "'prepareMesh' is not allowed.\n");
		return false;

	} // End if not preparing

	// Clear any existing source information.
	if(preparation_data_.owns_source)
		checked_array_delete(preparation_data_.vertex_source);
	preparation_data_.vertex_source = nullptr;
	preparation_data_.source_format = {};
	preparation_data_.owns_source = false;
	preparation_data_.vertex_records.clear();

	// If specifying nullptr (i.e. to clear) then we're done.
	if(source_ptr == nullptr)
		return true;

	// Validate requirements
	if(vertex_count == 0)
		return false;

	// If source format matches the format we're using to prepare
	// then just store the pointer for this vertex source. Otherwise
	// we need to allocate a temporary buffer and convert the data.
	preparation_data_.source_format = source_format;
	if(source_format.m_hash == vertex_format_.m_hash)
	{
		preparation_data_.vertex_source = reinterpret_cast<std::uint8_t*>(source_ptr);

	} // End if matching
	else
	{
		preparation_data_.vertex_source = new std::uint8_t[vertex_count * vertex_format_.getStride()];
		preparation_data_.owns_source = true;
		gfx::vertex_convert(vertex_format_, preparation_data_.vertex_source, source_format,
							reinterpret_cast<std::uint8_t*>(source_ptr), vertex_count);
	} // End if !matching

	// Allocate the vertex records for the new vertex buffer
	preparation_data_.vertex_records.clear();
	preparation_data_.vertex_records.resize(vertex_count);

	// Fill with 0xFFFFFFFF initially to indicate that no vertex
	// originally in this location has yet been inserted into the
	// final vertex list.
	memset(&preparation_data_.vertex_records[0], 0xFF, vertex_count * sizeof(std::uint32_t));

	// Some data needs computing? These variables are essentially 'toggles'
	// that are set largely so that we can early out if it was NEVER necessary
	// to generate these components (i.e. not one single vertex needed it).
	if(!source_format.has(gfx::attribute::Normal) && vertex_format_.has(gfx::attribute::Normal))
		preparation_data_.compute_normals = true;
	if(!source_format.has(gfx::attribute::Bitangent) && vertex_format_.has(gfx::attribute::Bitangent))
		preparation_data_.compute_binormals = true;
	if(!source_format.has(gfx::attribute::Tangent) && vertex_format_.has(gfx::attribute::Tangent))
		preparation_data_.compute_tangents = true;

	// Success!
	return true;
}

bool mesh::add_primitives(const triangle_array_t& triangles)
{
	std::uint32_t orig_index, index;

	// We can only do this if we are in the process of preparing the mesh
	if(prepare_status_ != mesh_status::preparing)
	{
		APPLOG_ERROR("Attempting to add primitives to a mesh without first calling "
					 "'prepareMesh' is not allowed.\n");
		return false;

	} // End if not preparing

	// Determine the correct offset to any relevant elements in the vertex
	bool has_position = vertex_format_.has(gfx::attribute::Position);
	bool has_normal = vertex_format_.has(gfx::attribute::Normal);
	std::uint16_t vertex_stride = vertex_format_.getStride();

	// During the construction process we test to see if any specified
	// vertex normal contains invalid data. If the original source vertex
	// data did not contain a normal, we can optimize and skip this step.
	bool source_has_normals = preparation_data_.source_format.has(gfx::attribute::Normal);
	bool source_has_binormal = preparation_data_.source_format.has(gfx::attribute::Bitangent);
	bool source_has_tangent = preparation_data_.source_format.has(gfx::attribute::Tangent);

	// In addition, we also record which of the required components each
	// vertex actually contained based on the following information.
	std::uint8_t vertex_flags = 0;
	if(source_has_normals)
		vertex_flags |= preparation_data::source_contains_normal;
	if(source_has_binormal)
		vertex_flags |= preparation_data::source_contains_binormal;
	if(source_has_tangent)
		vertex_flags |= preparation_data::source_contains_tangent;

	// Loop through the specified faces and process them.
	std::uint8_t* src_vertices_ptr = preparation_data_.vertex_source;
	for(const auto& src_tri : triangles)
	{
		// Retrieve vertex positions (if there are any) so that we can perform
		// degenerate testing.
		if(has_position)
		{
			math::vec3 v1;
			float vf1[4];
			gfx::vertex_unpack(vf1, gfx::attribute::Position, vertex_format_, src_vertices_ptr,
							   src_tri.indices[0]);
			math::vec3 v2;
			float vf2[4];
			gfx::vertex_unpack(vf2, gfx::attribute::Position, vertex_format_, src_vertices_ptr,
							   src_tri.indices[1]);
			math::vec3 v3;
			float vf3[4];
			gfx::vertex_unpack(vf3, gfx::attribute::Position, vertex_format_, src_vertices_ptr,
							   src_tri.indices[2]);
			memcpy(&v1[0], vf1, 3 * sizeof(float));
			memcpy(&v2[0], vf2, 3 * sizeof(float));
			memcpy(&v3[0], vf3, 3 * sizeof(float));

			// Skip triangle if it is degenerate.
			if((math::equal(v1, v2, math::epsilon<float>()) == math::tvec3<bool>{true, true, true}) ||
			   (math::equal(v1, v3, math::epsilon<float>()) == math::tvec3<bool>{true, true, true}) ||
			   (math::equal(v2, v3, math::epsilon<float>()) == math::tvec3<bool>{true, true, true}))
				continue;
		} // End if has position.

		// Prepare a triangle structure ready for population
		preparation_data_.triangle_count++;
		preparation_data_.triangle_data.resize(preparation_data_.triangle_count);
		triangle& triangle_data = preparation_data_.triangle_data[preparation_data_.triangle_count - 1];

		// Set triangle's subset information.
		triangle_data.data_group_id = src_tri.data_group_id;

		// For each index in the face
		for(std::uint32_t j = 0; j < 3; ++j)
		{
			// Extract the original index from the specified index buffer
			orig_index = src_tri.indices[j];

			// Retrieve the vertex record for the original vertex
			index = preparation_data_.vertex_records[orig_index];

			// Have we inserted this vertex into the vertex buffer previously?
			if(index == 0xFFFFFFFF)
			{
				// Vertex does not yet exist in the vertex buffer we are preparing
				// so copy the vertex in and record the index mapping for this vertex.
				index = preparation_data_.vertex_count++;
				preparation_data_.vertex_records[orig_index] = index;

				// Resize the output vertex buffer ready to hold this new data.
				std::size_t nInitialSize = preparation_data_.vertex_data.size();
				preparation_data_.vertex_data.resize(nInitialSize + vertex_stride);

				// Copy the data in.
				std::uint8_t* src_ptr = src_vertices_ptr + (orig_index * vertex_stride);
				std::uint8_t* dst_ptr = &preparation_data_.vertex_data[nInitialSize];
				memcpy(dst_ptr, src_ptr, vertex_stride);

				// Also record other pertenant details about this vertex.
				preparation_data_.vertex_flags.push_back(vertex_flags);

				// Clear any invalid normals (completely messes up HDR if ANY NaNs make
				// it this far)
				if(has_normal && source_has_normals)
				{
					float fnorm[4];
					gfx::vertex_unpack(fnorm, gfx::attribute::Normal, vertex_format_, dst_ptr);
					if(std::isnan(fnorm[0]) || std::isnan(fnorm[1]) || std::isnan(fnorm[2]))
						gfx::vertex_pack(fnorm, true, gfx::attribute::Normal, vertex_format_, dst_ptr);

				} // End if have normal

				// Grow the size of the bounding box
				if(has_position)
				{
					float fpos[4];
					gfx::vertex_unpack(fpos, gfx::attribute::Position, vertex_format_, dst_ptr);
					bbox_.add_point(math::vec3(fpos[0], fpos[1], fpos[2]));
				}

			} // End if vertex not recorded in this buffer yet

			// Copy the index in
			triangle_data.indices[j] = index;

		} // Next Index

	} // Next Face

	// Success!
	return true;
}

static void create_mesh(const gfx::vertex_layout& format, const generator::any_mesh& mesh,
						mesh::preparation_data& data, math::bbox& bbox)
{

	// Determine the correct offset to any relevant elements in the vertex
	bool has_position = format.has(gfx::attribute::Position);
	bool has_texcoord0 = format.has(gfx::attribute::TexCoord0);
	bool has_normals = format.has(gfx::attribute::Normal);
	bool has_tangents = format.has(gfx::attribute::Tangent);
	bool has_bitangents = format.has(gfx::attribute::Bitangent);
	std::uint16_t vertex_stride = format.getStride();

	auto triangle_count = generator::count(mesh.triangles());
	auto vertex_count = generator::count(mesh.vertices());
	data.triangle_count = std::uint32_t(triangle_count);
	data.vertex_count = std::uint32_t(vertex_count);

	// Allocate enough space for the new vertex and triangle data
	data.vertex_data.resize(data.vertex_count * vertex_stride);
	data.vertex_flags.resize(data.vertex_count);
	data.triangle_data.resize(data.triangle_count);

	std::uint8_t* current_vertex_ptr = &data.vertex_data[0];
	size_t i = 0;
	for(const auto& v : mesh.vertices())
	{

		math::vec3 position = v.position;
		math::vec4 normal = math::vec4(v.normal, 0.0f);
		math::vec2 texcoords0 = v.tex_coord;
		// Store vertex components
		if(has_position)
			gfx::vertex_pack(&position.x, false, gfx::attribute::Position, format, current_vertex_ptr,
							 std::uint32_t(i));
		if(has_normals)
			gfx::vertex_pack(&normal.x, true, gfx::attribute::Normal, format, current_vertex_ptr,
							 std::uint32_t(i));
		if(has_texcoord0)
			gfx::vertex_pack(&texcoords0.x, true, gfx::attribute::TexCoord0, format, current_vertex_ptr,
							 std::uint32_t(i));

		bbox.add_point(position);
		i++;
	}

	size_t tri_idx = 0;
	for(const auto& triangle : mesh.triangles())
	{
		const auto& indices = triangle.vertices;
		auto& tri = data.triangle_data[tri_idx];
		tri.indices[0] = std::uint32_t(indices[0]);
		tri.indices[1] = std::uint32_t(indices[1]);
		tri.indices[2] = std::uint32_t(indices[2]);

		tri_idx++;
	}

	// We need to generate binormals / tangents?
	data.compute_binormals = has_bitangents;
	data.compute_tangents = has_tangents;
}

bool mesh::create_cylinder(const gfx::vertex_layout& format, float radius, float height, std::uint32_t stacks,
						   std::uint32_t slices, mesh_create_origin origin, bool hardware_copy /* = true */)
{
	// Clear out old data.
	dispose();

	// We are in the process of preparing.
	prepare_status_ = mesh_status::preparing;
	vertex_format_ = format;

	using namespace generator;
	capped_cylinder_mesh_t cylinder(radius, height * 0.5, slices, stacks);
	math::quat rot(math::vec3(math::radians(-90.0f), 0.f, 0.0f));
	auto mesh = rotate_mesh(cylinder, rot);

	create_mesh(vertex_format_, mesh, preparation_data_, bbox_);
	// Finish up
	return end_prepare(hardware_copy, false, false);
}

bool mesh::create_capsule(const gfx::vertex_layout& format, float radius, float height, std::uint32_t stacks,
						  std::uint32_t slices, mesh_create_origin origin, bool hardware_copy /* = true */)
{
	// Clear out old data.
	dispose();

	// We are in the process of preparing.
	prepare_status_ = mesh_status::preparing;
	vertex_format_ = format;

	using namespace generator;
	capsule_mesh_t capsule(radius, height * 0.5, slices, stacks);
	math::quat rot(math::vec3(math::radians(-90.0f), 0.f, 0.0f));
	auto mesh = rotate_mesh(capsule, rot);

	create_mesh(vertex_format_, mesh, preparation_data_, bbox_);
	// Finish up
	return end_prepare(hardware_copy, false, false);
}

bool mesh::create_sphere(const gfx::vertex_layout& format, float radius, std::uint32_t stacks,
						 std::uint32_t slices, mesh_create_origin origin, bool hardware_copy /* = true */)
{
	// Clear out old data.
	dispose();

	// We are in the process of preparing.
	prepare_status_ = mesh_status::preparing;
	vertex_format_ = format;

	using namespace generator;
	sphere_mesh_t sphere(radius, slices, stacks);
	math::quat rot(math::vec3(math::radians(-90.0f), 0.f, 0.0f));
	auto mesh = rotate_mesh(sphere, rot);

	create_mesh(vertex_format_, mesh, preparation_data_, bbox_);
	// Finish up
	return end_prepare(hardware_copy, false, false);
}

bool mesh::create_torus(const gfx::vertex_layout& format, float outer_radius, float inner_radius,
						std::uint32_t bands, std::uint32_t sides, mesh_create_origin origin,
						bool hardware_copy /* = true */)
{
	// Clear out old data.
	dispose();

	// We are in the process of preparing.
	prepare_status_ = mesh_status::preparing;
	vertex_format_ = format;

	using namespace generator;
	torus_mesh_t torus(inner_radius, outer_radius, sides, bands);
	math::quat rot(math::vec3(math::radians(-90.0f), 0.f, 0.0f));
	auto mesh = rotate_mesh(torus, rot);

	create_mesh(vertex_format_, mesh, preparation_data_, bbox_);
	// Finish up
	return end_prepare(hardware_copy, false, false);
}

bool mesh::create_teapot(const gfx::vertex_layout& format, bool hardware_copy /*= true*/)
{
	// Clear out old data.
	dispose();

	// We are in the process of preparing.
	prepare_status_ = mesh_status::preparing;
	vertex_format_ = format;

	using namespace generator;
	teapot_mesh_t teapot;
	math::quat rot(math::vec3(math::radians(-90.0f), 0.f, 0.0f));
	auto mesh = rotate_mesh(teapot, rot);

	create_mesh(vertex_format_, mesh, preparation_data_, bbox_);
	// Finish up
	return end_prepare(hardware_copy, false, false);
}

bool mesh::create_icosahedron(const gfx::vertex_layout& format, bool hardware_copy /*= true*/)
{
	// Clear out old data.
	dispose();

	// We are in the process of preparing.
	prepare_status_ = mesh_status::preparing;
	vertex_format_ = format;

	using namespace generator;
	icosahedron_mesh_t icosahedron;
	math::quat rot(math::vec3(math::radians(-90.0f), 0.f, 0.0f));
	auto mesh = rotate_mesh(icosahedron, rot);

	create_mesh(vertex_format_, mesh, preparation_data_, bbox_);
	// Finish up
	return end_prepare(hardware_copy, false, false);
}

bool mesh::create_dodecahedron(const gfx::vertex_layout& format, bool hardware_copy /*= true*/)
{
	// Clear out old data.
	dispose();

	// We are in the process of preparing.
	prepare_status_ = mesh_status::preparing;
	vertex_format_ = format;

	using namespace generator;
	dodecahedron_mesh_t dodecahedron;
	math::quat rot(math::vec3(math::radians(-90.0f), 0.f, 0.0f));
	auto mesh = rotate_mesh(dodecahedron, rot);

	create_mesh(vertex_format_, mesh, preparation_data_, bbox_);
	// Finish up
	return end_prepare(hardware_copy, false, false);
}

bool mesh::create_icosphere(const gfx::vertex_layout& format, int tesselation_level,
							bool hardware_copy /*= true*/)
{
	// Clear out old data.
	dispose();

	// We are in the process of preparing.
	prepare_status_ = mesh_status::preparing;
	vertex_format_ = format;

	using namespace generator;
	ico_sphere_mesh_t icosphere(1, tesselation_level + 1);
	math::quat rot(math::vec3(math::radians(-90.0f), 0.f, 0.0f));
	auto mesh = rotate_mesh(icosphere, rot);

	create_mesh(vertex_format_, mesh, preparation_data_, bbox_);
	// Finish up
	return end_prepare(hardware_copy, false, false);
}

bool mesh::create_cone(const gfx::vertex_layout& format, float radius, float radius_tip, float height,
					   std::uint32_t stacks, std::uint32_t slices, mesh_create_origin origin,
					   bool hardware_copy /* = true */)
{
	// Clear out old data.
	dispose();

	// We are in the process of preparing.
	prepare_status_ = mesh_status::preparing;
	vertex_format_ = format;

	using namespace generator;
	capped_cone_mesh_t cone(radius, 1.0, stacks, slices);
	math::quat rot(math::vec3(math::radians(-90.0f), 0.f, 0.0f));
	auto mesh = rotate_mesh(cone, rot);

	create_mesh(vertex_format_, mesh, preparation_data_, bbox_);
	// Finish up
	return end_prepare(hardware_copy, false, false);
}

bool mesh::create_plane(const gfx::vertex_layout& format, float width, float height,
						std::uint32_t width_segments, std::uint32_t height_segments,
						mesh_create_origin origin, bool hardware_copy /* = true */)
{
	// Clear out old data.
	dispose();

	// We are in the process of preparing.
	prepare_status_ = mesh_status::preparing;
	vertex_format_ = format;

	using namespace generator;
	plane_mesh_t plane({width * 0.5f, height * 0.5f}, {width_segments, height_segments});
	math::quat rot(math::vec3(math::radians(-90.0f), 0.f, 0.0f));
	auto mesh = rotate_mesh(plane, rot);

	create_mesh(vertex_format_, mesh, preparation_data_, bbox_);
	// Finish up
	return end_prepare(hardware_copy, false, false);
}

bool mesh::create_cube(const gfx::vertex_layout& format, float width, float height, float depth,
					   std::uint32_t width_segments, std::uint32_t height_segments,
					   std::uint32_t depth_segments, mesh_create_origin origin,
					   bool hardware_copy /* = true */)
{
	// Clear out old data.
	dispose();

	// We are in the process of preparing.
	prepare_status_ = mesh_status::preparing;
	vertex_format_ = format;

	using namespace generator;
	box_mesh_t box({width * 0.5f, height * 0.5f, depth * 0.5f},
				   {width_segments, height_segments, depth_segments});
	math::quat rot(math::vec3(math::radians(-90.0f), 0.f, 0.0f));
	auto mesh = rotate_mesh(box, rot);

	create_mesh(vertex_format_, mesh, preparation_data_, bbox_);
	// Finish up
	return end_prepare(hardware_copy, false, false);
}

bool mesh::end_prepare(bool hardware_copy /* = true */, bool weld /* = true */, bool optimize /* = true */,
					   bool build_buffers /*= true*/)
{
	// Were we previously preparing?
	if(prepare_status_ != mesh_status::preparing)
	{
		APPLOG_ERROR("Attempting to call 'end_prepare' on a mesh without first "
					 "calling 'prepareMesh' is not "
					 "allowed.\n");
		return false;

	} // End if previously preparing

	// Scan the preparation data for degenerate triangles.
	std::uint16_t position_offset = vertex_format_.getOffset(gfx::attribute::Position);
	// std::uint16_t vertex_stride = _vertex_format.getStride();
	std::uint8_t* src_vertices_ptr = &preparation_data_.vertex_data[0] + position_offset;
	for(std::uint32_t i = 0; i < preparation_data_.triangle_count; ++i)
	{
		triangle& tri = preparation_data_.triangle_data[i];
		math::vec3 v1;
		float vf1[4];
		gfx::vertex_unpack(vf1, gfx::attribute::Position, vertex_format_, src_vertices_ptr, tri.indices[0]);
		math::vec3 v2;
		float vf2[4];
		gfx::vertex_unpack(vf2, gfx::attribute::Position, vertex_format_, src_vertices_ptr, tri.indices[1]);
		math::vec3 v3;
		float vf3[4];
		gfx::vertex_unpack(vf3, gfx::attribute::Position, vertex_format_, src_vertices_ptr, tri.indices[2]);
		memcpy(&v1[0], vf1, 3 * sizeof(float));
		memcpy(&v2[0], vf2, 3 * sizeof(float));
		memcpy(&v3[0], vf3, 3 * sizeof(float));

		math::vec3 c = math::cross(v2 - v1, v3 - v1);
		if(math::length2(c) < (4.0f * 0.000001f * 0.000001f))
			tri.flags |= triangle_flags::degenerate;

	} // Next triangle

	// Process the vertex data in order to generate any additional components that
	// may be necessary
	// (i.e. Normal, Binormal and Tangent)
	if(!generate_vertex_components(weld))
		return false;

	// Allocate the system memory vertex buffer ready for population.
	vertex_count_ = preparation_data_.vertex_count;
	system_vb_ = new std::uint8_t[vertex_count_ * vertex_format_.getStride()];

	// Copy vertex data into the new buffer and dispose of the temporary data.
	memcpy(system_vb_, &preparation_data_.vertex_data[0], vertex_count_ * vertex_format_.getStride());
	preparation_data_.vertex_data.clear();
	preparation_data_.vertex_flags.clear();
	preparation_data_.vertex_count = 0;

	// Allocate the memory for our system memory index buffer
	face_count_ = preparation_data_.triangle_count;
	system_ib_ = new std::uint32_t[face_count_ * 3];

	// math::transform triangle indices, material and data group information
	// to the final triangle data arrays. We keep the latter two handy so
	// that we know precisely which subset each triangle belongs to.
	triangle_data_.resize(face_count_);
	std::uint32_t* dst_indices_ptr = system_ib_;
	for(std::uint32_t i = 0; i < face_count_; ++i)
	{
		// Copy indices.
		const triangle& tri_in = preparation_data_.triangle_data[i];
		*dst_indices_ptr++ = tri_in.indices[0];
		*dst_indices_ptr++ = tri_in.indices[1];
		*dst_indices_ptr++ = tri_in.indices[2];

		// Copy triangle subset information.
		mesh_subset_key& tri_out = triangle_data_[i];
		tri_out.data_group_id = tri_in.data_group_id;

	} // Next triangle
	preparation_data_.triangle_count = 0;
	preparation_data_.triangle_data.clear();

	// Index data has been updated and potentially needs to be serialized.
	if(build_buffers)
		build_vb(hardware_copy);

	// Skin binding data has potentially been updated and needs to be serialized.

	// Finally perform the final sort of the mesh data in order
	// to build the index buffer and subset tables.
	if(!sort_mesh_data(optimize, hardware_copy, build_buffers))
		return false;

	// The mesh is now prepared
	prepare_status_ = mesh_status::prepared;
	hardware_mesh_ = hardware_copy;
	optimize_mesh_ = optimize;

	// Success!
	return true;
}

void mesh::build_vb(bool hardware_copy)
{
	// A video memory copy of the mesh was requested?
	if(hardware_copy)
	{
		// Calculate the required size of the vertex buffer
		std::uint32_t buffer_size = vertex_count_ * vertex_format_.getStride();

		const gfx::memory_view* mem = gfx::copy(system_vb_, static_cast<std::uint32_t>(buffer_size));
		hardware_vb_ = std::make_shared<gfx::vertex_buffer>(mem, vertex_format_);

	} // End if video memory vertex buffer required
}

void mesh::build_ib(bool hardware_copy)
{
	// Hardware versions of the final buffer were required?
	if(hardware_copy)
	{
		// Calculate the required size of the index buffer
		std::uint32_t buffer_size = face_count_ * 3 * sizeof(std::uint32_t);

		// Allocate hardware buffer if required (i.e. it does not already exist).
		if(!hardware_ib_)
		{
			const gfx::memory_view* mem = gfx::copy(system_ib_, static_cast<std::uint32_t>(buffer_size));
			hardware_ib_ = std::make_shared<gfx::index_buffer>(mem, BGFX_BUFFER_INDEX32);
		} // End if not allocated
		else
		{
			auto ib = std::static_pointer_cast<gfx::index_buffer>(hardware_ib_);
			if(!ib->is_valid())
			{
				const gfx::memory_view* mem = gfx::copy(system_ib_, static_cast<std::uint32_t>(buffer_size));
				hardware_ib_ = std::make_shared<gfx::index_buffer>(mem, BGFX_BUFFER_INDEX32);
			}
		}

	} // End if hardware buffer required
}

bool mesh::sort_mesh_data(bool optimize, bool hardware_copy, bool build_buffer)
{
	std::map<mesh_subset_key, std::uint32_t> subset_sizes;
	std::map<mesh_subset_key, std::uint32_t>::iterator it_subset_size;
	data_group_subset_map_t::iterator it_data_group;

	// Clear out any old data EXCEPT the old subset index
	// We'll need this in order to understand how to update
	// the material reference counting later on.
	data_groups_.clear();
	subset_lookup_.clear();

	// Our first job is to collate all the various subsets and also
	// to determine how many triangles should exist in each.
	for(std::uint32_t i = 0; i < face_count_; ++i)
	{
		const mesh_subset_key& subset_key = triangle_data_[i];

		// Already contains this material / data group combination?
		it_subset_size = subset_sizes.find(subset_key);
		if(it_subset_size == subset_sizes.end())
		{
			// Add a new entry for this subset
			subset_sizes[subset_key] = 1;

		} // End if !exists
		else
		{
			// Update the existing subset
			it_subset_size->second++;

		} // End if already encountered

	} // Next triangle

	// We should now have a complete list of subsets and the number of triangles
	// which should exist in each. Populate mesh subset table and update start /
	// count
	// values so that we can correctly generate the new sorted index buffer.
	std::int32_t counter = 0;
	subset_array_t new_subsets;
	for(it_subset_size = subset_sizes.begin(); it_subset_size != subset_sizes.end(); ++it_subset_size)
	{
		// Construct a new subset and populate with initial construction
		// values including the expected starting face location.
		const mesh_subset_key& key = it_subset_size->first;
		auto* sub = new subset();
		sub->data_group_id = key.data_group_id;
		sub->face_start = counter;
		counter += it_subset_size->second;

		// Ensure that "FaceCount" defaults to zero at this point
		// so that we can keep a running total during the final buffer
		// construction.
		sub->face_count = 0;

		// Also reset vertex values as appropriate (will grow
		// using standard 'bounding' value insert).
		sub->vertex_start = 0x7FFFFFFF;
		sub->vertex_count = 0;

		// Add to list for fast linear access, and lookup table
		// for sorted search.
		new_subsets.push_back(sub);
		subset_lookup_[key] = sub;

		// Add to data group lookup table
		it_data_group = data_groups_.find(sub->data_group_id);
		if(it_data_group == data_groups_.end())
			data_groups_[sub->data_group_id].push_back(sub);
		else
			it_data_group->second.push_back(sub);

	} // Next subset

	// Allocate space for new sorted index buffer and face re-map information
	std::uint32_t* src_indices_ptr = system_ib_;
	auto dst_indices_ptr = new std::uint32_t[face_count_ * 3];
	auto face_remap_ptr = new std::uint32_t[face_count_];

	// Start building new indices
	std::uint32_t index = 0;
	std::uint32_t index_start = 0;
	for(std::uint32_t i = 0; i < face_count_; ++i)
	{
		// Find a matching subset for this triangle
		subset* sub = subset_lookup_[triangle_data_[i]];

		// Copy index data over to new buffer, taking care to record the correct
		// vertex values as required. We'll temporarily use VertexStart and
		// VertexCount
		// as a MathUtility::minValue/max record that we'll come round and correct
		// later.
		index_start = (static_cast<std::uint32_t>(sub->face_start) + sub->face_count) * 3;

		// Index[0]
		index = static_cast<std::uint32_t>(*src_indices_ptr++);
		if(static_cast<std::int32_t>(index) < sub->vertex_start)
			sub->vertex_start = static_cast<std::int32_t>(index);
		if(index > sub->vertex_count)
			sub->vertex_count = index;
		dst_indices_ptr[index_start++] = index;

		// Index[1]
		index = static_cast<std::uint32_t>(*src_indices_ptr++);
		if(static_cast<std::int32_t>(index) < sub->vertex_start)
			sub->vertex_start = static_cast<std::int32_t>(index);
		if(index > sub->vertex_count)
			sub->vertex_count = index;
		dst_indices_ptr[index_start++] = index;

		// Index[2]
		index = static_cast<std::uint32_t>(*src_indices_ptr++);
		if(static_cast<std::int32_t>(index) < sub->vertex_start)
			sub->vertex_start = static_cast<std::int32_t>(index);
		if(index > sub->vertex_count)
			sub->vertex_count = index;
		dst_indices_ptr[index_start++] = index;

		// Store face re-map information so that we can remap data as required
		face_remap_ptr[i] = static_cast<std::uint32_t>(sub->face_start) + sub->face_count;

		// We have now recorded a triangle in this subset
		sub->face_count++;

	} // Next triangle

	auto sort_predicate = [](const subset* lhs, const subset* rhs) {
		return lhs->data_group_id < rhs->data_group_id;
	};

	// Sort the subset list in order to ensure that all subsets with the same
	// materials and data groups are added next to one another in the final
	// index buffer. This ensures that we can batch draw all subsets that share
	// common properties.
	std::sort(new_subsets.begin(), new_subsets.end(), sort_predicate);

	// Perform the same sort on the data group and material mapped lists.
	// Also take the time to build the final list of materials used by this mesh
	// (render control batching system requires that we cache this information in
	// a
	// specific format).
	for(it_data_group = data_groups_.begin(); it_data_group != data_groups_.end(); ++it_data_group)
		std::sort(it_data_group->second.begin(), it_data_group->second.end(), sort_predicate);

	// Optimize the faces as we transfer to the final destination index buffer
	// if requested. Otherwise, just copy them over directly.
	src_indices_ptr = dst_indices_ptr;
	dst_indices_ptr = system_ib_;
	counter = 0;
	for(auto subset : new_subsets)
	{
		// Note: Remember that at this stage, the subset's 'vertex_count' member
		// still describes
		// a 'max' vertex (not a count)... We're correcting this later.
		if(optimize)
			build_optimized_index_buffer(subset, src_indices_ptr + (subset->face_start * 3), dst_indices_ptr,
										 static_cast<std::uint32_t>(subset->vertex_start),
										 static_cast<std::uint32_t>(subset->vertex_count));
		else
			memcpy(dst_indices_ptr, src_indices_ptr + (subset->face_start * 3),
				   static_cast<std::size_t>(subset->face_count) * 3 * sizeof(std::uint32_t));

		// This subset's starting face now refers to its location
		// in the final destination buffer rather than the temporary one.
		subset->face_start = counter;
		counter += subset->face_count;

		// Move on to output next sorted subset.
		dst_indices_ptr += subset->face_count * 3;

	} // Next subset

	// Clean up.
	checked_array_delete(src_indices_ptr);

	// Rebuild the additional triangle data based on the newly sorted
	// subset data, and also convert the previously recorded maximum
	// vertex value (stored in "vertex_count") into its final form
	for(auto subset : new_subsets)
	{
		// Convert vertex "Max" to "Count"
		subset->vertex_count = (subset->vertex_count - static_cast<std::uint32_t>(subset->vertex_start)) + 1;

		// Update additional triangle data array.
		auto fstart = static_cast<std::uint32_t>(subset->face_start);
		for(std::uint32_t j = fstart; j < (fstart + subset->face_count); ++j)
		{
			triangle_data_[j].data_group_id = subset->data_group_id;

		} // Next triangle

	} // Next subset

	// We're done with the remap data.
	// TODO: Note - we don't actually use the face remap information at
	// the moment, but it could be useful?
	checked_array_delete(face_remap_ptr);

	// Hardware versions of the final buffer were required?
	if(build_buffer)
		build_ib(hardware_copy);
	// Index data and subsets have been updated and potentially need to be
	// serialized.

	// Destroy old subset data.
	for(auto subset : mesh_subsets_)
		checked_delete(subset);
	mesh_subsets_.clear();

	// Use the new subset data.
	mesh_subsets_ = new_subsets;

	// Success!
	return true;
}

float mesh::find_vertex_optimizer_score(const optimizer_vertex_info* vertex_info_ptr)
{
	float score = 0.0f;

	// Do any remaining triangles use this vertex?
	if(vertex_info_ptr->unused_triangle_references == 0)
		return -1.0f;

	std::int32_t cache_position = vertex_info_ptr->cache_position;
	if(cache_position < 0)
	{

		// Vertex is not in FIFO cache - no score.
	}
	else
	{
		if(cache_position < 3)
		{
			// This vertex was used in the last triangle,
			// so it has a fixed score, whichever of the three
			// it's in. Otherwise, you can get very different
			// answers depending on whether you add
			// the triangle 1,2,3 or 3,1,2 - which is silly.
			score = MeshOptimizer::LastTriScore;
		}
		else
		{
			// Points for being high in the cache.
			const float scaler = 1.0f / (MeshOptimizer::MaxVertexCacheSize - 3);
			score = 1.0f - (cache_position - 3) * scaler;
			score = math::pow(score, MeshOptimizer::CacheDecayPower);
		}

	} // End if already in vertex cache

	// Bonus points for having a low number of tris still to
	// use the vert, so we get rid of lone verts quickly.
	float valence_boost = math::pow(static_cast<float>(vertex_info_ptr->unused_triangle_references),
									-MeshOptimizer::ValenceBoostPower);
	score += MeshOptimizer::ValenceBoostScale * valence_boost;

	// Return the final score
	return score;
}

void mesh::build_optimized_index_buffer(const subset* subset, std::uint32_t* src_buffer_ptr,
										std::uint32_t* dest_buffer_ptr, std::uint32_t min_vertex,
										std::uint32_t max_vertex)
{
	float best_score = 0.0f, score;
	std::int32_t best_triangle = -1;
	std::uint32_t vertex_cache_size = 0;
	std::uint32_t index, triangle_index, temp;

	// Declare vertex cache storage (plus one to allow them to drop "off the end")
	std::uint32_t vertex_cache_ptr[MeshOptimizer::MaxVertexCacheSize + 1];
	std::memset(vertex_cache_ptr, 0, sizeof(vertex_cache_ptr));
	// First allocate enough room for the optimization information for each vertex
	// and triangle
	std::uint32_t vertex_count = (max_vertex - min_vertex) + 1;
	auto vertex_info_ptr = new optimizer_vertex_info[vertex_count];
	auto triangle_info_ptr = new optimizer_triangle_info[subset->face_count];

	// The first pass is to initialize the vertex information with information
	// about the
	// faces which reference them.
	for(std::uint32_t i = 0; i < subset->face_count; ++i)
	{
		index = src_buffer_ptr[i * 3] - min_vertex;
		vertex_info_ptr[index].unused_triangle_references++;
		vertex_info_ptr[index].triangle_references.push_back(i);
		index = src_buffer_ptr[(i * 3) + 1] - min_vertex;
		vertex_info_ptr[index].unused_triangle_references++;
		vertex_info_ptr[index].triangle_references.push_back(i);
		index = src_buffer_ptr[(i * 3) + 2] - min_vertex;
		vertex_info_ptr[index].unused_triangle_references++;
		vertex_info_ptr[index].triangle_references.push_back(i);

	} // Next triangle

	// Initialize vertex scores
	for(std::uint32_t i = 0; i < vertex_count; ++i)
		vertex_info_ptr[i].vertex_score = find_vertex_optimizer_score(&vertex_info_ptr[i]);

	// Compute the score for each triangle, and record the triangle with the best
	// score
	for(std::uint32_t i = 0; i < subset->face_count; ++i)
	{
		// The triangle score is the sum of the scores of each of
		// its three vertices.
		index = src_buffer_ptr[i * 3] - min_vertex;
		score = vertex_info_ptr[index].vertex_score;
		index = src_buffer_ptr[(i * 3) + 1] - min_vertex;
		score += vertex_info_ptr[index].vertex_score;
		index = src_buffer_ptr[(i * 3) + 2] - min_vertex;
		score += vertex_info_ptr[index].vertex_score;
		triangle_info_ptr[i].triangle_score = score;

		// Record the triangle with the highest score
		if(score > best_score)
		{
			best_score = score;
			best_triangle = static_cast<std::int32_t>(i);

		} // End if better than previous score

	} // Next triangle

	// Now we can start adding triangles, beginning with the previous highest
	// scoring triangle.
	for(std::uint32_t i = 0; i < subset->face_count; ++i)
	{
		// If we don't know the best triangle, for whatever reason, find it
		if(best_triangle < 0)
		{
			best_triangle = -1;
			best_score = 0.0f;

			// Iterate through the entire list of un-added faces
			for(std::uint32_t j = 0; j < subset->face_count; ++j)
			{
				if(!triangle_info_ptr[j].added)
				{
					score = triangle_info_ptr[j].triangle_score;

					// Record the triangle with the highest score
					if(score > best_score)
					{
						best_score = score;
						best_triangle = static_cast<std::int32_t>(j);

					} // End if better than previous score

				} // End if not added

			} // Next triangle

		} // End if best triangle is not known

		// Use the best scoring triangle from last pass and reset score keeping
		triangle_index = static_cast<std::uint32_t>(best_triangle);
		optimizer_triangle_info* tri_ptr = &triangle_info_ptr[triangle_index];
		best_triangle = -1;
		best_score = 0.0f;

		// This triangle can be added to the 'draw' list, and each
		// of the vertices it references should be updated.
		tri_ptr->added = true;
		for(std::uint32_t j = 0; j < 3; ++j)
		{
			// Extract the vertex index and store in the index buffer
			index = src_buffer_ptr[(triangle_index * 3) + j];
			*dest_buffer_ptr++ = index;

			// Adjust the index so that it points into our info buffer
			// rather than the actual source vertex itself.
			index = index - min_vertex;

			// Retrieve the referenced vertex information
			optimizer_vertex_info* vert_ptr = &vertex_info_ptr[index];

			// Reduce the 'valence' of this vertex (one less triangle is now
			// referencing)
			vert_ptr->unused_triangle_references--;

			// Remove this triangle from the list of references in the vertex
			auto itReference = std::find(vert_ptr->triangle_references.begin(),
										 vert_ptr->triangle_references.end(), triangle_index);
			if(itReference != vert_ptr->triangle_references.end())
				vert_ptr->triangle_references.erase(itReference);

			// Now we must update the vertex cache to include this vertex. If it was
			// already in the cache, it should be moved to the head, otherwise it
			// should
			// be inserted (pushing one off the end).
			if(vert_ptr->cache_position == -1)
			{
				// Not in the vertex cache, insert it at the head.
				if(vertex_cache_size > 0)
				{
					// First shuffle EVERYONE up by one position in the cache.
					memmove(&vertex_cache_ptr[1], &vertex_cache_ptr[0],
							vertex_cache_size * sizeof(std::uint32_t));

				} // End if any vertices exist in the cache

				// Grow the cache if applicable
				if(vertex_cache_size < MeshOptimizer::MaxVertexCacheSize)
					vertex_cache_size++;
				else
				{
					// Set the associated index of the vertex which dropped "off the end"
					// of the cache.
					vertex_info_ptr[vertex_cache_ptr[vertex_cache_size]].cache_position = -1;

				} // End if no more room

				// Overwrite the first entry
				vertex_cache_ptr[0] = index;

			} // End if not in cache
			else if(vert_ptr->cache_position > 0)
			{
				// Already in the vertex cache, move it to the head.
				// Note : If the cache position is already 0, we just ignore
				// it... hence the above 'else if' rather than just 'else'.
				if(vert_ptr->cache_position == 1)
				{
					// We were in the second slot, just swap the two
					temp = vertex_cache_ptr[0];
					vertex_cache_ptr[0] = index;
					vertex_cache_ptr[1] = temp;

				} // End if simple swap
				else
				{
					// Shuffle EVERYONE up who came before us.
					memmove(&vertex_cache_ptr[1], &vertex_cache_ptr[0],
							static_cast<std::size_t>(vert_ptr->cache_position) * sizeof(std::uint32_t));

					// Insert this vertex at the head
					vertex_cache_ptr[0] = index;

				} // End if memory move required

			} // End if already in cache

			// Update the cache position records for all vertices in the cache
			for(std::uint32_t k = 0; k < vertex_cache_size; ++k)
				vertex_info_ptr[vertex_cache_ptr[k]].cache_position = static_cast<std::int32_t>(k);

		} // Next Index

		// Recalculate the of all vertices contained in the cache
		for(std::uint32_t j = 0; j < vertex_cache_size; ++j)
		{
			optimizer_vertex_info* vert_ptr = &vertex_info_ptr[vertex_cache_ptr[j]];
			vert_ptr->vertex_score = find_vertex_optimizer_score(vert_ptr);

		} // Next entry in the vertex cache

		// Update the score of the triangles which reference this vertex
		// and record the highest scoring.
		for(std::uint32_t j = 0; j < vertex_cache_size; ++j)
		{
			optimizer_vertex_info* vert_ptr = &vertex_info_ptr[vertex_cache_ptr[j]];

			// For each triangle referenced
			for(std::uint32_t k = 0; k < vert_ptr->unused_triangle_references; ++k)
			{
				triangle_index = vert_ptr->triangle_references[k];
				tri_ptr = &triangle_info_ptr[triangle_index];
				score = vertex_info_ptr[src_buffer_ptr[(triangle_index * 3)] - min_vertex].vertex_score;
				score += vertex_info_ptr[src_buffer_ptr[(triangle_index * 3) + 1] - min_vertex].vertex_score;
				score += vertex_info_ptr[src_buffer_ptr[(triangle_index * 3) + 2] - min_vertex].vertex_score;
				tri_ptr->triangle_score = score;

				// Highest scoring so far?
				if(score > best_score)
				{
					best_score = score;
					best_triangle = static_cast<std::int32_t>(triangle_index);

				} // End if better than previous score

			} // Next triangle

		} // Next entry in the vertex cache

	} // Next triangle to Add

	// Destroy the temporary arrays
	checked_array_delete(vertex_info_ptr);
	checked_array_delete(triangle_info_ptr);
}

void mesh::bind_render_buffers()
{
	for(size_t i = 0; i < mesh_subsets_.size(); ++i)
	{
		bind_render_buffers_for_subset(std::uint32_t(i));
	}
}

void mesh::bind_render_buffers_for_subset(std::uint32_t data_group_id)
{
	// Attempt to find a matching subset.
	auto it = subset_lookup_.find(mesh_subset_key(data_group_id));
	if(it == subset_lookup_.end())
		return;

	// Process and draw all subsets of the mesh that use the specified material.
	std::uint32_t subset_vert_start = 0;
	std::uint32_t subset_vert_end = 0;

	subset* subset = it->second;
	auto face_start = static_cast<std::uint32_t>(subset->face_start);
	auto face_count = subset->face_count;
	auto vertex_start = static_cast<std::uint32_t>(subset->vertex_start);
	auto vertex_end = vertex_start + subset->vertex_count - 1;
	std::uint32_t vertex_count = 0;
	// Vertex start/end is a little more complex, but can be computed
	// using a containment style test. First precompute some values to
	// make the tests a little simpler.
	subset_vert_start = static_cast<std::uint32_t>(subset->vertex_start);
	subset_vert_end = subset_vert_start + subset->vertex_count - 1;

	// Perform the containment tests
	if(subset_vert_start < vertex_start)
		vertex_start = subset_vert_start;
	if(subset_vert_start > vertex_end)
		vertex_end = subset_vert_start;
	if(subset_vert_end < vertex_start)
		vertex_start = subset_vert_end;
	if(subset_vert_end > vertex_end)
		vertex_end = subset_vert_end;

	// Compute the final vertex count.
	vertex_count = (vertex_end - vertex_start) + 1;

	// Render any batched data.
	if(face_count > 0)
		bind_mesh_data(face_start, face_count, vertex_start, vertex_count);
}

void mesh::bind_mesh_data(std::uint32_t face_start, std::uint32_t face_count, std::uint32_t vertex_start,
						  std::uint32_t vertex_count)
{
	(void)vertex_start;
	std::uint32_t index_start = face_start * 3;
	std::uint32_t index_count = face_count * 3;
	// Hardware or software rendering?
	if(hardware_mesh_)
	{
		// Render using hardware streams
		auto vb = std::static_pointer_cast<gfx::vertex_buffer>(hardware_vb_);
		auto ib = std::static_pointer_cast<gfx::index_buffer>(hardware_ib_);

		gfx::set_vertex_buffer(0, vb->native_handle(), 0, vertex_count);
		gfx::set_index_buffer(ib->native_handle(), index_start, index_count);

	} // End if has hardware copy
	else
	{

		if(vertex_count == gfx::get_avail_transient_vertex_buffer(vertex_count, vertex_format_))
		{
			gfx::transient_vertex_buffer vb;
			gfx::alloc_transient_vertex_buffer(&vb, vertex_count, vertex_format_);
			memcpy(vb.data, system_vb_, vb.size);
			gfx::set_vertex_buffer(0, &vb, 0, vertex_count);
		}

		if(index_count == gfx::get_avail_transient_index_buffer(index_count))
		{
			gfx::transient_index_buffer ib;
			gfx::alloc_transient_index_buffer(&ib, index_count);
			memcpy(ib.data, system_ib_, ib.size);
			gfx::set_index_buffer(&ib, index_start, index_count);
		}

	} // End if software only copy
}

bool mesh::generate_vertex_components(bool weld)
{
	// Vertex normals were requested (and at least some were not yet provided?)
	if(force_normal_generation_ || preparation_data_.compute_normals)
	{
		// Generate the adjacency information for vertex normal computation
		std::vector<std::uint32_t> adjacency;
		if(!generate_adjacency(adjacency))
		{
			APPLOG_ERROR("Failed to generate adjacency buffer mesh containing {0} faces.\n",
						 preparation_data_.triangle_count);
			return false;

		} // End if failed to generate
		if(force_barycentric_generation_ || preparation_data_.compute_barycentric)
		{
			// Generate any vertex barycentric coords that have not been provided
			if(!generate_vertex_barycentrics(&adjacency.front()))
			{
				APPLOG_ERROR("Failed to generate vertex barycentric coords for mesh "
							 "containing {0} faces.\n",
							 preparation_data_.triangle_count);
				return false;

			} // End if failed to generate

		} // End if compute

		// Generate any vertex normals that have not been provided
		if(!generate_vertex_normals(&adjacency.front()))
		{
			APPLOG_ERROR("Failed to generate vertex normals for mesh containing {0} faces.\n",
						 preparation_data_.triangle_count);
			return false;

		} // End if failed to generate

	} // End if compute

	// Weld vertices at this point
	if(weld)
	{
		if(!weld_vertices())
		{
			APPLOG_ERROR("Failed to weld vertices for mesh containing {0} faces.\n",
						 preparation_data_.triangle_count);
			return false;

		} // End if failed to weld

	} // End if optional weld

	// Binormals and / or tangents were requested (and at least some where not yet
	// provided?)
	if(force_tangent_generation_ || preparation_data_.compute_binormals || preparation_data_.compute_tangents)
	{
		// Requires normals
		if(vertex_format_.has(gfx::attribute::Normal))
		{
			// Generate any vertex tangents that have not been provided
			if(!generate_vertex_tangents())
			{
				APPLOG_ERROR("Failed to generate vertex tangents for mesh containing "
							 "{0} faces.\n",
							 preparation_data_.triangle_count);
				return false;

			} // End if failed to generate

		} // End if has normals

	} // End if compute

	// Success!
	return true;
}

bool mesh::generate_vertex_normals(std::uint32_t* adjacency_ptr,
								   std::vector<std::uint32_t>* remap_array_ptr /* = nullptr */)
{
	std::uint32_t start_tri, previous_tri, current_tri;
	math::vec3 vec_edge1, vec_edge2, vec_normal;
	std::uint32_t i, j, k, index;

	// Get access to useful data offset information.
	std::uint16_t position_offset = vertex_format_.getOffset(gfx::attribute::Position);
	bool has_normals = vertex_format_.has(gfx::attribute::Normal);
	std::uint16_t vertex_stride = vertex_format_.getStride();

	// Final format requests vertex normals?
	if(!has_normals)
		return true;

	// Size the remap array accordingly and populate it with the default mapping.
	std::uint32_t original_vertex_count = preparation_data_.vertex_count;
	if(remap_array_ptr)
	{
		remap_array_ptr->resize(preparation_data_.vertex_count);
		for(i = 0; i < preparation_data_.vertex_count; ++i)
			(*remap_array_ptr)[i] = i;

	} // End if supplied

	// Pre-compute surface normals for each triangle
	std::uint8_t* src_vertices_ptr = &preparation_data_.vertex_data[0];
	auto* normals_ptr = new math::vec3[preparation_data_.triangle_count];
	memset(normals_ptr, 0, preparation_data_.triangle_count * sizeof(math::vec3));
	for(i = 0; i < preparation_data_.triangle_count; ++i)
	{
		// Retrieve positions of each referenced vertex.
		const triangle& tri = preparation_data_.triangle_data[i];
		const auto* v1 = reinterpret_cast<const math::vec3*>(
			src_vertices_ptr + (tri.indices[0] * vertex_stride) + position_offset);
		const auto* v2 = reinterpret_cast<const math::vec3*>(
			src_vertices_ptr + (tri.indices[1] * vertex_stride) + position_offset);
		const auto* v3 = reinterpret_cast<const math::vec3*>(
			src_vertices_ptr + (tri.indices[2] * vertex_stride) + position_offset);

		// Compute the two edge vectors required for generating our normal
		// We normalize here to prevent problems when the triangles are very small.
		vec_edge1 = math::normalize(*v2 - *v1);
		vec_edge2 = math::normalize(*v3 - *v1);

		// Generate the normal
		vec_normal = math::cross(vec_edge1, vec_edge2);
		normals_ptr[i] = math::normalize(vec_normal);

	} // Next Face

	// Now compute the actual VERTEX normals using face adjacency information
	for(i = 0; i < preparation_data_.triangle_count; ++i)
	{
		triangle& tri = preparation_data_.triangle_data[i];
		if(tri.flags & triangle_flags::degenerate)
			continue;

		// Process each vertex in the face
		for(j = 0; j < 3; ++j)
		{
			// Retrieve the index for this vertex.
			index = tri.indices[j];

			// Skip this vertex if normal information was already provided.
			if(!force_normal_generation_ &&
			   (preparation_data_.vertex_flags[index] & preparation_data::source_contains_normal))
				continue;

			// To generate vertex normals using the adjacency information we first
			// need to walk backwards
			// through the list to find the first triangle that references this vertex
			// (using entrance/exit
			// edge strategy).
			// Once we have the first triangle, step forwards and sum the normals of
			// each of the faces
			// for each triangle we touch. This is essentially a flood fill through
			// all of the triangles
			// that touch this vertex, without ever having to test the entire set for
			// shared vertices.
			// The initial backwards traversal prevents us from having to store (and
			// test) a 'visited' flag
			// for
			// every triangle in the buffer.

			// First walk backwards...
			start_tri = i;
			previous_tri = i;
			current_tri = adjacency_ptr[(i * 3) + ((j + 2) % 3)];
			for(;;)
			{
				// Stop walking if we reach the starting triangle again, or if there
				// is no connectivity out of this edge
				if(current_tri == start_tri || current_tri == 0xFFFFFFFF)
					break;

				// Find the edge in the adjacency list that we came in through
				for(k = 0; k < 3; ++k)
				{
					if(adjacency_ptr[(current_tri * 3) + k] == previous_tri)
						break;

				} // Next item in adjacency list

				// If we found the edge we entered through, the exit edge will
				// be the edge counter-clockwise from this one when walking backwards
				if(k < 3)
				{
					previous_tri = current_tri;
					current_tri = adjacency_ptr[(current_tri * 3) + ((k + 2) % 3)];

				} // End if found entrance edge
				else
				{
					break;

				} // End if failed to find entrance edge

			} // Next Test

			// We should now be at the starting triangle, we can start to walk
			// forwards
			// collecting the face normals. First find the exit edge so we can start
			// walking.
			if(current_tri != 0xFFFFFFFF)
			{
				for(k = 0; k < 3; ++k)
				{
					if(adjacency_ptr[(current_tri * 3) + k] == previous_tri)
						break;

				} // Next item in adjacency list
			}
			else
			{
				// Couldn't step back, so first triangle is the current triangle
				current_tri = i;
				k = j;
			}

			if(k < 3)
			{
				start_tri = current_tri;
				previous_tri = current_tri;
				current_tri = adjacency_ptr[(current_tri * 3) + k];
				vec_normal = normals_ptr[start_tri];
				for(;;)
				{
					// Stop walking if we reach the starting triangle again, or if there
					// is no connectivity out of this edge
					if(current_tri == start_tri || current_tri == 0xFFFFFFFF)
						break;

					// Add this normal.
					vec_normal += normals_ptr[current_tri];

					// Find the edge in the adjacency list that we came in through
					for(k = 0; k < 3; ++k)
					{
						if(adjacency_ptr[(current_tri * 3) + k] == previous_tri)
							break;

					} // Next item in adjacency list

					// If we found the edge we came entered through, the exit edge will
					// be the edge clockwise from this one when walking forwards
					if(k < 3)
					{
						previous_tri = current_tri;
						current_tri = adjacency_ptr[(current_tri * 3) + ((k + 1) % 3)];

					} // End if found entrance edge
					else
					{
						break;

					} // End if failed to find entrance edge

				} // Next Test

			} // End if found entrance edge

			// Normalize the new vertex normal
			vec_normal = math::normalize(vec_normal);

			// If the normal we are about to store is significantly different from any
			// normal
			// already stored in this vertex (excepting the case where it is <0,0,0>),
			// we need
			// to split the vertex into two.
			float fn[4];
			gfx::vertex_unpack(fn, gfx::attribute::Normal, vertex_format_, src_vertices_ptr, index);
			math::vec3 ref_normal;
			ref_normal[0] = fn[0];
			ref_normal[1] = fn[1];
			ref_normal[2] = fn[2];
			if(ref_normal.x == 0.0f && ref_normal.y == 0.0f && ref_normal.z == 0.0f)
			{
				gfx::vertex_pack(fn, true, gfx::attribute::Normal, vertex_format_, src_vertices_ptr, index);
			} // End if no normal stored here yet
			else
			{
				// Split and store in a new vertex if it is different (enough)
				if(math::abs(ref_normal.x - vec_normal.x) >= 1e-3f ||
				   math::abs(ref_normal.y - vec_normal.y) >= 1e-3f ||
				   math::abs(ref_normal.z - vec_normal.z) >= 1e-3f)
				{
					// Make room for new vertex data.
					preparation_data_.vertex_data.resize(preparation_data_.vertex_data.size() +
														 vertex_stride);

					// Ensure that we update the 'src_vertices_ptr' pointer (used
					// throughout the
					// loop). The internal buffer wrapped by the resized vertex data
					// vector
					// may have been re-allocated.
					src_vertices_ptr = &preparation_data_.vertex_data[0];

					// Duplicate the vertex at the end of the buffer
					memcpy(src_vertices_ptr + (preparation_data_.vertex_count * vertex_stride),
						   src_vertices_ptr + (index * vertex_stride), vertex_stride);

					// Duplicate any other remaining information.
					preparation_data_.vertex_flags.push_back(preparation_data_.vertex_flags[index]);

					// Record the split
					if(remap_array_ptr)
						(*remap_array_ptr)[index] = preparation_data_.vertex_count;

					// Store the new normal and finally record the fact that we have
					// added a new vertex.
					index = preparation_data_.vertex_count++;
					math::vec4 norm(vec_normal, 0.0f);
					gfx::vertex_pack(math::value_ptr(norm), true, gfx::attribute::Normal, vertex_format_,
									 src_vertices_ptr, index);

					// Update the index
					tri.indices[j] = index;

				} // End if normal is different

			} // End if normal already stored here

		} // Next Vertex

	} // Next Face

	// We're done with the surface normals
	checked_array_delete(normals_ptr);

	// If no new vertices were introduced, then it is not necessary
	// for the caller to remap anything.
	if(remap_array_ptr && original_vertex_count == preparation_data_.vertex_count)
		remap_array_ptr->clear();

	// Success!
	return true;
}

bool mesh::generate_vertex_barycentrics(std::uint32_t* adjacency)
{
	(void)adjacency;
	return true;
}

bool mesh::generate_vertex_tangents()
{
	math::vec3 *tangents = nullptr, *bitangents = nullptr;
	std::uint32_t i, i1, i2, i3, num_faces, num_verts;
	math::vec3 P, Q, T, B, cross_vec, normal_vec;

	// Get access to useful data offset information.
	std::uint16_t vertex_stride = vertex_format_.getStride();

	bool has_normals = vertex_format_.has(gfx::attribute::Normal);
	// This will fail if we don't already have normals however.
	if(!has_normals)
		return false;

	// Final format requests tangents?
	bool requires_tangents = vertex_format_.has(gfx::attribute::Tangent);
	bool requires_bitangents = vertex_format_.has(gfx::attribute::Bitangent);
	if(!force_tangent_generation_ && !requires_bitangents && !requires_tangents)
		return true;

	// Allocate storage space for the tangent and bitangent vectors
	// that we will effectively need to average for shared vertices.
	num_faces = preparation_data_.triangle_count;
	num_verts = preparation_data_.vertex_count;
	tangents = new math::vec3[num_verts];
	bitangents = new math::vec3[num_verts];
	memset(tangents, 0, sizeof(math::vec3) * num_verts);
	memset(bitangents, 0, sizeof(math::vec3) * num_verts);

	// Iterate through each triangle in the mesh
	std::uint8_t* src_vertices_ptr = &preparation_data_.vertex_data[0];
	for(i = 0; i < num_faces; ++i)
	{
		triangle& tri = preparation_data_.triangle_data[i];

		// Compute the three indices for the triangle
		i1 = tri.indices[0];
		i2 = tri.indices[1];
		i3 = tri.indices[2];

		// Retrieve references to the positions of the three vertices in the
		// triangle.
		math::vec3 E;
		float fE[4];
		gfx::vertex_unpack(fE, gfx::attribute::Position, vertex_format_, src_vertices_ptr, i1);
		math::vec3 F;
		float fF[4];
		gfx::vertex_unpack(fF, gfx::attribute::Position, vertex_format_, src_vertices_ptr, i2);
		math::vec3 G;
		float fG[4];
		gfx::vertex_unpack(fG, gfx::attribute::Position, vertex_format_, src_vertices_ptr, i3);
		memcpy(&E[0], fE, 3 * sizeof(float));
		memcpy(&F[0], fF, 3 * sizeof(float));
		memcpy(&G[0], fG, 3 * sizeof(float));

		// Retrieve references to the base texture coordinates of the three vertices
		// in the triangle.
		// TODO: Allow customization of which tex coordinates to generate from.
		math::vec2 Et;
		float fEt[4];
		gfx::vertex_unpack(&fEt[0], gfx::attribute::TexCoord0, vertex_format_, src_vertices_ptr, i1);
		math::vec2 Ft;
		float fFt[4];
		gfx::vertex_unpack(&fFt[0], gfx::attribute::TexCoord0, vertex_format_, src_vertices_ptr, i2);
		math::vec2 Gt;
		float fGt[4];
		gfx::vertex_unpack(&fGt[0], gfx::attribute::TexCoord0, vertex_format_, src_vertices_ptr, i3);
		memcpy(&Et[0], fEt, 2 * sizeof(float));
		memcpy(&Ft[0], fFt, 2 * sizeof(float));
		memcpy(&Gt[0], fGt, 2 * sizeof(float));

		// Compute the known variables P & Q, where "P = F-E" and "Q = G-E"
		// based on our original discussion of the tangent vector
		// calculation.
		P = F - E;
		Q = G - E;

		// Also compute the know variables <s1,t1> and <s2,t2>. Recall that
		// these are the texture coordinate deltas similarly for "F-E"
		// and "G-E".
		float s1 = Ft.x - Et.x;
		float t1 = Ft.y - Et.y;
		float s2 = Gt.x - Et.x;
		float t2 = Gt.y - Et.y;

		// Next we can pre-compute part of the equation we developed
		// earlier: "1/(s1 * t2 - s2 * t1)". We do this in two separate
		// stages here in order to ensure that the texture coordinates
		// are not invalid.
		float r = (s1 * t2 - s2 * t1);
		if(math::abs(r) < math::epsilon<float>())
			continue;
		r = 1.0f / r;

		// All that's left for us to do now is to run the matrix
		// multiplication and multiply the result by the scalar portion
		// we precomputed earlier.
		T.x = r * (t2 * P.x - t1 * Q.x);
		T.y = r * (t2 * P.y - t1 * Q.y);
		T.z = r * (t2 * P.z - t1 * Q.z);
		B.x = r * (s1 * Q.x - s2 * P.x);
		B.y = r * (s1 * Q.y - s2 * P.y);
		B.z = r * (s1 * Q.z - s2 * P.z);

		// Add the tangent and bitangent vectors (summed average) to
		// any previous values computed for each vertex.
		tangents[i1] += T;
		tangents[i2] += T;
		tangents[i3] += T;
		bitangents[i1] += B;
		bitangents[i2] += B;
		bitangents[i3] += B;

	} // Next triangle

	// Generate final tangent vectors
	for(i = 0; i < num_verts; i++, src_vertices_ptr += vertex_stride)
	{
		// Skip if the original imported data already provided a bitangent /
		// tangent.
		bool has_bitangent =
			((preparation_data_.vertex_flags[i] & preparation_data::source_contains_binormal) != 0);
		bool has_tangent =
			((preparation_data_.vertex_flags[i] & preparation_data::source_contains_tangent) != 0);
		if(!force_tangent_generation_ && has_bitangent && has_tangent)
			continue;

		// Retrieve the normal vector from the vertex and the computed
		// tangent vector.
		float normal[4];
		gfx::vertex_unpack(normal, gfx::attribute::Normal, vertex_format_, src_vertices_ptr);
		memcpy(&normal_vec[0], normal, 3 * sizeof(float));

		T = tangents[i];

		// GramSchmidt orthogonalize
		T = T - (normal_vec * math::dot(normal_vec, T));
		T = math::normalize(T);

		// Store tangent if required
		if(force_tangent_generation_ || (!has_tangent && requires_tangents))
			gfx::vertex_pack(&math::vec4(T, 1.0f)[0], true, gfx::attribute::Tangent, vertex_format_,
							 src_vertices_ptr);

		// Compute and store bitangent if required
		if(force_tangent_generation_ || (!has_bitangent && requires_bitangents))
		{
			// Calculate the new orthogonal bitangent
			B = math::cross(normal_vec, T);
			B = math::normalize(B);

			// Compute the "handedness" of the tangent and bitangent. This
			// ensures the inverted / mirrored texture coordinates still have
			// an accurate matrix.
			cross_vec = math::cross(normal_vec, T);
			if(math::dot(cross_vec, bitangents[i]) < 0.0f)
			{
				// Flip the bitangent
				B = -B;

			} // End if coordinates inverted

			// Store.
			gfx::vertex_pack(&math::vec4(B, 1.0f)[0], true, gfx::attribute::Bitangent, vertex_format_,
							 src_vertices_ptr);

		} // End if requires bitangent

	} // Next vertex

	// Cleanup
	checked_array_delete(tangents);
	checked_array_delete(bitangents);

	// Return success
	return true;
}

bool mesh::generate_adjacency(std::vector<std::uint32_t>& adjacency)
{
	std::map<adjacent_edge_key, std::uint32_t> edge_tree;
	std::map<adjacent_edge_key, std::uint32_t>::iterator it_edge;

	// What is the status of the mesh?
	if(prepare_status_ != mesh_status::prepared)
	{
		// Validate requirements
		if(preparation_data_.triangle_count == 0)
			return false;

		// Retrieve useful data offset information.
		std::uint16_t position_offset = vertex_format_.getOffset(gfx::attribute::Position);
		std::uint16_t vertex_stride = vertex_format_.getStride();

		// Insert all edges into the edge tree
		std::uint8_t* src_vertices_ptr = &preparation_data_.vertex_data[0] + position_offset;
		for(std::uint32_t i = 0; i < preparation_data_.triangle_count; ++i)
		{
			adjacent_edge_key edge;

			// Degenerate triangles cannot participate.
			const triangle& tri = preparation_data_.triangle_data[i];
			if(tri.flags & triangle_flags::degenerate)
				continue;

			// Retrieve positions of each referenced vertex.
			const math::vec3* v1 =
				reinterpret_cast<const math::vec3*>(src_vertices_ptr + (tri.indices[0] * vertex_stride));
			const math::vec3* v2 =
				reinterpret_cast<const math::vec3*>(src_vertices_ptr + (tri.indices[1] * vertex_stride));
			const math::vec3* v3 =
				reinterpret_cast<const math::vec3*>(src_vertices_ptr + (tri.indices[2] * vertex_stride));

			// edge 1
			edge.vertex1 = v1;
			edge.vertex2 = v2;
			edge_tree[edge] = i;

			// edge 2
			edge.vertex1 = v2;
			edge.vertex2 = v3;
			edge_tree[edge] = i;

			// edge 3
			edge.vertex1 = v3;
			edge.vertex2 = v1;
			edge_tree[edge] = i;

		} // Next Face

		// Size the output array.
		adjacency.resize(preparation_data_.triangle_count * 3, 0xFFFFFFFF);

		// Now, find any adjacent edges for each triangle edge
		for(std::uint32_t i = 0; i < preparation_data_.triangle_count; ++i)
		{
			adjacent_edge_key edge;

			// Degenerate triangles cannot participate.
			const triangle& tri = preparation_data_.triangle_data[i];
			if(tri.flags & triangle_flags::degenerate)
				continue;

			// Retrieve positions of each referenced vertex.
			const math::vec3* v1 =
				reinterpret_cast<const math::vec3*>(src_vertices_ptr + (tri.indices[0] * vertex_stride));
			const math::vec3* v2 =
				reinterpret_cast<const math::vec3*>(src_vertices_ptr + (tri.indices[1] * vertex_stride));
			const math::vec3* v3 =
				reinterpret_cast<const math::vec3*>(src_vertices_ptr + (tri.indices[2] * vertex_stride));

			// Note: Notice below that the order of the edge vertices
			//       is swapped. This is because we want to find the
			//       matching ADJACENT edge, rather than simply finding
			//       the same edge that we're currently processing.

			// edge 1
			edge.vertex2 = v1;
			edge.vertex1 = v2;

			// Find the matching adjacent edge
			it_edge = edge_tree.find(edge);
			if(it_edge != edge_tree.end())
				adjacency[(i * 3)] = it_edge->second;

			// edge 2
			edge.vertex2 = v2;
			edge.vertex1 = v3;

			// Find the matching adjacent edge
			it_edge = edge_tree.find(edge);
			if(it_edge != edge_tree.end())
				adjacency[(i * 3) + 1] = it_edge->second;

			// edge 3
			edge.vertex2 = v3;
			edge.vertex1 = v1;

			// Find the matching adjacent edge
			it_edge = edge_tree.find(edge);
			if(it_edge != edge_tree.end())
				adjacency[(i * 3) + 2] = it_edge->second;

		} // Next Face

	} // End if not prepared
	else
	{
		// Validate requirements
		if(face_count_ == 0)
			return false;

		// Retrieve useful data offset information.
		std::uint16_t position_offset = vertex_format_.getOffset(gfx::attribute::Position);
		std::uint16_t vertex_stride = vertex_format_.getStride();

		// Insert all edges into the edge tree
		std::uint8_t* src_vertices_ptr = system_vb_ + position_offset;
		std::uint32_t* src_indices_ptr = system_ib_;
		for(std::uint32_t i = 0; i < face_count_; ++i, src_indices_ptr += 3)
		{
			adjacent_edge_key edge;

			// Retrieve positions of each referenced vertex.
			const auto* v1 =
				reinterpret_cast<const math::vec3*>(src_vertices_ptr + (src_indices_ptr[0] * vertex_stride));
			const auto* v2 =
				reinterpret_cast<const math::vec3*>(src_vertices_ptr + (src_indices_ptr[1] * vertex_stride));
			const auto* v3 =
				reinterpret_cast<const math::vec3*>(src_vertices_ptr + (src_indices_ptr[2] * vertex_stride));

			// edge 1
			edge.vertex1 = v1;
			edge.vertex2 = v2;
			edge_tree[edge] = i;

			// edge 2
			edge.vertex1 = v2;
			edge.vertex2 = v3;
			edge_tree[edge] = i;

			// edge 3
			edge.vertex1 = v3;
			edge.vertex2 = v1;
			edge_tree[edge] = i;

		} // Next Face

		// Size the output array.
		adjacency.resize(face_count_ * 3, 0xFFFFFFFF);

		// Now, find any adjacent edges for each triangle edge
		src_indices_ptr = system_ib_;
		for(std::uint32_t i = 0; i < face_count_; ++i, src_indices_ptr += 3)
		{
			adjacent_edge_key edge;

			// Retrieve positions of each referenced vertex.
			const math::vec3* v1 =
				reinterpret_cast<const math::vec3*>(src_vertices_ptr + (src_indices_ptr[0] * vertex_stride));
			const math::vec3* v2 =
				reinterpret_cast<const math::vec3*>(src_vertices_ptr + (src_indices_ptr[1] * vertex_stride));
			const math::vec3* v3 =
				reinterpret_cast<const math::vec3*>(src_vertices_ptr + (src_indices_ptr[2] * vertex_stride));

			// Note: Notice below that the order of the edge vertices
			//       is swapped. This is because we want to find the
			//       matching ADJACENT edge, rather than simply finding
			//       the same edge that we're currently processing.

			// edge 1
			edge.vertex2 = v1;
			edge.vertex1 = v2;

			// Find the matching adjacent edge
			it_edge = edge_tree.find(edge);
			if(it_edge != edge_tree.end())
				adjacency[(i * 3)] = it_edge->second;

			// edge 2
			edge.vertex2 = v2;
			edge.vertex1 = v3;

			// Find the matching adjacent edge
			it_edge = edge_tree.find(edge);
			if(it_edge != edge_tree.end())
				adjacency[(i * 3) + 1] = it_edge->second;

			// edge 3
			edge.vertex2 = v3;
			edge.vertex1 = v1;

			// Find the matching adjacent edge
			it_edge = edge_tree.find(edge);
			if(it_edge != edge_tree.end())
				adjacency[(i * 3) + 2] = it_edge->second;

		} // Next Face

	} // End if prepared

	// Success!
	return true;
}

bool mesh::weld_vertices(float tolerance, std::vector<std::uint32_t>* vertex_remap_ptr /* = nullptr */)
{
	weld_key key;
	std::map<weld_key, std::uint32_t> vertex_tree;
	std::map<weld_key, std::uint32_t>::const_iterator it_key;
	byte_array_t new_vertex_data, new_vertex_flags;
	std::uint32_t new_vertex_count = 0;

	// Allocate enough space to build the remap array for the existing vertices
	if(vertex_remap_ptr)
		vertex_remap_ptr->resize(preparation_data_.vertex_count);
	auto collapse_map = new std::uint32_t[preparation_data_.vertex_count];

	// Retrieve useful data offset information.
	std::uint16_t vertex_stride = vertex_format_.getStride();

	// For each vertex to be welded.
	for(std::uint32_t i = 0; i < preparation_data_.vertex_count; ++i)
	{
		// Build a new key structure for inserting
		key.vertex = (&preparation_data_.vertex_data[0]) + (i * vertex_stride);
		key.format = vertex_format_;
		key.tolerance = tolerance;

		// Does a vertex with matching details already exist in the tree.
		it_key = vertex_tree.find(key);
		if(it_key == vertex_tree.end())
		{
			// No matching vertex. Insert into the tree (value = NEW index of vertex).
			vertex_tree[key] = new_vertex_count;
			collapse_map[i] = new_vertex_count;
			if(vertex_remap_ptr)
				(*vertex_remap_ptr)[i] = new_vertex_count;

			// Store the vertex in the new buffer
			new_vertex_data.resize((new_vertex_count + 1) * vertex_stride);
			memcpy(&new_vertex_data[new_vertex_count * vertex_stride], key.vertex, vertex_stride);
			new_vertex_flags.push_back(preparation_data_.vertex_flags[i]);
			new_vertex_count++;

		} // End if no matching vertex
		else
		{
			// A vertex already existed at this location.
			// Just mark the 'collapsed' index for this vertex in the remap array.
			collapse_map[i] = it_key->second;
			if(vertex_remap_ptr)
				(*vertex_remap_ptr)[i] = 0xFFFFFFFF;

		} // End if vertex already existed

	} // Next Vertex

	// If nothing was welded, just bail
	if(preparation_data_.vertex_count == new_vertex_count)
	{
		checked_array_delete(collapse_map);

		if(vertex_remap_ptr)
			vertex_remap_ptr->clear();
		return true;

	} // End if nothing to do

	// Otherwise, replace the old preparation vertices and remap
	preparation_data_.vertex_data.clear();
	preparation_data_.vertex_data.resize(new_vertex_data.size());
	memcpy(&preparation_data_.vertex_data[0], &new_vertex_data[0], new_vertex_data.size());
	preparation_data_.vertex_flags.clear();
	preparation_data_.vertex_flags.resize(new_vertex_flags.size());
	memcpy(&preparation_data_.vertex_flags[0], &new_vertex_flags[0], new_vertex_flags.size());
	preparation_data_.vertex_count = new_vertex_count;

	// Now remap all the triangle indices
	for(std::uint32_t i = 0; i < preparation_data_.triangle_count; ++i)
	{
		triangle& tri = preparation_data_.triangle_data[i];
		tri.indices[0] = collapse_map[tri.indices[0]];
		tri.indices[1] = collapse_map[tri.indices[1]];
		tri.indices[2] = collapse_map[tri.indices[2]];

	} // Next triangle

	// Clean up
	checked_array_delete(collapse_map);

	// Success!
	return true;
}

std::uint32_t mesh::get_face_count() const
{
	if(prepare_status_ == mesh_status::prepared)
		return face_count_;
	else if(prepare_status_ == mesh_status::preparing)
		return static_cast<std::uint32_t>(preparation_data_.triangle_data.size());
	else
		return 0;
}

std::uint32_t mesh::get_vertex_count() const
{
	if(prepare_status_ == mesh_status::prepared)
		return vertex_count_;
	else if(prepare_status_ == mesh_status::preparing)
		return preparation_data_.vertex_count;
	else
		return 0;
}

std::uint8_t* mesh::get_system_vb()
{
	return system_vb_;
}

std::uint32_t* mesh::get_system_ib()
{
	return system_ib_;
}

const gfx::vertex_layout& mesh::get_vertex_format() const
{
	return vertex_format_;
}

const mesh::subset* mesh::get_subset(std::uint32_t data_group_id /* = 0 */) const
{
	auto it = subset_lookup_.find(mesh_subset_key(data_group_id));
	if(it == subset_lookup_.end())
		return nullptr;
	return it->second;
}

const skin_bind_data& mesh::get_skin_bind_data() const
{
	return skin_bind_data_;
}

const mesh::bone_palette_array_t& mesh::get_bone_palettes() const
{
	return bone_palettes_;
}

const std::unique_ptr<mesh::armature_node>& mesh::get_armature() const
{
	return root_;
}

irect32_t mesh::calculate_screen_rect(const math::transform& world, const camera& cam) const
{

	auto bounds = math::bbox::mul(get_bounds(), world);
	math::vec3 cen = bounds.get_center();
	math::vec3 ext = bounds.get_extents();
	std::array<math::vec2, 8> extentPoints = {{
		cam.world_to_viewport(math::vec3(cen.x - ext.x, cen.y - ext.y, cen.z - ext.z)),
		cam.world_to_viewport(math::vec3(cen.x + ext.x, cen.y - ext.y, cen.z - ext.z)),
		cam.world_to_viewport(math::vec3(cen.x - ext.x, cen.y - ext.y, cen.z + ext.z)),
		cam.world_to_viewport(math::vec3(cen.x + ext.x, cen.y - ext.y, cen.z + ext.z)),
		cam.world_to_viewport(math::vec3(cen.x - ext.x, cen.y + ext.y, cen.z - ext.z)),
		cam.world_to_viewport(math::vec3(cen.x + ext.x, cen.y + ext.y, cen.z - ext.z)),
		cam.world_to_viewport(math::vec3(cen.x - ext.x, cen.y + ext.y, cen.z + ext.z)),
		cam.world_to_viewport(math::vec3(cen.x + ext.x, cen.y + ext.y, cen.z + ext.z)),
	}};

	math::vec2 min = extentPoints[0];
	math::vec2 max = extentPoints[0];
	for(const auto& v : extentPoints)
	{
		min = math::min(min, v);
		max = math::max(max, v);
	}
	return irect32_t(irect32_t::value_type(min.x), irect32_t::value_type(min.y), irect32_t::value_type(max.x),
					 irect32_t::value_type(max.y));
}

///////////////////////////////////////////////////////////////////////////////
// skin_bind_data Member Definitions
///////////////////////////////////////////////////////////////////////////////
void skin_bind_data::add_bone(const bone_influence& bone)
{
	bones_.push_back(bone);
}

void skin_bind_data::remove_empty_bones()
{
	for(size_t i = 0; i < bones_.size();)
	{
		if(bones_[i].influences.empty())
		{
			bones_.erase(bones_.begin() + static_cast<int>(i));

		} // End if empty
		else
		{
			++i;
		}

	} // Next Bone
}

void skin_bind_data::clear_vertex_influences()
{
	for(size_t i = 0; i < bones_.size(); ++i)
	{
		bones_[i].influences.clear();
	}
}

void skin_bind_data::clear()
{
	bones_.clear();
}

void skin_bind_data::remap_vertices(const std::vector<std::uint32_t>& remap)
{
	// Iterate through all bone information and remap vertex indices.
	for(size_t i = 0; i < bones_.size(); ++i)
	{
		vertex_influence_array_t new_influences;
		vertex_influence_array_t& influences = bones_[i].influences;
		new_influences.reserve(influences.size());
		for(size_t j = 0; j < influences.size(); ++j)
		{
			std::uint32_t new_index = remap[influences[j].vertex_index];
			if(new_index != 0xFFFFFFFF)
			{
				// Insert an influence at the new index
				new_influences.push_back(vertex_influence(new_index, influences[j].weight));

				// If the vertex was split into two, we want to retain an
				// influence to the original index too.
				if(new_index >= remap.size())
				{
					new_influences.push_back(
						vertex_influence(influences[j].vertex_index, influences[j].weight));
				}

			} // End if !removed

		} // Next source influence
		bones_[i].influences = new_influences;

	} // Next bone
}

void skin_bind_data::build_vertex_table(std::uint32_t vertex_count,
										const std::vector<std::uint32_t>& vertex_remap,
										vertex_data_array_t& table)
{
	std::uint32_t vertex;

	// Initialize the vertex table with the required number of vertices.
	table.reserve(vertex_count);
	for(vertex = 0; vertex < vertex_count; ++vertex)
	{
		vertex_data data;
		data.palette = -1;
		data.original_vertex = vertex;
		table.push_back(data);

	} // Next Vertex

	// Iterate through all bone information and populate the above array.
	for(size_t i = 0; i < bones_.size(); ++i)
	{
		vertex_influence_array_t& influences = bones_[i].influences;
		for(size_t j = 0; j < influences.size(); ++j)
		{

			// Vertex data has been remapped?
			if(vertex_remap.size() > 0)
			{
				vertex = vertex_remap[influences[j].vertex_index];
				if(vertex == 0xFFFFFFFF)
					continue;
				auto& data = table[vertex];
				// Push influence data.
				data.influences.push_back(static_cast<std::int32_t>(i));
				data.weights.push_back(influences[j].weight);
			} // End if remap
			else
			{
				auto& data = table[influences[j].vertex_index];
				// Push influence data.
				data.influences.push_back(static_cast<std::int32_t>(i));
				data.weights.push_back(influences[j].weight);
			}

		} // Next Influence

	} // Next Bone
}

const std::vector<skin_bind_data::bone_influence>& skin_bind_data::get_bones() const
{
	return bones_;
}

std::vector<skin_bind_data::bone_influence>& skin_bind_data::get_bones()
{
	return bones_;
}

bool skin_bind_data::has_bones() const
{
	return !get_bones().empty();
}

const skin_bind_data::bone_influence* skin_bind_data::find_bone_by_id(const std::string& name) const
{
	auto it = std::find_if(std::begin(bones_), std::end(bones_),
						   [name](const auto& bone) { return name == bone.bone_id; });
	if(it != std::end(bones_))
	{
		return &(*it);
	}

	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
// bone_palette Member Definitions
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
//  Name : bone_palette() (Constructor)
/// <summary>
/// Class constructor.
/// </summary>
//-----------------------------------------------------------------------------
bone_palette::bone_palette(std::uint32_t palette_size)
	: data_group_id_(0)
	, maximum_size_(palette_size)
	, maximum_blend_index_(-1)
{
}

//-----------------------------------------------------------------------------
//  Name : bone_palette() (Copy Constructor)
/// <summary>
/// Class copy constructor.
/// </summary>
//-----------------------------------------------------------------------------
bone_palette::bone_palette(const bone_palette& rhs)
	: bones_lut_(rhs.bones_lut_)
	, bones_(rhs.bones_)
	, faces_(rhs.faces_)
	, data_group_id_(rhs.data_group_id_)
	, maximum_size_(rhs.maximum_size_)
	, maximum_blend_index_(rhs.maximum_blend_index_)
{
}

//-----------------------------------------------------------------------------
//  Name : ~bone_palette() (Destructor)
/// <summary>
/// Clean up any resources being used.
/// </summary>
//-----------------------------------------------------------------------------
bone_palette::~bone_palette()
{
}

std::vector<math::transform>
bone_palette::get_skinning_matrices(const std::vector<math::transform>& node_transforms,
									const skin_bind_data& bind_data, bool compute_inverse_transpose) const
{
	// Retrieve the main list of bones from the skin bind data that will
	// be referenced by the palette's bone index list.
	const auto& bind_list = bind_data.get_bones();
	if(node_transforms.empty())
		return node_transforms;

	const std::uint32_t max_blend_transforms = gfx::get_max_blend_transforms();
	std::vector<math::transform> transforms;
	transforms.resize(max_blend_transforms);

	// Compute transformation matrix for each bone in the palette
	for(size_t i = 0; i < bones_.size(); ++i)
	{
		auto bone = bones_[i];
		const auto& bone_transform = node_transforms[bone];
		const auto& bone_data = bind_list[bone];
		auto& transform = transforms[i];
		transform = bone_transform * bone_data.bind_pose_transform;
		if(compute_inverse_transpose)
		{
			transform = math::transpose(math::inverse(transform));
		}

	} // Next Bone

	return transforms;
}

void bone_palette::assign_bones(bone_index_map_t& bones, std::vector<std::uint32_t>& faces)
{
	bone_index_map_t::iterator it_bone, it_bone2;

	// Iterate through newly specified input bones and add any unique ones to the
	// palette.
	for(it_bone = bones.begin(); it_bone != bones.end(); ++it_bone)
	{
		it_bone2 = bones_lut_.find(it_bone->first);
		if(it_bone2 == bones_lut_.end())
		{
			bones_lut_[it_bone->first] = static_cast<std::uint32_t>(bones_.size());
			bones_.push_back(it_bone->first);

		} // End if not already added

	} // Next Bone

	// Merge the new face list with ours.
	faces_.insert(faces_.end(), faces.begin(), faces.end());
}

void bone_palette::assign_bones(const std::vector<std::uint32_t>& bones)
{
	bone_index_map_t::iterator it_bone;

	// Clear out prior data.
	bones_.clear();
	bones_lut_.clear();

	// Iterate through newly specified input bones and add any unique ones to the
	// palette.
	for(size_t i = 0; i < bones.size(); ++i)
	{
		it_bone = bones_lut_.find(bones[i]);
		if(it_bone == bones_lut_.end())
		{
			bones_lut_[bones[i]] = static_cast<std::uint32_t>(bones_.size());
			bones_.push_back(bones[i]);

		} // End if not already added

	} // Next Bone
}

void bone_palette::compute_palette_fit(bone_index_map_t& input, std::int32_t& current_space,
									   std::int32_t& common_bones, std::int32_t& additional_bones)
{
	// Reset values
	current_space = static_cast<std::int32_t>(maximum_size_ - static_cast<std::uint32_t>(bones_.size()));
	common_bones = 0;
	additional_bones = 0;

	// Early out if possible
	if(bones_.size() == 0)
	{
		additional_bones = static_cast<std::int32_t>(input.size());
		return;

	} // End if no bones stored
	else if(input.size() == 0)
	{
		return;

	} // End if no bones input

	// Iterate through newly specified input bones and see how many
	// indices it has in common with our existing set.
	bone_index_map_t::iterator it_bone, it_bone2;
	for(it_bone = input.begin(); it_bone != input.end(); ++it_bone)
	{
		it_bone2 = bones_lut_.find(it_bone->first);
		if(it_bone2 != bones_lut_.end())
			common_bones++;
		else
			additional_bones++;

	} // Next Bone
}

std::uint32_t bone_palette::translate_bone_to_palette(std::uint32_t bone_index) const
{
	auto it_bone = bones_lut_.find(bone_index);
	if(it_bone == bones_lut_.end())
		return 0xFFFFFFFF;
	return it_bone->second;
}

std::uint32_t bone_palette::get_data_group() const
{
	return data_group_id_;
}

void bone_palette::set_data_group(std::uint32_t group)
{
	data_group_id_ = group;
}

std::int32_t bone_palette::get_maximum_blend_index() const
{
	return maximum_blend_index_;
}

void bone_palette::set_maximum_blend_index(int nIndex)
{
	maximum_blend_index_ = nIndex;
}

std::uint32_t bone_palette::get_maximum_size() const
{
	return maximum_size_;
}

std::vector<std::uint32_t>& bone_palette::get_influenced_faces()
{
	return faces_;
}

void bone_palette::clear_influenced_faces()
{
	faces_.clear();
}

const std::vector<std::uint32_t>& bone_palette::get_bones() const
{
	return bones_;
}

///////////////////////////////////////////////////////////////////////////////
// Global Operator Definitions
///////////////////////////////////////////////////////////////////////////////
bool operator<(const mesh::adjacent_edge_key& key1, const mesh::adjacent_edge_key& key2)
{
	// Test vertex positions.
	if(math::epsilonNotEqual(key1.vertex1->x, key2.vertex1->x, math::epsilon<float>()))
		return (key2.vertex1->x < key1.vertex1->x);
	if(math::epsilonNotEqual(key1.vertex1->y, key2.vertex1->y, math::epsilon<float>()))
		return (key2.vertex1->y < key1.vertex1->y);
	if(math::epsilonNotEqual(key1.vertex1->z, key2.vertex1->z, math::epsilon<float>()))
		return (key2.vertex1->z < key1.vertex1->z);

	if(math::epsilonNotEqual(key1.vertex2->x, key2.vertex2->x, math::epsilon<float>()))
		return (key2.vertex2->x < key1.vertex2->x);
	if(math::epsilonNotEqual(key1.vertex2->y, key2.vertex2->y, math::epsilon<float>()))
		return (key2.vertex2->y < key1.vertex2->y);
	if(math::epsilonNotEqual(key1.vertex2->z, key2.vertex2->z, math::epsilon<float>()))
		return (key2.vertex2->z < key1.vertex2->z);

	// Exactly equal
	return false;
}

bool operator<(const mesh::mesh_subset_key& key1, const mesh::mesh_subset_key& key2)
{
	return key1.data_group_id < key2.data_group_id;
}

bool operator<(const mesh::weld_key& key1, const mesh::weld_key& key2)
{
	float pos1[4];
	gfx::vertex_unpack(pos1, gfx::attribute::Position, key1.format, key1.vertex);
	math::vec3 v1(pos1[0], pos1[1], pos1[2]);

	float pos2[4] = {0};
	gfx::vertex_unpack(pos1, gfx::attribute::Position, key2.format, key2.vertex);
	math::vec3 v2(pos2[0], pos2[1], pos2[2]);
	float tolerance = key1.tolerance * key2.tolerance;

	return math::distance2(v1, v2) > tolerance;
}

bool operator<(const mesh::bone_combination_key& key1, const mesh::bone_combination_key& key2)
{
	// Data group id must match.
	if(key1.data_group_id != key2.data_group_id)
		return key1.data_group_id < key2.data_group_id;

	const mesh::face_influences* p1 = key1.influences;
	const mesh::face_influences* p2 = key2.influences;

	// The bone count must match.
	if(p1->bones.size() != p2->bones.size())
		return p1->bones.size() < p2->bones.size();

	// Compare the bone indices in each list
	auto it_bone1 = p1->bones.begin();
	auto it_bone2 = p2->bones.begin();
	for(; it_bone1 != p1->bones.end() && it_bone2 != p2->bones.end(); ++it_bone1, ++it_bone2)
	{
		if(it_bone1->first != it_bone2->first)
			return it_bone1->first < it_bone2->first;

	} // Next Bone

	// Exact match (for the purposes of combining influences)
	return false;
}
