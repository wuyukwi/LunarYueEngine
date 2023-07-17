#include "camera.h"

#include <core/graphics/graphics.h>

#include <limits>

float camera::get_zoom_factor() const
{
	if(viewport_size_.height == 0)
	{
		return 0.0f;
	}

	return ortho_size_ / (float(viewport_size_.height) / 2.0f);
}

float camera::get_ppu() const
{
	return float(viewport_size_.height) / (2.0f * ortho_size_);
}

void camera::set_viewport_size(const usize32_t& viewport_size)
{
	viewport_size_ = viewport_size;
	set_aspect_ratio(float(viewport_size.width) / float(viewport_size.height));
}

void camera::set_orthographic_size(float size)
{
	ortho_size_ = size;

	touch();
}

void camera::set_fov(float fFOVY)
{
	// Skip if no-op
	if(math::equal(fFOVY, fov_, math::epsilon<float>()))
	{
		return;
	}

	// Update projection matrix and view frustum
	fov_ = fFOVY;

	touch();
}

void camera::set_projection_mode(projection_mode mode)
{
	// Bail if this is a no op.
	if(mode == projection_mode_)
	{
		return;
	}

	// Alter the projection mode.
	projection_mode_ = mode;

	touch();
}

void camera::set_near_clip(float distance)
{
	// Skip if this is a no-op
	if(math::equal(distance, near_clip_, math::epsilon<float>()))
	{
		return;
	}

	// Store value
	near_clip_ = distance;

	touch();

	// Make sure near clip is less than the far clip
	if(near_clip_ > far_clip_)
	{
		set_far_clip(near_clip_);
	}
}

void camera::set_far_clip(float distance)
{
	// Skip if this is a no-op
	if(math::equal(distance, far_clip_, math::epsilon<float>()))
	{
		return;
	}

	// Store value
	far_clip_ = distance;

	touch();

	// Make sure near clip is less than the far clip
	if(near_clip_ > far_clip_)
	{
		set_near_clip(far_clip_);
	}
}

math::bbox camera::get_local_bounding_box()
{
	if(projection_mode_ == projection_mode::perspective)
	{
		float far_size = math::tan(math::radians<float>(fov_ * 0.5f)) * far_clip_;
		return math::bbox(-far_size * aspect_ratio_, -far_size, near_clip_, far_size * aspect_ratio_,
						  far_size, far_clip_);
	}

	float spread = far_clip_ - near_clip_;
	math::vec3 center = {0.0f, 0.0f, (far_clip_ + near_clip_) * 0.5f};
	float orthographicSize = get_ortho_size();
	math::vec3 size = {orthographicSize * 2.0f * aspect_ratio_, orthographicSize * 2.0f, spread};
	return math::bbox(center - (size / 2.0f), center + (size / 2.0f));
}

void camera::set_aspect_ratio(float aspect, bool bLocked /* = false */)
{
	// Is this a no-op?
	if(math::equal(aspect, aspect_ratio_, math::epsilon<float>()))
	{
		aspect_locked_ = bLocked;
		return;

	} // End if aspect is the same

	// Update camera properties
	aspect_ratio_ = aspect;
	aspect_locked_ = bLocked;
	aspect_dirty_ = true;
	frustum_dirty_ = true;
	projection_dirty_ = true;
}

bool camera::is_aspect_locked() const
{
	return (get_projection_mode() == projection_mode::orthographic || aspect_locked_);
}

