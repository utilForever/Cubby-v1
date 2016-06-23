/*************************************************************************
> File Name: Mesh.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A generic mesh object, used to render complex geometrix shapes.
> Created Time: 2016/06/24
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "Mesh.h"

TriangleMesh::TriangleMesh() :
	m_staticMeshID(-1), m_materialID(-1), m_textureID(-1), m_meshType(MeshType::Color)
{
	
}

TriangleMesh::~TriangleMesh()
{
	// Delete the vertices
	for (unsigned int i = 0; i < m_vertices.size(); ++i)
	{
		delete m_vertices[i];
		m_vertices[i] = nullptr;
	}

	// Delete the texture coordinates
	for (unsigned int i = 0; i < m_textureCoordinates.size(); ++i)
	{
		delete m_textureCoordinates[i];
		m_textureCoordinates[i] = nullptr;
	}

	// Delete the triangles
	for (unsigned int i = 0; i < m_triangles.size(); ++i)
	{
		delete m_triangles[i];
		m_triangles[i] = nullptr;
	}
}