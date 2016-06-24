/*************************************************************************
> File Name: VertexArray.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A vertex array, used for static renderering.
> Created Time: 2016/06/25
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "VertexArray.h"

VertexArray::~VertexArray()
{
	if (m_numVertices)
	{
		delete m_pVertexArray;
		m_pVertexArray = nullptr;
	}

	if (m_numIndices)
	{
		delete m_pIndices;
		m_pIndices = nullptr;
	}

	if (m_numTextureCoordinates)
	{
		delete m_pTextureCoordinates;
		m_pTextureCoordinates = nullptr;
	}

	m_numVertices = 0;
	m_numIndices = 0;
	m_numTextureCoordinates = 0;
}