/*************************************************************************
> File Name: VoxelObject.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Voxel object.
> Created Time: 2016/07/04
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "VoxelObject.h"

// Constructor, Destructor

VoxelObject::VoxelObject() :
	m_isUsingQubicleManager(false)
{
	Reset();
}

VoxelObject::~VoxelObject()
{
	UnloadObject();
	Reset();
}

void VoxelObject::SetRenderer(Renderer* pRenderer)
{
	m_pRenderer = pRenderer;
}

void VoxelObject::SetQubicleBinaryManager(QubicleBinaryManager* pQubicleBinaryManager)
{
	m_pQubicleBinaryManager = pQubicleBinaryManager;
}

void VoxelObject::Reset()
{
	m_pVoxelModel = nullptr;
	m_isLoaded = false;
}

// Rebuild
void VoxelObject::RebuildVoxelModel(bool faceMerge) const
{
	m_pVoxelModel->RebuildMesh(faceMerge);
}

QubicleBinary* VoxelObject::GetQubicleModel() const
{
	return m_pVoxelModel;
}

Matrix4 VoxelObject::GetModelMatrix(int qubicleMatrixIndex) const
{
	if (m_pVoxelModel == nullptr || m_isLoaded == false)
	{
		return Matrix4();
	}

	return m_pVoxelModel->GetModelMatrix(qubicleMatrixIndex);
}

glm::vec3 VoxelObject::GetCenter() const
{
	if (m_pVoxelModel == nullptr || m_isLoaded == false)
	{
		return glm::vec3(0.0f, 0.0f, 0.0f);
	}

	glm::vec3 centerPos;
	
	for (int i = 0; i < m_pVoxelModel->GetNumMatrices(); ++i)
	{
		QubicleMatrix* pMatrix = m_pVoxelModel->GetQubicleMatrix(i);

		centerPos.x += (pMatrix->m_matrixSizeX * 0.5f) * pMatrix->m_scale;
		centerPos.y += (pMatrix->m_matrixSizeY * 0.5f) * pMatrix->m_scale;
		centerPos.z += (pMatrix->m_matrixSizeZ * 0.5f) * pMatrix->m_scale;
	}

	centerPos /= static_cast<float>(m_pVoxelModel->GetNumMatrices());

	return centerPos;
}

void VoxelObject::LoadObject(const char* qbFileName, bool useManager)
{
	m_isUsingQubicleManager = useManager;

	if (m_pVoxelModel == nullptr)
	{
		if (useManager)
		{
			m_pVoxelModel = m_pQubicleBinaryManager->GetQubicleBinaryFile(qbFileName, false);
		}
		else
		{
			m_pVoxelModel = new QubicleBinary(m_pRenderer);
			m_pVoxelModel->Import(qbFileName, true);
		}
	}

	m_isLoaded = true;
}

void VoxelObject::UnloadObject()
{
	if (m_isLoaded)
	{
		if (m_isUsingQubicleManager == false)
		{
			delete m_pVoxelModel;
		}

		m_pVoxelModel = nullptr;
	}

	m_isLoaded = false;
}

// Rendering modes
void VoxelObject::SetWireFrameRender(bool isWireframe) const
{
	if (m_pVoxelModel != nullptr)
	{
		m_pVoxelModel->SetWireFrameRender(isWireframe);
	}
}

void VoxelObject::SetMeshAlpha(float alpha) const
{
	if (m_pVoxelModel != nullptr)
	{
		m_pVoxelModel->SetMeshAlpha(alpha);
	}
}

void VoxelObject::SetMeshSingleColor(float r, float g, float b) const
{
	if (m_pVoxelModel != nullptr)
	{
		m_pVoxelModel->SetMeshSingleColor(r, g, b);
	}
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void VoxelObject::Update(float dt) const
{
	// Do nothing
}

void VoxelObject::Render(bool renderOutline, bool reflection, bool silhouette, Color outlineColor) const
{
	if (m_pVoxelModel != nullptr)
	{
		m_pVoxelModel->Render(renderOutline, reflection, silhouette, outlineColor);
	}
}