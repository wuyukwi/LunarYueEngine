#pragma once

#include "bbox.h"
#include "math_types.h"
#include "plane.h"
#include "transform.h"
#include <array>
namespace math
{
using namespace glm;

//-----------------------------------------------------------------------------
// Main class declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : bbox_extruded (Class)
/// <summary>
/// Storage for extruded box values and wraps up common functionality
/// </summary>
//-----------------------------------------------------------------------------
struct bbox_extruded
{
	//-------------------------------------------------------------------------
	// Constructors & Destructors
	//-------------------------------------------------------------------------
	bbox_extruded();
	bbox_extruded(const bbox& sourceBounds, const vec3& origin, float range, const transform* tr = nullptr);

	//-------------------------------------------------------------------------
	// Public Methods
	//-------------------------------------------------------------------------
	void reset();
	void extrude(const bbox& sourceBounds, const vec3& origin, float range, const transform* tr = nullptr);
	bool get_edge(unsigned int edge, vec3& v1, vec3& v2) const;
	bool test_line(const vec3& v1, const vec3& v2) const;
	bool test_sphere(const vec3& center, float radius) const;

	//-------------------------------------------------------------------------
	// Public Variables
	//-------------------------------------------------------------------------
	/// Source bounding box minimum extents
	vec3 source_min;
	/// Source bounding box maximum extents
	vec3 source_max;
	/// Origin of the extrusion / projection.
	vec3 projection_point;
	/// Distance to extrude / project.
	float projection_range;
	/// The 6 final extruded planes.
	std::array<plane, 6> extruded_planes;
	/// Flags denoting the points used for each extruded (silhouette) edge.
	std::array<std::array<unsigned int, 2>, 6> silhouette_edges;
	/// Number of edges extruded
	unsigned int edge_count;
};
}
