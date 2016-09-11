/*************************************************************************
> File Name: InstanceManager.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    Instanced objects are a group of objects that all share the same render call.
>    Thus we can group together all instances into a single call, and just modify
>    the objects matrix and other render properties in the shader, in a single
>    draw call. This is mostly used for rendering large chunks of similar objects
>    where the only difference is the model matrix (scale, position, rotation),
>    For example, scenery, flowers, clutter, etc.
> Created Time: 2016/07/16
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_INSTANCE_MANAGER_H
#define CUBBY_INSTANCE_MANAGER_H

#include <vector>

#include <glm/vec3.hpp>

#include <Maths/Matrix4.h>

// Forward declaration
class Chunk;
class QubicleBinary;
class Renderer;

struct InstanceObject
{
	void UpdateMatrix(glm::vec3 position, glm::vec3 rotation, float scale)
	{
		m_worldMatrix.SetRotation(rotation.x, rotation.y, rotation.z);
		m_worldMatrix.SetTranslation(position);

		Matrix4 scaleMat;
		scaleMat.SetScale(glm::vec3(scale, scale, scale));
		m_worldMatrix = scaleMat * m_worldMatrix;
	}

	bool m_erase;
	bool m_render;
	Matrix4 m_worldMatrix;

	Chunk* m_pOwningChunk;

	int m_voxelX;
	int m_voxelY;
	int m_voxelZ;
};

using InstanceObjectList = std::vector<InstanceObject*>;

class InstanceParent
{
public:
	unsigned int m_vertexArray;
	unsigned int m_positionBuffer;
	unsigned int m_normalBuffer;
	unsigned int m_colorBuffer;
	unsigned int m_matrixBuffer;

	InstanceObjectList m_vpInstanceObjectList;

	std::string m_modelName;
	QubicleBinary* m_pQubicleBinary;
};

using InstanceParentList = std::vector<InstanceParent*>;

class InstanceManager
{
public:
	// Constructor, Destructor
	InstanceManager(Renderer* pRenderer);
	~InstanceManager();

	void ClearInstanceObjects();

	// Counters
	int GetNumInstanceParents() const;
	int GetNumInstanceObjectsForParent(int parentID);
	int GetNumInstanceRenderObjectsForParent(int parentID);
	int GetTotalNumInstanceObjects();
	int GetTotalNumInstanceRenderObjects();

	// Setup
	void SetupGLBuffers(InstanceParent *pInstanceParent) const;

	// Creation
	InstanceParent* GetInstanceParent(std::string modelName);
	InstanceObject* AddInstanceObject(std::string modelName, glm::vec3 position, glm::vec3 rotation, float instanceScale, Chunk* pOwningChunk, int voxelX, int voxelY, int voxelZ);

	// Rendering modes
	void SetWireFrameRender(bool wireframe);

	// Update
	void Update(float dt);

	// Rendering
	void Render();

private:
	/* Private members */
	Renderer* m_pRenderer;

	// Render modes
	bool m_renderWireFrame;

	// Shader
	unsigned int m_instanceShader;

	// List of instance parents what we render in a single render call for all children instances
	InstanceParentList m_vpInstanceParentList;

	// Timer to check for when to erase instances that are no longer linked to an owning chunk
	float m_checkChunkInstanceTimer;
};

#endif