const math::transform& camera::get_projection() const
{

	// Only update matrix if something has changed
	if(get_projection_mode() == projection_mode::perspective)
	{
		if(projection_dirty_)
		{
			// Generate the updated perspective projection matrix
			float fov_radians = math::radians<float>(get_fov());
			static const auto perspective_ =
				gfx::is_homogeneous_depth() ? math::perspectiveNO<float> : math::perspectiveZO<float>;

			math::mat4 projection = perspective_(fov_radians, aspect_ratio_, near_clip_, far_clip_);

			projection[2][0] += aa_data_.z;
			projection[2][1] += aa_data_.w;
			projection_ = projection;
			// Matrix has been updated
			projection_dirty_ = false;
			aspect_dirty_ = false;

		} // End if projection matrix needs updating
		else if(aspect_dirty_)
		{
			// Just alter the aspect ratio
			math::mat4 projection = projection_;
			projection[0][0] = projection[1][1] / aspect_ratio_;
			projection_ = projection;
			// Matrix has been updated
			aspect_dirty_ = false;

		} // End if only aspect ratio changed

	} // End if perspective
	else if(get_projection_mode() == projection_mode::orthographic)
	{
		if(projection_dirty_ || aspect_dirty_)
		{
			// Generate the updated orthographic projection matrix
			float zoom = get_zoom_factor();
			const frect_t rect = {-float(viewport_size_.width) / 2.0f, float(viewport_size_.height) / 2.0f,
								  float(viewport_size_.width) / 2.0f, -float(viewport_size_.height) / 2.0f};
			static const auto ortho_ =
				gfx::is_homogeneous_depth() ? math::orthoNO<float> : math::orthoZO<float>;

			math::mat4 projection = ortho_(rect.left * zoom, rect.right * zoom, rect.bottom * zoom,
										   rect.top * zoom, get_near_clip(), get_far_clip());
			projection[2][0] += aa_data_.z;
			projection[2][1] += aa_data_.w;
			projection_ = projection;
			// Matrix has been updated
			projection_dirty_ = false;
			aspect_dirty_ = false;

		} // End if projection matrix needs updating

	} // End if orthographic

	// Return the projection matrix.
	return projection_;
}

void camera::look_at(const math::vec3& vEye, const math::vec3& vAt)
{
	look_at(vEye, vAt, math::vec3(0.0f, 1.0f, 0.0f));
}

void camera::look_at(const math::vec3& vEye, const math::vec3& vAt, const math::vec3& vUp)
{
	view_ = math::lookAt(vEye, vAt, vUp);

	touch();
}

math::vec3 camera::get_position() const
{
	return math::inverse(view_).get_position();
}

math::vec3 camera::x_unit_axis() const
{
	return math::inverse(view_).x_unit_axis();
}
math::vec3 camera::y_unit_axis() const
{
	return math::inverse(view_).y_unit_axis();
}

math::vec3 camera::z_unit_axis() const
{
	return math::inverse(view_).z_unit_axis();
}

const math::frustum& camera::get_frustum() const
{
	// Recalculate frustum if necessary
	if(frustum_dirty_ && !frustum_locked_)
	{
		frustum_.update(get_view(), get_projection(), gfx::is_homogeneous_depth());
		frustum_dirty_ = false;

		// Also build the frustum / volume that represents the space between the
		// camera position and its near plane. This frustum represents the
		// 'volume' that can end up clipping geometry.

		clipping_volume_ = frustum_;
		clipping_volume_.planes[math::volume_plane::far_plane].data.w =
			-clipping_volume_.planes[math::volume_plane::near_plane].data.w; // At near plane
		clipping_volume_.planes[math::volume_plane::near_plane].data.w =
			//-math::dot((math::vec3&)_clipping_volume.planes[math::volume_plane::near_plane],
			//		   get_position()); // At camera
			-math::plane::dot_coord(clipping_volume_.planes[math::volume_plane::near_plane], get_position());
		// The corner points also need adjusting in this case such that they sit
		// precisely on the new planes.
		clipping_volume_.points[math::volume_geometry_point::left_bottom_far] =
			clipping_volume_.points[math::volume_geometry_point::left_bottom_near];
		clipping_volume_.points[math::volume_geometry_point::left_top_far] =
			clipping_volume_.points[math::volume_geometry_point::left_top_near];
		clipping_volume_.points[math::volume_geometry_point::right_bottom_far] =
			clipping_volume_.points[math::volume_geometry_point::right_bottom_near];
		clipping_volume_.points[math::volume_geometry_point::right_top_far] =
			clipping_volume_.points[math::volume_geometry_point::right_top_near];
		clipping_volume_.points[math::volume_geometry_point::left_bottom_near] = clipping_volume_.position;
		clipping_volume_.points[math::volume_geometry_point::left_top_near] = clipping_volume_.position;
		clipping_volume_.points[math::volume_geometry_point::right_bottom_near] = clipping_volume_.position;
		clipping_volume_.points[math::volume_geometry_point::right_top_near] = clipping_volume_.position;

	} // End if recalc frustum

	// Return the frustum
	return frustum_;
}

