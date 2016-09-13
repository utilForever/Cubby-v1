/*************************************************************************
> File Name: VoxelObject.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Voxel object.
> Created Time: 2016/07/04
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_VOXEL_OBJECT_H
#define CUBBY_VOXEL_OBJECT_H

#include "QubicleBinaryManager.h"

class VoxelObject
{
public:
	// Constructor, Destructor
	VoxelObject();
	~VoxelObject();

	void SetRenderer(Renderer* pRenderer);
	void SetQubicleBinaryManager(QubicleBinaryManager* pQubicleBinaryManager);

	void Reset();

	// Rebuild
	void RebuildVoxelModel(bool isFaceMerge) const;

	QubicleBinary* GetQubicleModel() const;
	Matrix4 GetModelMatrix(int qubicleMatrixIndex) const;

	glm::vec3 GetCenter() const;

	void LoadObject(const char* qbFileName, bool useManager = true);
	void UnloadObject();

	// Rendering modes
	void SetWireFrameRender(bool isWireframe) const;
	void SetMeshAlpha(float alpha) const;
	void SetMeshSingleColor(float r, float g, float b) const;

	void Update(float dt) const;
	void Render(bool renderOutline, bool reflection, bool silhouette, Color outlineColor) const;

private:
	Renderer* m_pRenderer;
	QubicleBinaryManager* m_pQubicleBinaryManager;

	// Loaded flag
	bool m_isLoaded;

	// If we are using the qubicle manager we don't need to delete our QB after use
	bool m_isUsingQubicleManager;

	// The qubicle binary data
	QubicleBinary* m_pVoxelModel;
};

#endif