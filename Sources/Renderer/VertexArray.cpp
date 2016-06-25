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
	if (numVertices)
	{
		delete pVertexArray;
		pVertexArray = nullptr;
	}

	if (numIndices)
	{
		delete pIndices;
		pIndices = nullptr;
	}

	if (numTextureCoordinates)
	{
		delete pTextureCoordinates;
		pTextureCoordinates = nullptr;
	}

	numVertices = 0;
	numIndices = 0;
	numTextureCoordinates = 0;
}