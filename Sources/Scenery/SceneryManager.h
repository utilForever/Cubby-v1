/*************************************************************************
> File Name: SceneryManager.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Scenery manager class.
> Created Time: 2016/07/14
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_SCENERY_MANAGER_H
#define CUBBY_SCENERY_MANAGER_H

#include <Blocks/ChunkManager.h>
#include <Renderer/Renderer.h>

struct SceneryObject
{
	std::string m_name;
	std::string m_objectFileName;
	glm::vec3 m_positionOffset;
	glm::vec3 m_worldFileOffset;

	QubicleImportDirection m_importDirection;
	QubicleImportDirection m_parentImportDirection;

	QubicleBinary* m_pQubicleBinaryFile;

	float m_length;
	float m_height;
	float m_width;

	float m_scale;
	float m_rotation;

	float m_radius;

	bool m_canSelect;

	bool m_outlineRender;
	bool m_hoverRender;

	glm::vec2 m_screenPosition;
};

using SceneryObjectList = std::vector<SceneryObject*>;

class SceneryManager
{
public:
	// Constructor, Destructor
	SceneryManager(Renderer* pRenderer, ChunkManager* pChunkManager);
	~SceneryManager();

	void ClearSceneryObjects();

	int GetNumSceneryObjects() const;
	void ResetNumRenderSceneryCounter();
	int GetNumRenderSceneryObjects() const;
	SceneryObject* GetSceneryObject(int index);
	SceneryObject* GetSceneryObject(std::string name);
	SceneryObject* AddSceneryObject(std::string name, std::string fileName, glm::vec3 pos, glm::vec3 worldFileOffset, QubicleImportDirection importDirection, QubicleImportDirection parentImportDirection, float scale, float rotation);
	SceneryObject* AddSceneryObject(std::string name, std::string fileName, glm::vec3 pos, glm::vec3 worldFileOffset, QubicleImportDirection importDirection, QubicleImportDirection parentImportDirection, QubicleBinary* pQubicleBinaryFile, float length, float height, float width, float scale, float rotation);
	void DeleteSceneryObject(std::string name);

	void DeleteLayout(std::string name);
	void UpdateLayoutPosition(std::string name, glm::vec3 newPosition);
	void UpdateLayoutDirection(std::string name, QubicleImportDirection direction);

	// Render modes
	void SetRenderOutlines(bool outlines);
	void SetRenderLabels(bool labels);

	// Rendering
	void Render(bool reflection, bool silhouette, bool shadow, bool renderOnlyOutline, bool renderOnlyNormal);
	void RenderDebug();
	void RenderSceneryObject(SceneryObject* pSceneryObject, bool outline, bool reflection, bool silhouette, bool boundingBox, bool shadow) const;
	void RenderOutlineScenery();

private:
	Renderer* m_pRenderer;
	ChunkManager* m_pChunkManager;

	SceneryObjectList m_vpSceneryObjectList;

	int m_numRenderScenery;

	bool m_renderOutlines;
	bool m_renderLabels;
};

#endif