const math::frustum& camera::get_clipping_volume() const
{
	// Recalculate frustum if necessary
	if(frustum_dirty_ && !frustum_locked_)
	{
		get_frustum();
	}

	// Return the clipping volume
	return clipping_volume_;
}

math::volume_query camera::bounds_in_frustum(const math::bbox& AABB) const
{
	// Recompute the frustum as necessary.
	const math::frustum& f = get_frustum();

	// Request that frustum classifies
	return f.classify_aabb(AABB);
}

math::volume_query camera::bounds_in_frustum(const math::bbox& AABB, const math::transform& t) const
{
	// Recompute the frustum as necessary.
	const math::frustum& f = get_frustum();

	// Request that frustum classifies
	return math::frustum::classify_obb(f, AABB, t);
}

math::vec3 camera::world_to_viewport(const math::vec3& pos) const
{
	// Ensure we have an up-to-date projection and view matrix
	auto view_proj = get_view_projection();

	// Transform the point into clip space
	math::vec4 clip = view_proj * math::vec4{pos.x, pos.y, pos.z, 1.0f};

	// Project!
	const float recip_w = 1.0f / clip.w;
	clip.x *= recip_w;
	clip.y *= recip_w;
	clip.z *= recip_w;

	// Transform to final screen space position
	math::vec3 point;
	point.x = ((clip.x * 0.5f) + 0.5f) * float(viewport_size_.width) + float(viewport_pos_.x);
	point.y = ((clip.y * -0.5f) + 0.5f) * float(viewport_size_.height) + float(viewport_pos_.y);
	point.z = clip.z;

	// Point on screen!
	return point;
}

bool camera::viewport_to_ray(const math::vec2& point, math::vec3& vec_ray_start,
							 math::vec3& vec_ray_dir) const
{

	// Ensure we have an up-to-date projection and view matrix
	math::transform mtx_proj = get_projection();
	math::transform mtx_view = get_view();
	math::transform mtx_inv_view = math::inverse(mtx_view);

	// Transform the pick position from view port space into camera space
	math::vec3 cursor;
	cursor.x = (((2.0f * (point.x - viewport_pos_.x)) / float(viewport_size_.width)) - 1.0f) / mtx_proj[0][0];
	cursor.y =
		-(((2.0f * (point.y - viewport_pos_.y)) / float(viewport_size_.height)) - 1.0f) / mtx_proj[1][1];
	cursor.z = 1.0f;

	// Transform the camera space pick ray into 3D space
	if(get_projection_mode() == projection_mode::orthographic)
	{
		// Obtain the ray from the cursor position
		vec_ray_start = math::transform::transform_coord(cursor, mtx_inv_view);
		vec_ray_dir = mtx_inv_view[2];

	} // End If IsOrthohraphic
	else
	{
		// Obtain the ray from the cursor position
		vec_ray_start = mtx_inv_view.get_position();
		vec_ray_dir.x =
			cursor.x * mtx_inv_view[0][0] + cursor.y * mtx_inv_view[1][0] + cursor.z * mtx_inv_view[2][0];
		vec_ray_dir.y =
			cursor.x * mtx_inv_view[0][1] + cursor.y * mtx_inv_view[1][1] + cursor.z * mtx_inv_view[2][1];
		vec_ray_dir.z =
			cursor.x * mtx_inv_view[0][2] + cursor.y * mtx_inv_view[1][2] + cursor.z * mtx_inv_view[2][2];

	} // End If !IsOrthohraphic
	// Success!
	return true;
}

