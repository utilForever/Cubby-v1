/*************************************************************************
> File Name: VertexArray.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A vertex array, used for static renderering.
> Created Time: 2016/06/25
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_VERTEX_ARRAY_H
#define CUBBY_VERTEX_ARRAY_H

enum class VertexType
{
	POSITION = 0,
	POSITION_DIFFUSE,
	POSITION_DIFFUSE_ALPHA,
	POSITION_NORMAL,
	POSITION_NORMAL_COLOR,
	POSITION_NORMAL_UV,
	POSITION_NORMAL_UV_COLOR,
};

struct VertexArray
{
	// Destructor
	~VertexArray();

	VertexType type;

	unsigned int materialID;
	unsigned int textureID;

	int numVertices;
	int numTextureCoordinates;
	int numIndices;

	float* pVertexArray;
	float* pTextureCoordinates;
	unsigned int* pIndices;

	int vertexSize;
	int textureCoordinateSize;
};

#endif