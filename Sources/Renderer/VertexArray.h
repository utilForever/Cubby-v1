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

enum VertexType
{
	POSITION = 0,
	POSITION_DIFFUSE,
	POSITION_DIFFUSE_ALPHA,
	POSITION_NORMAL,
	POSITION_NORMAL_COLOR,
	POSITION_NORMAL_UV,
	POSITION_NORMAL_UV_COLOR,
};

class VertexArray
{
public:
	// Destructor
	~VertexArray();

private:
	VertexType m_type;

	unsigned int m_materialID;
	unsigned int m_textureID;

	int m_numVertices;
	int m_numTextureCoordinates;
	int m_numIndices;

	float* m_pVertexArray;
	float* m_pTextureCoordinates;
	unsigned int* m_pIndices;

	int m_vertexSize;
	int m_textureCoordinateSize;
};

#endif