bool camera::viewport_to_world(const math::vec2& point, const math::plane& pl, math::vec3& world_pos,
							   bool clip) const
{

	if(clip && ((point.x < viewport_pos_.x) || (point.x > (viewport_pos_.x + viewport_size_.width)) ||
				(point.y < viewport_pos_.y) || (point.y > (viewport_pos_.y + viewport_size_.height))))
	{
		return false;
	}

	math::vec3 pick_ray_dir;
	math::vec3 pick_ray_origin;
	// Convert the screen coordinates to a ray.
	if(!viewport_to_ray(point, pick_ray_origin, pick_ray_dir))
	{
		return false;
	}

	// Get the length of the 'adjacent' side of the virtual triangle formed
	// by the direction and normal.
	float proj_ray_length = math::plane::dot_normal(pl, pick_ray_dir);
	if(math::abs<float>(proj_ray_length) < math::epsilon<float>())
	{
		return false;
	}

	// Calculate distance to plane along its normal
	float distance = math::plane::dot_normal(pl, pick_ray_origin) + pl.data.w;

	// If both the "direction" and origin are on the same side of the plane
	// then we can't possibly intersect (perspective rule only)
	if(get_projection_mode() == projection_mode::perspective)
	{
		int nSign1 = (distance > 0) ? 1 : (distance < 0) ? -1 : 0;
		int nSign2 = (proj_ray_length > 0) ? 1 : (proj_ray_length < 0) ? -1 : 0;
		if(nSign1 == nSign2)
		{
			return false;
		}

	} // End if perspective

	// Calculate the actual interval (Distance along the adjacent side / length of
	// adjacent side).
	distance /= -proj_ray_length;

	// Store the results
	world_pos = pick_ray_origin + (pick_ray_dir * distance);

	// Success!
	return true;
}

bool camera::viewport_to_major_axis(const math::vec2& point, const math::vec3& Origin, math::vec3& world_pos,
									math::vec3& major_axis) const
{
	return viewport_to_major_axis(point, Origin, z_unit_axis(), world_pos, major_axis);
}

bool camera::viewport_to_major_axis(const math::vec2& point, const math::vec3& Origin,
									const math::vec3& Normal, math::vec3& world_pos,
									math::vec3& major_axis) const
{
	// First select the major axis plane based on the specified normal
	major_axis = math::vec3(1, 0, 0); // YZ

	// Get absolute normal vector
	float x = math::abs<float>(Normal.x);
	float y = math::abs<float>(Normal.y);
	float z = math::abs<float>(Normal.z);

	// If all the components are effectively equal, select one plane
	if(math::abs<float>(x - y) < math::epsilon<float>() && math::abs<float>(x - z) < math::epsilon<float>())
	{
		major_axis = math::vec3(0, 0, 1); // XY

	} // End if components equal
	else
	{
		// Calculate which component of the normal is the major axis
		float norm = x;
		if(norm < y)
		{
			norm = y;
			major_axis = math::vec3(0, 1, 0);
		} // XZ
		if(norm < z)
		{
			norm = z;
			major_axis = math::vec3(0, 0, 1);
		} // XY

	} // End if perform compare

	// Generate the intersection plane based on this information
	// and pass through to the standard viewportToWorld method
	math::plane p = math::plane::from_point_normal(Origin, major_axis);
	return viewport_to_world(point, p, world_pos, false);
}

bool camera::viewport_to_camera(const math::vec3& point, math::vec3& camera_pos) const
{
	// Ensure that we have an up-to-date projection and view matrix
	auto& mtx_proj = get_projection();

	// Transform the pick position from screen space into camera space
	camera_pos.x =
		(((2.0f * (point.x - viewport_pos_.x)) / float(viewport_size_.width)) - 1) / mtx_proj[0][0];
	camera_pos.y =
		-(((2.0f * (point.y - viewport_pos_.y)) / float(viewport_size_.height)) - 1) / mtx_proj[1][1];
	camera_pos.z = get_near_clip();

	// Success!
	return true;
}

