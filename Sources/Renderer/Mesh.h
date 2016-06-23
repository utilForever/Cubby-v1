/*************************************************************************
> File Name: Mesh.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A generic mesh object, used to render complex geometrix shapes.
> Created Time: 2016/06/24
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_MESH_H
#define CUBBY_MESH_H

#include <vector>

// Vertex data
typedef struct MeshVertex
{
	float vertexPosition[3];
	float vertexNormals[3];
	float vertexColor[4];
} MeshVertex;

// Texture coordinate
typedef struct MeshTextureCoordinate
{
	float s, t;
} MeshTextureCoordinate;

// Triangle
typedef struct MeshTriangle
{
	unsigned int vertexIndieces[3];
} MeshTriangle;

enum class MeshType
{
	Color = 0,
	Textured,
};

class TriangleMesh
{
public:
	// Constructor, Destructor
	TriangleMesh();
	~TriangleMesh();

private:
	std::vector<MeshTriangle*> m_triangles;
	std::vector<MeshVertex*> m_vertices;
	std::vector<MeshTextureCoordinate*> m_textureCoordinates;

	unsigned int m_staticMeshID;
	unsigned int m_materialID;
	unsigned int m_textureID;

	MeshType m_meshType;
};

#endif