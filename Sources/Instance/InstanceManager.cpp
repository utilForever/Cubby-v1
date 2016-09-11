/*************************************************************************
> File Name: InstanceManager.cpp
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

#include <algorithm>

#include <Blocks/Chunk.h>
#include <Models/QubicleBinary.h>
#include <Renderer/Renderer.h>

#include "InstanceManager.h"

// Constructor, Destructor
InstanceManager::InstanceManager(Renderer* pRenderer) :
	m_pRenderer(pRenderer), m_renderWireFrame(false), m_instanceShader(-1), m_checkChunkInstanceTimer(0.0f)
{
	m_pRenderer->LoadGLSLShader("Resources/Shaders/instance.vertex", "Resources/Shaders/instance.pixel", &m_instanceShader);
}

InstanceManager::~InstanceManager()
{	
	ClearInstanceObjects();
}

void InstanceManager::ClearInstanceObjects()
{
	for (size_t i = 0; i < m_vpInstanceParentList.size(); ++i)
	{
		for (size_t j = 0; j < m_vpInstanceParentList[i]->m_vpInstanceObjectList.size(); ++j)
		{
			delete m_vpInstanceParentList[i]->m_vpInstanceObjectList[j];
			m_vpInstanceParentList[i]->m_vpInstanceObjectList[j] = nullptr;
		}

		m_vpInstanceParentList[i]->m_vpInstanceObjectList.clear();

		delete m_vpInstanceParentList[i]->m_pQubicleBinary;

		delete m_vpInstanceParentList[i];
		m_vpInstanceParentList[i] = nullptr;
	}

	m_vpInstanceParentList.clear();
}

// Counters
int InstanceManager::GetNumInstanceParents() const
{
	int numInstanceParents = m_vpInstanceParentList.size();

	return numInstanceParents;
}

int InstanceManager::GetNumInstanceObjectsForParent(int parentID)
{
	int numInstanceObjects = m_vpInstanceParentList[parentID]->m_vpInstanceObjectList.size();

	return numInstanceObjects;
}

int InstanceManager::GetNumInstanceRenderObjectsForParent(int parentID)
{
	int renderCounter = 0;

	for (int i = 0; i < m_vpInstanceParentList[parentID]->m_vpInstanceObjectList.size(); ++i)
	{
		if (m_vpInstanceParentList[parentID]->m_vpInstanceObjectList[i]->m_render == false)
		{
			continue;
		}

		renderCounter += 1;
	}

	return renderCounter;
}

int InstanceManager::GetTotalNumInstanceObjects()
{
	int counter = 0;

	for (int instanceParentID = 0; instanceParentID < m_vpInstanceParentList.size(); ++instanceParentID)
	{
		counter += static_cast<int>(m_vpInstanceParentList[instanceParentID]->m_vpInstanceObjectList.size());
	}

	return counter;
}

int InstanceManager::GetTotalNumInstanceRenderObjects()
{
	int renderCounter = 0;

	for (int instanceParentID = 0; instanceParentID < m_vpInstanceParentList.size(); ++instanceParentID)
	{
		renderCounter += GetNumInstanceRenderObjectsForParent(instanceParentID);
	}

	return renderCounter;
}

// Setup
void InstanceManager::SetupGLBuffers(InstanceParent* pInstanceParent) const
{
	pInstanceParent->m_vertexArray = -1;
	pInstanceParent->m_positionBuffer = -1;
	pInstanceParent->m_normalBuffer = -1;
	pInstanceParent->m_colorBuffer = -1;
	pInstanceParent->m_matrixBuffer = -1;

	pInstanceParent->m_pQubicleBinary = new QubicleBinary(m_pRenderer);
	pInstanceParent->m_pQubicleBinary->Import(pInstanceParent->m_modelName.c_str(), true);

	TriangleMesh* pMesh = pInstanceParent->m_pQubicleBinary->GetQubicleMatrix(0)->m_pMesh;

	glShader* pShader = m_pRenderer->GetShader(m_instanceShader);

	GLint inPosition = glGetAttribLocation(pShader->GetProgramObject(), "in_position");
	GLint inNormal = glGetAttribLocation(pShader->GetProgramObject(), "in_normal");
	GLint inColor = glGetAttribLocation(pShader->GetProgramObject(), "in_color");

	glBindFragDataLocation(pShader->GetProgramObject(), 0, "outputColor");
	glBindFragDataLocation(pShader->GetProgramObject(), 1, "outputPosition");
	glBindFragDataLocation(pShader->GetProgramObject(), 2, "outputNormal");

	glGenVertexArrays(1, &pInstanceParent->m_vertexArray);
	glBindVertexArray(pInstanceParent->m_vertexArray);

	float* vertices = new float[4 * static_cast<int>(pMesh->vertices.size())];
	float* normals = new float[4 * static_cast<int>(pMesh->vertices.size())];
	float* colors = new float[4 * static_cast<int>(pMesh->vertices.size())];
	
	int counter = 0;
	
	for (int i = 0; i < pMesh->vertices.size(); i++)
	{
		// Vertices
		vertices[counter + 0] = pMesh->vertices[i]->vertexPosition[0];
		vertices[counter + 1] = pMesh->vertices[i]->vertexPosition[1];
		vertices[counter + 2] = pMesh->vertices[i]->vertexPosition[2];
		vertices[counter + 3] = 1.0f;

		// Normals
		normals[counter + 0] = pMesh->vertices[i]->vertexNormals[0];
		normals[counter + 1] = pMesh->vertices[i]->vertexNormals[1];
		normals[counter + 2] = pMesh->vertices[i]->vertexNormals[2];
		normals[counter + 3] = 1.0f;

		// Colors
		colors[counter + 0] = pMesh->vertices[i]->vertexColor[0];
		colors[counter + 1] = pMesh->vertices[i]->vertexColor[1];
		colors[counter + 2] = pMesh->vertices[i]->vertexColor[2];
		colors[counter + 3] = 1.0f;

		counter += 4;
	}

	if (pInstanceParent->m_positionBuffer != -1)
	{
		glDeleteBuffers(1, &pInstanceParent->m_positionBuffer);
	}

	glGenBuffers(1, &pInstanceParent->m_positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, pInstanceParent->m_positionBuffer);

	int sizeOfVertices = sizeof(float) * 4 * static_cast<int>(pMesh->vertices.size());
	glBufferData(GL_ARRAY_BUFFER, sizeOfVertices, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(inPosition);
	glVertexAttribPointer(inPosition, 4, GL_FLOAT, 0, 0, nullptr);

	if (pInstanceParent->m_normalBuffer != -1)
	{
		glDeleteBuffers(1, &pInstanceParent->m_normalBuffer);
	}

	glGenBuffers(1, &pInstanceParent->m_normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, pInstanceParent->m_normalBuffer);

	int sizeOfNormals = sizeof(float) * 4 * static_cast<int>(pMesh->vertices.size());
	glBufferData(GL_ARRAY_BUFFER, sizeOfNormals, normals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(inNormal);
	glVertexAttribPointer(inNormal, 4, GL_FLOAT, 0, 0, nullptr);

	if (pInstanceParent->m_colorBuffer != -1)
	{
		glDeleteBuffers(1, &pInstanceParent->m_colorBuffer);
	}

	glGenBuffers(1, &pInstanceParent->m_colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, pInstanceParent->m_colorBuffer);

	int sizeOfColors = sizeof(float) * 4 * static_cast<int>(pMesh->vertices.size());
	glBufferData(GL_ARRAY_BUFFER, sizeOfColors, colors, GL_STATIC_DRAW);
	glEnableVertexAttribArray(inColor);
	glVertexAttribPointer(inColor, 4, GL_FLOAT, 0, 0, nullptr);

	delete[] vertices;
	delete[] normals;
	delete[] colors;
}

// Creation
InstanceParent* InstanceManager::GetInstanceParent(std::string modelName)
{
	for (int instanceParentID = 0; instanceParentID < m_vpInstanceParentList.size(); ++instanceParentID)
	{
		if (m_vpInstanceParentList[instanceParentID]->m_modelName == modelName)
		{
			return m_vpInstanceParentList[instanceParentID];
		}
	}

	return nullptr;
}

InstanceObject* InstanceManager::AddInstanceObject(std::string modelName, glm::vec3 position, glm::vec3 rotation, float instanceScale, Chunk* pOwningChunk, int voxelX, int voxelY, int voxelZ)
{
	InstanceParent* pNewInstanceParent = GetInstanceParent(modelName);

	if (pNewInstanceParent == nullptr)
	{
		// Add new instance parent
		pNewInstanceParent = new InstanceParent();

		pNewInstanceParent->m_modelName = modelName;
		pNewInstanceParent->m_pQubicleBinary = new QubicleBinary(m_pRenderer);
		pNewInstanceParent->m_pQubicleBinary->Import(modelName.c_str(), true);

		SetupGLBuffers(pNewInstanceParent);

		m_vpInstanceParentList.push_back(pNewInstanceParent);
	}

	InstanceObject* pInstanceObject = new InstanceObject();
	pInstanceObject->m_erase = false;
	pInstanceObject->m_render = true;
	pInstanceObject->UpdateMatrix(position, rotation, instanceScale);

	pInstanceObject->m_pOwningChunk = pOwningChunk;
	pInstanceObject->m_voxelX = voxelX;
	pInstanceObject->m_voxelY = voxelY;
	pInstanceObject->m_voxelZ = voxelZ;

	pNewInstanceParent->m_vpInstanceObjectList.push_back(pInstanceObject);

	return pInstanceObject;
}

bool IsInstanceObjectNeedsErasing(InstanceObject* pInstanceObject)
{
	bool isNeedErase = pInstanceObject->m_erase;

	if (isNeedErase == true)
	{
		if (pInstanceObject->m_pOwningChunk != nullptr)
		{
			// TODO: Add functionality back in, when chunks own instances
		}

		delete pInstanceObject;
	}

	return isNeedErase;
}

// Rendering modes
void InstanceManager::SetWireFrameRender(bool wireframe)
{
	m_renderWireFrame = wireframe;
}

// Update
void InstanceManager::Update(float dt)
{
	if (m_checkChunkInstanceTimer > 0.0f)
	{
		m_checkChunkInstanceTimer -= dt;
	}

	for (int instanceParentID = 0; instanceParentID < m_vpInstanceParentList.size(); ++instanceParentID)
	{
		// Check chunk instances, so that instances linked to voxels erase when the voxel is destroyed
		if (m_checkChunkInstanceTimer <= 0.0f)
		{
			for (int instanceObjectID = 0; instanceObjectID < m_vpInstanceParentList[instanceParentID]->m_vpInstanceObjectList.size(); ++instanceObjectID)
			{
				InstanceObject* pInstanceObject = m_vpInstanceParentList[instanceParentID]->m_vpInstanceObjectList[instanceObjectID];

				if (pInstanceObject->m_erase)
				{
					continue;
				}

				if (pInstanceObject->m_pOwningChunk != nullptr)
				{
					bool active = pInstanceObject->m_pOwningChunk->GetActive(pInstanceObject->m_voxelX, pInstanceObject->m_voxelY, pInstanceObject->m_voxelZ);

					if (active == false)
					{
						pInstanceObject->m_erase = true;
					}
				}
			}

			m_checkChunkInstanceTimer = 2.0f;
		}
		else
		{
			m_checkChunkInstanceTimer -= dt;
		}

		m_vpInstanceParentList[instanceParentID]->m_vpInstanceObjectList.erase(remove_if(m_vpInstanceParentList[instanceParentID]->m_vpInstanceObjectList.begin(), m_vpInstanceParentList[instanceParentID]->m_vpInstanceObjectList.end(), IsInstanceObjectNeedsErasing), m_vpInstanceParentList[instanceParentID]->m_vpInstanceObjectList.end());
	}
}

// Rendering
void InstanceManager::Render()
{
	glShader* pShader = m_pRenderer->GetShader(m_instanceShader);

	GLint inModelMatrix = glGetAttribLocation(pShader->GetProgramObject(), "in_model_matrix");

	for (int instanceParentID = 0; instanceParentID < m_vpInstanceParentList.size(); ++instanceParentID)
	{
		TriangleMesh* pMesh = m_vpInstanceParentList[instanceParentID]->m_pQubicleBinary->GetQubicleMatrix(0)->m_pMesh;

		unsigned int numIndices = pMesh->triangles.size() * 3;
		unsigned int numTriangles = pMesh->triangles.size();
		unsigned int* indicesBuffer = new unsigned int[numIndices];

		int indexCounter = 0;

		for (size_t i = 0; i < numTriangles; ++i)
		{
			indicesBuffer[indexCounter] = pMesh->triangles[i]->vertexIndices[0];
			indicesBuffer[indexCounter + 1] = pMesh->triangles[i]->vertexIndices[1];
			indicesBuffer[indexCounter + 2] = pMesh->triangles[i]->vertexIndices[2];

			indexCounter += 3;
		}

		int numInstanceObjectsRender = GetNumInstanceRenderObjectsForParent(instanceParentID);

		if (numInstanceObjectsRender > 0)
		{
			float* newMatrices = new float[16 * numInstanceObjectsRender];

			int counter = 0;

			int instanceObjectRenderCounter = 0;

			for (size_t i = 0; i < m_vpInstanceParentList[instanceParentID]->m_vpInstanceObjectList.size() && instanceObjectRenderCounter < numInstanceObjectsRender; ++i)
			{
				if (m_vpInstanceParentList[instanceParentID]->m_vpInstanceObjectList[i]->m_render == false)
				{
					continue;
				}

				newMatrices[counter + 0] = m_vpInstanceParentList[instanceParentID]->m_vpInstanceObjectList[i]->m_worldMatrix.m_data[0];
				newMatrices[counter + 1] = m_vpInstanceParentList[instanceParentID]->m_vpInstanceObjectList[i]->m_worldMatrix.m_data[1];
				newMatrices[counter + 2] = m_vpInstanceParentList[instanceParentID]->m_vpInstanceObjectList[i]->m_worldMatrix.m_data[2];
				newMatrices[counter + 3] = m_vpInstanceParentList[instanceParentID]->m_vpInstanceObjectList[i]->m_worldMatrix.m_data[3];
				newMatrices[counter + 4] = m_vpInstanceParentList[instanceParentID]->m_vpInstanceObjectList[i]->m_worldMatrix.m_data[4];
				newMatrices[counter + 5] = m_vpInstanceParentList[instanceParentID]->m_vpInstanceObjectList[i]->m_worldMatrix.m_data[5];
				newMatrices[counter + 6] = m_vpInstanceParentList[instanceParentID]->m_vpInstanceObjectList[i]->m_worldMatrix.m_data[6];
				newMatrices[counter + 7] = m_vpInstanceParentList[instanceParentID]->m_vpInstanceObjectList[i]->m_worldMatrix.m_data[7];
				newMatrices[counter + 8] = m_vpInstanceParentList[instanceParentID]->m_vpInstanceObjectList[i]->m_worldMatrix.m_data[8];
				newMatrices[counter + 9] = m_vpInstanceParentList[instanceParentID]->m_vpInstanceObjectList[i]->m_worldMatrix.m_data[9];
				newMatrices[counter + 10] = m_vpInstanceParentList[instanceParentID]->m_vpInstanceObjectList[i]->m_worldMatrix.m_data[10];
				newMatrices[counter + 11] = m_vpInstanceParentList[instanceParentID]->m_vpInstanceObjectList[i]->m_worldMatrix.m_data[11];
				newMatrices[counter + 12] = m_vpInstanceParentList[instanceParentID]->m_vpInstanceObjectList[i]->m_worldMatrix.m_data[12];
				newMatrices[counter + 13] = m_vpInstanceParentList[instanceParentID]->m_vpInstanceObjectList[i]->m_worldMatrix.m_data[13];
				newMatrices[counter + 14] = m_vpInstanceParentList[instanceParentID]->m_vpInstanceObjectList[i]->m_worldMatrix.m_data[14];
				newMatrices[counter + 15] = m_vpInstanceParentList[instanceParentID]->m_vpInstanceObjectList[i]->m_worldMatrix.m_data[15];
				
				counter += 16;

				instanceObjectRenderCounter++;
			}

			glBindVertexArray(m_vpInstanceParentList[instanceParentID]->m_vertexArray);

			if (m_vpInstanceParentList[instanceParentID]->m_matrixBuffer != -1)
			{
				glDeleteBuffers(1, &m_vpInstanceParentList[instanceParentID]->m_matrixBuffer);
			}
			
			// Bind buffer for matrix and copy data into buffer
			glGenBuffers(1, &m_vpInstanceParentList[instanceParentID]->m_matrixBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, m_vpInstanceParentList[instanceParentID]->m_matrixBuffer);
			
			for (int i = 0; i < 4; ++i)
			{
				glVertexAttribPointer(inModelMatrix + i,	// Location
					4, GL_FLOAT, GL_FALSE,					// vec4
					4 * 16,									// Stride
					reinterpret_cast<void *>(16 * i));		// Start offset

				glEnableVertexAttribArray(inModelMatrix + i);
				glVertexAttribDivisor(inModelMatrix + i, 1);
			}
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16 * numInstanceObjectsRender, newMatrices, GL_STATIC_READ);

			delete[] newMatrices;
		}

		// Render the instances
		m_pRenderer->BeginGLSLShader(m_instanceShader);

		GLint projMatrixLoc = glGetUniformLocation(pShader->GetProgramObject(), "projMatrix");
		GLint viewMatrixLoc = glGetUniformLocation(pShader->GetProgramObject(), "viewMatrix");

		Matrix4 projMat;
		Matrix4 viewMat;
		m_pRenderer->GetProjectionMatrix(&projMat);
		m_pRenderer->GetModelViewMatrix(&viewMat);

		glUniformMatrix4fv(projMatrixLoc, 1, false, projMat.m_data);
		glUniformMatrix4fv(viewMatrixLoc, 1, false, viewMat.m_data);

		if (m_renderWireFrame)
		{
			m_pRenderer->SetLineWidth(1.0f);
			m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
			m_pRenderer->SetCullMode(CullMode::NOCULL);
		}
		else
		{
			m_pRenderer->SetCullMode(CullMode::BACK);
			m_pRenderer->SetRenderMode(RenderMode::SOLID);
		}

		m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);

		glDrawElementsInstanced(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, indicesBuffer, numInstanceObjectsRender);

		m_pRenderer->DisableTransparency();

		m_pRenderer->EndGLSLShader(m_instanceShader);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		delete[] indicesBuffer;
	}
}