float camera::estimate_zoom_factor(const math::plane& pl) const
{

	// Just return the actual zoom factor if this is orthographic
	if(get_projection_mode() == projection_mode::orthographic)
	{
		return get_zoom_factor();
	}

	math::vec3 world;
	// Otherwise, estimate is based on the distance from the grid plane.
	viewport_to_world(math::vec2(float(viewport_size_.width) / 2.0f, float(viewport_size_.height) / 2.0f), pl,
					  world, false);

	// Perform full position based estimation
	return estimate_zoom_factor(world);
}

//-----------------------------------------------------------------------------
// Name : estimateZoomFactor ()
/// <summary>
/// Given the current viewport type and projection mode, estimate the "zoom"
/// factor that can be used for scaling various operations relative to the
/// "scale" of an object as it appears in the viewport at the specified
/// position.
/// </summary>
//-----------------------------------------------------------------------------
float camera::estimate_zoom_factor(const math::vec3& world_pos) const
{
	return estimate_zoom_factor(world_pos, std::numeric_limits<float>::max());
}

float camera::estimate_zoom_factor(const math::plane& pl, float max_val) const
{
	// Just return the actual zoom factor if this is orthographic
	if(get_projection_mode() == projection_mode::orthographic)
	{
		float factor = get_zoom_factor();
		return math::min(max_val, factor);

	} // End if orthographic
	// Otherwise, estimate is based on the distance from the grid plane.
	math::vec3 world;
	viewport_to_world(math::vec2(float(viewport_size_.width) / 2.0f, float(viewport_size_.height) / 2.0f), pl,
					  world, false);

	// Perform full position based estimation
	return estimate_zoom_factor(world, max_val);
}

float camera::estimate_zoom_factor(const math::vec3& world_pos, float max_val) const
{
	// Just return the actual zoom factor if this is orthographic
	if(get_projection_mode() == projection_mode::orthographic)
	{
		float factor = get_zoom_factor();
		return math::min(max_val, factor);

	} // End if orthographic

	// New Zoom factor is based on the distance to this position
	// along the camera's look vector.
	math::vec3 view_pos = math::transform::transform_coord(world_pos, get_view());
	float distance = view_pos.z / (float(viewport_size_.height) * (45.0f / get_fov()));
	return std::min<float>(max_val, distance);
}

math::vec3 camera::estimate_pick_tolerance(float wire_tolerance, const math::vec3& pos,
										   const math::transform& object_transform) const
{
	// Scale tolerance based on estimated world space zoom factor.
	math::vec3 v = object_transform.transform_coord(pos);
	wire_tolerance *= estimate_zoom_factor(v);

	// Convert into object space tolerance.
	math::vec3 Objectwire_tolerance;
	math::vec3 vAxisScale = object_transform.get_scale();
	Objectwire_tolerance.x = wire_tolerance / vAxisScale.x;
	Objectwire_tolerance.y = wire_tolerance / vAxisScale.y;
	Objectwire_tolerance.z = wire_tolerance / vAxisScale.z;
	return Objectwire_tolerance;
}

void camera::record_current_matrices()
{
	last_view_ = get_view();
	last_projection_ = get_projection();
}

