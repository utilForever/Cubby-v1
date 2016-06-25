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
	staticMeshID(-1), materialID(-1), textureID(-1), meshType(MeshType::Color)
{
	
}

TriangleMesh::~TriangleMesh()
{
	// Delete the vertices
	for (unsigned int i = 0; i < vertices.size(); ++i)
	{
		delete vertices[i];
		vertices[i] = nullptr;
	}

	// Delete the texture coordinates
	for (unsigned int i = 0; i < textureCoordinates.size(); ++i)
	{
		delete textureCoordinates[i];
		textureCoordinates[i] = nullptr;
	}

	// Delete the triangles
	for (unsigned int i = 0; i < triangles.size(); ++i)
	{
		delete triangles[i];
		triangles[i] = nullptr;
	}
}