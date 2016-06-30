/*************************************************************************
> File Name: QubicleBinary.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Import Qubicle binary file.
> Created Time: 2016/06/30
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "QubicleBinary.h"

// Constructor, Destructor
QubicleBinary::QubicleBinary(Renderer* pRenderer) :
	m_pRenderer(pRenderer), m_isLoaded(false),
	m_colorFormat(0), m_zAxisOrientation(0), m_compressed(0), m_visibilityMaskEncoded(0), m_numMatrices(0),
	m_isRenderWireFrame(false), m_meshAlpha(0.0f),
	m_isSingleMeshColor(false), m_meshSingleColorR(0.0f), m_meshSingleColorG(false), m_meshSingleColorB(false),
	m_materialID(0)
{
	Reset();

	pRenderer->CreateMaterial(Color(1.0f, 1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f), 64, &m_materialID);
}

QubicleBinary::~QubicleBinary()
{
	Unload();
	Reset();
}

void QubicleBinary::SetNullLinkage(QubicleBinary* pBinary)
{
	for (auto pMatrix : m_vpMatrices)
	{
		for (int i = 0; i < pBinary->GetNumMatrices(); ++i)
		{
			if (pMatrix == pBinary->GetQubicleMatrix(i))
			{
				pMatrix = nullptr;
			}
		}
	}
}

void QubicleBinary::Unload()
{
	if (m_isLoaded)
	{
		ClearMatrices();
	}
}

void QubicleBinary::ClearMatrices()
{
	for (auto pMatrix : m_vpMatrices)
	{
		if (pMatrix == nullptr)
		{
			continue;
		}

		if (pMatrix->isRemoved == true)
		{
			continue;
		}

		m_pRenderer->ClearMesh(pMatrix->pMesh);
		pMatrix->pMesh = nullptr;

		delete[] pMatrix->pColor;

		delete pMatrix;
		// ReSharper disable once CppAssignedValueIsNeverUsed
		pMatrix = nullptr;
	}

	m_vpMatrices.clear();
}

void QubicleBinary::Reset()
{
	m_version[0] = 0;
	m_version[1] = 0;
	m_version[2] = 0;
	m_version[3] = 0;
	m_colorFormat = 0;
	m_zAxisOrientation = 0;
	m_compressed = 0;
	m_visibilityMaskEncoded = 0;
	m_numMatrices = 0;

	m_isLoaded = false;

	ClearMatrices();

	m_isRenderWireFrame = false;
}