void camera::set_aa_data(const usize32_t& viewport_size, std::uint32_t current_subpixel_index,
						 std::uint32_t temporal_aa_samples)
{
	if(temporal_aa_samples > 1)
	{
		float SampleX = math::halton(current_subpixel_index, 2) - 0.5f;
		float SampleY = math::halton(current_subpixel_index, 3) - 0.5f;
		if(temporal_aa_samples == 2)
		{

			float SamplesX[] = {-4.0f / 16.0f, 4.0f / 16.0f};
			float SamplesY[] = {4.0f / 16.0f, -4.0f / 16.0f};

			std::uint32_t Index = current_subpixel_index;
			SampleX = SamplesX[Index];
			SampleY = SamplesY[Index];
		}
		else if(temporal_aa_samples == 3)
		{
			// 3xMSAA
			//   A..
			//   ..B
			//   .C.
			// Rolling circle pattern (A,B,C).
			float SamplesX[] = {-2.0f / 3.0f, 2.0f / 3.0f, 0.0f / 3.0f};
			float SamplesY[] = {-2.0f / 3.0f, 0.0f / 3.0f, 2.0f / 3.0f};
			std::uint32_t Index = current_subpixel_index;
			SampleX = SamplesX[Index];
			SampleY = SamplesY[Index];
		}
		else if(temporal_aa_samples == 4)
		{
			// 4xMSAA
			// Pattern docs:
			// http://msdn.microsoft.com/en-us/library/windows/desktop/ff476218(v=vs.85).aspx
			//   .N..
			//   ...E
			//   W...
			//   ..S.
			// Rolling circle pattern (N,E,S,W).
			float SamplesX[] = {-2.0f / 16.0f, 6.0f / 16.0f, 2.0f / 16.0f, -6.0f / 16.0f};
			float SamplesY[] = {-6.0f / 16.0f, -2.0f / 16.0f, 6.0f / 16.0f, 2.0f / 16.0f};
			std::uint32_t Index = current_subpixel_index;
			SampleX = SamplesX[Index];
			SampleY = SamplesY[Index];
		}
		else if(temporal_aa_samples == 8)
		{
			// This works better than various orderings of 8xMSAA.
			std::uint32_t Index = current_subpixel_index;
			SampleX = math::halton(Index, 2) - 0.5f;
			SampleY = math::halton(Index, 3) - 0.5f;
		}
		else
		{
			// More than 8 samples can improve quality.
			std::uint32_t Index = current_subpixel_index;
			SampleX = math::halton(Index, 2) - 0.5f;
			SampleY = math::halton(Index, 3) - 0.5f;
		}

		aa_data_ = math::vec4(float(current_subpixel_index), float(temporal_aa_samples), SampleX, SampleY);

		float width = static_cast<float>(viewport_size.width);
		float height = static_cast<float>(viewport_size.height);
		aa_data_.z *= (2.0f / width);
		aa_data_.w *= (2.0f / height);
	}
	else
	{
		aa_data_ = math::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	projection_dirty_ = true;
}

void camera::touch()
{
	// All modifications require projection matrix and
	// frustum to be updated.
	view_dirty_ = true;
	projection_dirty_ = true;
	frustum_dirty_ = true;
}

camera camera::get_face_camera(uint32_t face, const math::transform& transform)
{
	camera cam;
	cam.set_fov(90.0f);
	cam.set_aspect_ratio(1.0f, true);
	cam.set_near_clip(0.01f);
	cam.set_far_clip(256.0f);

	// Configurable axis vectors used to construct view matrices. In the
	// case of the omni light, we align all frustums to the world axes.
	math::vec3 X(1, 0, 0);
	math::vec3 Y(0, 1, 0);
	math::vec3 Z(0, 0, 1);
	math::vec3 Zero(0, 0, 0);
	math::transform t;
	// Generate the correct view matrix for the frustum

	switch(face)
	{
		case 0: // right
			t.set_rotation(-Z, +Y, +X);
			break;
		case 1: // left
			t.set_rotation(+Z, +Y, -X);
			break;
		case 2: // up
			if(!gfx::is_origin_bottom_left())
			{
				t.set_rotation(+X, -Z, +Y);
			}
			else
			{
				t.set_rotation(+X, +Z, -Y);
			}
			break;
		case 3: // down
			if(!gfx::is_origin_bottom_left())
			{
				t.set_rotation(+X, +Z, -Y);
			}
			else
			{
				t.set_rotation(+X, -Z, +Y);
			}
			break;
		case 4: // front
			t.set_rotation(+X, +Y, +Z);
			break;
		case 5: // back
			t.set_rotation(-X, +Y, -Z);
			break;
	}

	t = transform * t;
	// First update so the camera can cache the previous matrices
	cam.record_current_matrices();
	// Set new transform
	cam.look_at(t.get_position(), t.get_position() + t.z_unit_axis(), t.y_unit_axis());

	return cam;
}
