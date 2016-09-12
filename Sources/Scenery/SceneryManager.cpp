/*************************************************************************
> File Name: SceneryManager.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Scenery manager class.
> Created Time: 2016/07/14
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "SceneryManager.h"

// Constructor, Destructor
SceneryManager::SceneryManager(Renderer* pRenderer, ChunkManager* pChunkManager) :
	m_pRenderer(pRenderer), m_pChunkManager(pChunkManager),
	m_numRenderScenery(0), m_renderOutlines(false), m_renderLabels(false)
{

}

SceneryManager::~SceneryManager()
{
	ClearSceneryObjects();
}

void SceneryManager::ClearSceneryObjects()
{
	for (size_t i = 0; i < m_vpSceneryObjectList.size(); ++i)
	{
		delete m_vpSceneryObjectList[i];
		m_vpSceneryObjectList[i] = nullptr;
	}

	m_vpSceneryObjectList.clear();
}

int SceneryManager::GetNumSceneryObjects() const
{
	return m_vpSceneryObjectList.size();
}

void SceneryManager::ResetNumRenderSceneryCounter()
{
	m_numRenderScenery = 0;
}

int SceneryManager::GetNumRenderSceneryObjects() const
{
	return m_numRenderScenery;
}

SceneryObject* SceneryManager::GetSceneryObject(int index)
{
	return m_vpSceneryObjectList[index];
}

SceneryObject* SceneryManager::GetSceneryObject(std::string name)
{
	for (size_t i = 0; i < m_vpSceneryObjectList.size(); ++i)
	{
		if (m_vpSceneryObjectList[i]->m_name == name)
		{
			return m_vpSceneryObjectList[i];
		}
	}

	return nullptr;
}

SceneryObject* SceneryManager::AddSceneryObject(std::string name, std::string fileName, glm::vec3 pos, glm::vec3 worldFileOffset, QubicleImportDirection importDirection, QubicleImportDirection parentImportDirection, float scale, float rotation)
{
	// TODO: Should add back in duplicate name check?
	
	QubicleBinary* pQubicleBinaryFile = new QubicleBinary(m_pRenderer);
	pQubicleBinaryFile->Import(fileName.c_str(), true);

	return AddSceneryObject(name, fileName, pos, worldFileOffset, importDirection, parentImportDirection, pQubicleBinaryFile, static_cast<float>(pQubicleBinaryFile->GetQubicleMatrix(0)->m_matrixSizeX), static_cast<float>(pQubicleBinaryFile->GetQubicleMatrix(0)->m_matrixSizeY), static_cast<float>(pQubicleBinaryFile->GetQubicleMatrix(0)->m_matrixSizeZ), scale, rotation);
}

SceneryObject* SceneryManager::AddSceneryObject(std::string name, std::string fileName, glm::vec3 pos, glm::vec3 worldFileOffset, QubicleImportDirection importDirection, QubicleImportDirection parentImportDirection, QubicleBinary* pQubicleBinaryFile, float length, float height, float width, float scale, float rotation)
{
	SceneryObject* pNewSceneryObject = new SceneryObject();

	pNewSceneryObject->m_name = name;
	pNewSceneryObject->m_objectFileName = fileName;
	pNewSceneryObject->m_positionOffset = pos;
	pNewSceneryObject->m_worldFileOffset = worldFileOffset;
	pNewSceneryObject->m_importDirection = importDirection;
	pNewSceneryObject->m_parentImportDirection = parentImportDirection;

	pNewSceneryObject->m_pQubicleBinaryFile = pQubicleBinaryFile;
	pNewSceneryObject->m_length = length;
	pNewSceneryObject->m_height = height;
	pNewSceneryObject->m_width = width;

	pNewSceneryObject->m_scale = scale;
	pNewSceneryObject->m_rotation = rotation;

	pNewSceneryObject->m_radius = 1.0f;

	pNewSceneryObject->m_canSelect = true;
	pNewSceneryObject->m_outlineRender = false;
	pNewSceneryObject->m_hoverRender = false;

	m_vpSceneryObjectList.push_back(pNewSceneryObject);

	return pNewSceneryObject;
}

void SceneryManager::DeleteSceneryObject(std::string name)
{
	SceneryObject* pDeleteObject = nullptr;

	// Find
	for (size_t i = 0; i < m_vpSceneryObjectList.size(); ++i)
	{
		if (m_vpSceneryObjectList[i]->m_name == name)
		{
			pDeleteObject = m_vpSceneryObjectList[i];
		}
	}

	// Erase from container
	SceneryObjectList::iterator iter = find(m_vpSceneryObjectList.begin(), m_vpSceneryObjectList.end(), pDeleteObject);
	
	if (iter != m_vpSceneryObjectList.end())
	{
		m_vpSceneryObjectList.erase(iter);
	}

	// Delete
	if (pDeleteObject != nullptr)
	{
		delete pDeleteObject;
	}
}

void SceneryManager::DeleteLayout(std::string name)
{
	std::string nameToSearch = name + ".";
	SceneryObjectList vDeleteList;

	for (size_t i = 0; i < m_vpSceneryObjectList.size(); ++i)
	{
		if (m_vpSceneryObjectList[i]->m_name.find(nameToSearch) != std::string::npos)
		{
			vDeleteList.push_back(m_vpSceneryObjectList[i]);
		}
	}

	for (size_t i = 0; i < vDeleteList.size(); ++i)
	{
		DeleteSceneryObject(vDeleteList[i]->m_name);
	}
}

void SceneryManager::UpdateLayoutPosition(std::string name, glm::vec3 newPosition)
{
	std::string nameToSearch = name + ".";

	for (size_t i = 0; i < m_vpSceneryObjectList.size(); ++i)
	{
		if (m_vpSceneryObjectList[i]->m_name.find(nameToSearch) != std::string::npos)
		{
			m_vpSceneryObjectList[i]->m_worldFileOffset.x = static_cast<float>(static_cast<int>(newPosition.x));
			m_vpSceneryObjectList[i]->m_worldFileOffset.y = static_cast<float>(static_cast<int>(newPosition.y));
			m_vpSceneryObjectList[i]->m_worldFileOffset.z = static_cast<float>(static_cast<int>(newPosition.z));
		}
	}
}

void SceneryManager::UpdateLayoutDirection(std::string name, QubicleImportDirection direction)
{
	std::string nameToSearch = name + ".";

	for (size_t i = 0; i < m_vpSceneryObjectList.size(); ++i)
	{
		if (m_vpSceneryObjectList[i]->m_name.find(nameToSearch) != std::string::npos)
		{
			m_vpSceneryObjectList[i]->m_parentImportDirection = direction;
		}
	}
}

// Render modes
void SceneryManager::SetRenderOutlines(bool outlines)
{
	m_renderOutlines = outlines;
}

void SceneryManager::SetRenderLabels(bool labels)
{
	m_renderLabels = labels;
}

// Rendering
void SceneryManager::Render(bool reflection, bool silhouette, bool shadow, bool renderOnlyOutline, bool renderOnlyNormal)
{
	for (size_t i = 0; i < m_vpSceneryObjectList.size(); ++i)
	{
		SceneryObject* pSceneryObject = m_vpSceneryObjectList[i];

		if (silhouette && (pSceneryObject->m_outlineRender == false && pSceneryObject->m_hoverRender == false))
		{
			// Don't render silhouette unless we are rendering outline
			continue; 
		}

		if (renderOnlyNormal && (pSceneryObject->m_outlineRender || pSceneryObject->m_hoverRender))
		{
			continue;
		}
		if (renderOnlyOutline && pSceneryObject->m_outlineRender == false && pSceneryObject->m_hoverRender == false)
		{
			continue;
		}

		bool renderBounding = false;
		RenderSceneryObject(pSceneryObject, false, reflection, silhouette, renderBounding, shadow);

		m_numRenderScenery++;
	}
}

void SceneryManager::RenderDebug()
{
	for (size_t i = 0; i < m_vpSceneryObjectList.size(); ++i)
	{
		SceneryObject* pSceneryObject = m_vpSceneryObjectList[i];

		glm::vec3 pos = pSceneryObject->m_worldFileOffset + pSceneryObject->m_positionOffset;

		m_pRenderer->PushMatrix();

		m_pRenderer->TranslateWorldMatrix(pos.x, pos.y, pos.z);
		m_pRenderer->SetLineWidth(1.0f);
		m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);

		m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
		m_pRenderer->DrawSphere(pSceneryObject->m_radius, 20, 20);

		m_pRenderer->PopMatrix();
	}
}

void SceneryManager::RenderSceneryObject(SceneryObject* pSceneryObject, bool outline, bool reflection, bool silhouette, bool boundingBox, bool shadow) const
{
	m_pRenderer->PushMatrix();

	// First translate to world file origin
	m_pRenderer->TranslateWorldMatrix(pSceneryObject->m_worldFileOffset.x, pSceneryObject->m_worldFileOffset.y, pSceneryObject->m_worldFileOffset.z);

	bool switchedFaceCulling = false;
	switch (pSceneryObject->m_parentImportDirection)
	{
	case QubicleImportDirection::Normal:		{  } break;
	case QubicleImportDirection::MirrorX:		{ m_pRenderer->ScaleWorldMatrix(-1.0f, 1.0f, 1.0f); switchedFaceCulling = !switchedFaceCulling; } break;
	case QubicleImportDirection::MirrorY:		{ m_pRenderer->ScaleWorldMatrix(1.0f, -1.0f, 1.0f); switchedFaceCulling = !switchedFaceCulling; } break;
	case QubicleImportDirection::MirrorZ:		{ m_pRenderer->ScaleWorldMatrix(1.0f, 1.0f, -1.0f); switchedFaceCulling = !switchedFaceCulling; } break;
	case QubicleImportDirection::RotateY90:		{ m_pRenderer->RotateWorldMatrix(0.0f, -90.0f, 0.0f); } break;
	case QubicleImportDirection::RotateY180:	{ m_pRenderer->RotateWorldMatrix(0.0f, -180.0f, 0.0f); } break;
	case QubicleImportDirection::RotateY270:	{ m_pRenderer->RotateWorldMatrix(0.0f, -270.0f, 0.0f); } break;
	case QubicleImportDirection::RotateX90:		{ m_pRenderer->RotateWorldMatrix(-90.0f, 0.0f, 0.0f); } break;
	case QubicleImportDirection::RotateX180:	{ m_pRenderer->RotateWorldMatrix(-180.0f, 0.0f, 0.0f); } break;
	case QubicleImportDirection::RotateX270:	{ m_pRenderer->RotateWorldMatrix(-270.0f, 0.0f, 0.0f); } break;
	case QubicleImportDirection::RotateZ90:		{ m_pRenderer->RotateWorldMatrix(0.0f, 0.0f, -90.0f); } break;
	case QubicleImportDirection::RotateZ180:	{ m_pRenderer->RotateWorldMatrix(0.0f, 0.0f, -180.0f); } break;
	case QubicleImportDirection::RotateZ270:	{ m_pRenderer->RotateWorldMatrix(0.0f, 0.0f, -270.0f); } break;
	}

	// Now local object offset
	m_pRenderer->TranslateWorldMatrix(pSceneryObject->m_positionOffset.x, pSceneryObject->m_positionOffset.y, pSceneryObject->m_positionOffset.z);

	// Translate for block size offset
	m_pRenderer->TranslateWorldMatrix(0.0f, -Chunk::BLOCK_RENDER_SIZE, 0.0f);

	float length = pSceneryObject->m_length * 0.5f;
	float height = pSceneryObject->m_height * 0.5f;
	float width = pSceneryObject->m_width * 0.5f;

	// Rotate the scenery object
	m_pRenderer->RotateWorldMatrix(0.0f, pSceneryObject->m_rotation, 0.0f);

	// Scale the scenery object
	m_pRenderer->ScaleWorldMatrix(pSceneryObject->m_scale, pSceneryObject->m_scale, pSceneryObject->m_scale);

	// Translate to the center
	m_pRenderer->TranslateWorldMatrix(0.0f, height, 0.0f);

	switch (pSceneryObject->m_importDirection)
	{
	case QubicleImportDirection::Normal:		{  } break;
	case QubicleImportDirection::MirrorX:		{ m_pRenderer->ScaleWorldMatrix(-1.0f, 1.0f, 1.0f); switchedFaceCulling = !switchedFaceCulling; } break;
	case QubicleImportDirection::MirrorY:		{ m_pRenderer->ScaleWorldMatrix(1.0f, -1.0f, 1.0f); switchedFaceCulling = !switchedFaceCulling; } break;
	case QubicleImportDirection::MirrorZ:		{ m_pRenderer->ScaleWorldMatrix(1.0f, 1.0f, -1.0f); switchedFaceCulling = !switchedFaceCulling; } break;
	case QubicleImportDirection::RotateY90:		{ m_pRenderer->RotateWorldMatrix(0.0f, -90.0f, 0.0f); } break;
	case QubicleImportDirection::RotateY180:	{ m_pRenderer->RotateWorldMatrix(0.0f, -180.0f, 0.0f); } break;
	case QubicleImportDirection::RotateY270:	{ m_pRenderer->RotateWorldMatrix(0.0f, -270.0f, 0.0f); } break;
	case QubicleImportDirection::RotateX90:		{ m_pRenderer->RotateWorldMatrix(-90.0f, 0.0f, 0.0f); } break;
	case QubicleImportDirection::RotateX180:	{ m_pRenderer->RotateWorldMatrix(-180.0f, 0.0f, 0.0f); } break;
	case QubicleImportDirection::RotateX270:	{ m_pRenderer->RotateWorldMatrix(-270.0f, 0.0f, 0.0f); } break;
	case QubicleImportDirection::RotateZ90:		{ m_pRenderer->RotateWorldMatrix(0.0f, 0.0f, -90.0f); } break;
	case QubicleImportDirection::RotateZ180:	{ m_pRenderer->RotateWorldMatrix(0.0f, 0.0f, -180.0f); } break;
	case QubicleImportDirection::RotateZ270:	{ m_pRenderer->RotateWorldMatrix(0.0f, 0.0f, -270.0f); } break;
	}

	Color outlineColor(1.0f, 1.0f, 0.0f, 1.0f);

	if (pSceneryObject->m_hoverRender && pSceneryObject->m_outlineRender == false)
	{
		outlineColor = Color(1.0f, 0.0f, 1.0f, 1.0f);
	}

	if (shadow)
	{
		m_pRenderer->SetCullMode(switchedFaceCulling ? CullMode::BACK : CullMode::FRONT);
	}
	else
	{
		m_pRenderer->SetCullMode(switchedFaceCulling ? CullMode::FRONT : CullMode::BACK);
	}

	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);

	pSceneryObject->m_pQubicleBinaryFile->Render(outline, reflection, silhouette, outlineColor);

	if (boundingBox)
	{
		m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
		m_pRenderer->SetCullMode(CullMode::NOCULL);
		m_pRenderer->SetLineWidth(1.0f);

		m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);

		m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 0.0f, 1.0f);
		m_pRenderer->ImmediateNormal(0.0f, 0.0f, -1.0f);
		m_pRenderer->ImmediateVertex(length, -height, -width);
		m_pRenderer->ImmediateVertex(-length, -height, -width);
		m_pRenderer->ImmediateVertex(-length, height, -width);
		m_pRenderer->ImmediateVertex(length, height, -width);

		m_pRenderer->ImmediateNormal(0.0f, 0.0f, 1.0f);
		m_pRenderer->ImmediateVertex(-length, -height, width);
		m_pRenderer->ImmediateVertex(length, -height, width);
		m_pRenderer->ImmediateVertex(length, height, width);
		m_pRenderer->ImmediateVertex(-length, height, width);

		m_pRenderer->ImmediateNormal(1.0f, 0.0f, 0.0f);
		m_pRenderer->ImmediateVertex(length, -height, width);
		m_pRenderer->ImmediateVertex(length, -height, -width);
		m_pRenderer->ImmediateVertex(length, height, -width);
		m_pRenderer->ImmediateVertex(length, height, width);

		m_pRenderer->ImmediateNormal(-1.0f, 0.0f, 0.0f);
		m_pRenderer->ImmediateVertex(-length, -height, -width);
		m_pRenderer->ImmediateVertex(-length, -height, width);
		m_pRenderer->ImmediateVertex(-length, height, width);
		m_pRenderer->ImmediateVertex(-length, height, -width);

		m_pRenderer->ImmediateNormal(0.0f, -1.0f, 0.0f);
		m_pRenderer->ImmediateVertex(-length, -height, -width);
		m_pRenderer->ImmediateVertex(length, -height, -width);
		m_pRenderer->ImmediateVertex(length, -height, width);
		m_pRenderer->ImmediateVertex(-length, -height, width);

		m_pRenderer->ImmediateNormal(0.0f, 1.0f, 0.0f);
		m_pRenderer->ImmediateVertex(length, height, -width);
		m_pRenderer->ImmediateVertex(-length, height, -width);
		m_pRenderer->ImmediateVertex(-length, height, width);
		m_pRenderer->ImmediateVertex(length, height, width);

		m_pRenderer->DisableImmediateMode();

		m_pRenderer->SetCullMode(CullMode::BACK);
	}

	m_pRenderer->PopMatrix();
}

void SceneryManager::RenderOutlineScenery()
{
	for (size_t i = 0; i < m_vpSceneryObjectList.size(); ++i)
	{
		SceneryObject* pSceneryObject = m_vpSceneryObjectList[i];

		if (pSceneryObject->m_outlineRender == false && pSceneryObject->m_hoverRender == false)
		{
			continue;
		}

		RenderSceneryObject(pSceneryObject, true, false, false, false, false);
	}
}