/*************************************************************************
> File Name: BlockParticleManager.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A manager class for the voxel particle system. This class manages
>    particle effects, particle emitters and the individual particle objects.
> Created Time: 2016/07/12
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <algorithm>

#include <Maths/3DMaths.h>
#include <Utils/Random.h>

#include "BlockParticleManager.h"

float vertices[] = {
	-0.5f, -0.5f, 0.5f, 1.0f, // Front
	0.5f, -0.5f, 0.5f, 1.0f,
	0.5f, 0.5f, 0.5f, 1.0f,
	-0.5f, 0.5f, 0.5f, 1.0f,
	-0.5f, -0.5f, -0.5f, 1.0f, // Back
	-0.5f, 0.5f, -0.5f, 1.0f,
	0.5f, 0.5f, -0.5f, 1.0f,
	0.5f, -0.5f, -0.5f, 1.0f,
	-0.5f, -0.5f, -0.5f, 1.0f, // Left
	-0.5f, -0.5f, 0.5f, 1.0f,
	-0.5f, 0.5f, 0.5f, 1.0f,
	-0.5f, 0.5f, -0.5f, 1.0f,
	0.5f, -0.5f, -0.5f, 1.0f, // Right
	0.5f, 0.5f, -0.5f, 1.0f,
	0.5f, 0.5f, 0.5f, 1.0f,
	0.5f, -0.5f, 0.5f, 1.0f,
	-0.5f, 0.5f, 0.5f, 1.0f, // Top
	0.5f, 0.5f, 0.5f, 1.0f,
	0.5f, 0.5f, -0.5f, 1.0f,
	-0.5f, 0.5f, -0.5f, 1.0f,
	-0.5f, -0.5f, 0.5f, 1.0f, // Bottom
	-0.5f, -0.5f, -0.5f, 1.0f,
	0.5f, -0.5f, -0.5f, 1.0f,
	0.5f, -0.5f, 0.5f, 1.0f
};

float normals[] = {
	0.0f, 0.0f, 1.0f, 1.0f, // Front
	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, -1.0f, 1.0f, // Back
	0.0f, 0.0f, -1.0f, 1.0f,
	0.0f, 0.0f, -1.0f, 1.0f,
	0.0f, 0.0f, -1.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f, // Left
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	-1.0f, 0.0f, 0.0f, 1.0f, // Right
	-1.0f, 0.0f, 0.0f, 1.0f,
	-1.0f, 0.0f, 0.0f, 1.0f,
	-1.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f, // Top
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, -1.0f, 0.0f, 1.0f, // Bottom
	0.0f, -1.0f, 0.0f, 1.0f,
	0.0f, -1.0f, 0.0f, 1.0f,
	0.0f, -1.0f, 0.0f, 1.0f
};

int indices[] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23
};

// Constructor, Destructor
BlockParticleManager::BlockParticleManager(Renderer* pRenderer, ChunkManager* pChunkManager) :
	m_pRenderer(pRenderer), m_pChunkManager(pChunkManager),
	m_particleEffectCounter(0), m_renderWireFrame(false), m_instanceRendering(true),
	m_vertexArray(-1), m_positionBuffer(-1), m_normalBuffer(-1), m_colorBuffer(-1), m_matrixBuffer(-1), m_instanceShader(-1)
{

	bool shaderLoaded = m_pRenderer->LoadGLSLShader("Resources/Shaders/instance.vertex", "Resources/Shaders/instance.pixel", &m_instanceShader);
	assert(shaderLoaded == true);

	// Materials
	m_pRenderer->CreateMaterial(Color(0.7f, 0.7f, 0.7f, 1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 0.0f), 64, &m_blockMaterialID);

	SetupGLBuffers();
}

BlockParticleManager::~BlockParticleManager()
{
	ClearBlockParticles();
	ClearBlockParticleEmitters();
	ClearBlockParticleEffects();
}

// Clearing
void BlockParticleManager::ClearBlockParticles()
{
	for (size_t i = 0; i < m_vpBlockParticlesList.size(); ++i)
	{
		delete m_vpBlockParticlesList[i];
		m_vpBlockParticlesList[i] = nullptr;
	}

	m_vpBlockParticlesList.clear();
}

void BlockParticleManager::ClearBlockParticleEmitters()
{
	for (size_t i = 0; i < m_vpBlockParticleEmittersList.size(); ++i)
	{
		delete m_vpBlockParticleEmittersList[i];
		m_vpBlockParticleEmittersList[i] = nullptr;
	}

	m_vpBlockParticleEmittersList.clear();
}

void BlockParticleManager::ClearBlockParticleEffects()
{
	for (size_t i = 0; i < m_vpBlockParticleEffectsList.size(); ++i)
	{
		delete m_vpBlockParticleEffectsList[i];
		m_vpBlockParticleEffectsList[i] = nullptr;
	}

	m_vpBlockParticleEffectsList.clear();
}

void BlockParticleManager::RemoveEmitterLinkage(BlockParticleEmitter* pEmitter)
{
	for (size_t i = 0; i < m_vpBlockParticlesList.size(); ++i)
	{
		if (m_vpBlockParticlesList[i]->m_pParent == pEmitter)
		{
			m_vpBlockParticlesList[i]->m_pParent = nullptr;
		}
	}
}

void BlockParticleManager::ClearParticleChunkCacheForChunk(Chunk* pChunk)
{
	for (size_t i = 0; i < m_vpBlockParticlesList.size(); ++i)
	{
		m_vpBlockParticlesList[i]->ClearParticleChunkCacheForChunk(pChunk);
	}
}

unsigned int BlockParticleManager::GetInstanceShaderIndex() const
{
	return m_instanceShader;
}

void BlockParticleManager::SetupGLBuffers()
{
	if (m_instanceShader != -1)
	{
		glShader* pShader = m_pRenderer->GetShader(m_instanceShader);

		GLint inPosition = glGetAttribLocation(pShader->GetProgramObject(), "in_position");
		GLint inNormal = glGetAttribLocation(pShader->GetProgramObject(), "in_normal");
		// GLint inColor = glGetAttribLocation(pShader->GetProgramObject(), "in_color");
		// GLint inModelMatrix = glGetAttribLocation(pShader->GetProgramObject(), "in_model_matrix");

		glBindFragDataLocation(pShader->GetProgramObject(), 0, "outputColor");
		glBindFragDataLocation(pShader->GetProgramObject(), 1, "outputPosition");
		glBindFragDataLocation(pShader->GetProgramObject(), 2, "outputNormal");

		if (m_vertexArray != -1)
		{
			glDeleteVertexArrays(1, &m_vertexArray);
		}

		glGenVertexArrays(1, &m_vertexArray);
		glBindVertexArray(m_vertexArray);

		if (m_positionBuffer != -1)
		{
			glDeleteBuffers(1, &m_positionBuffer);
		}

		glGenBuffers(1, &m_positionBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_positionBuffer);

		int sizeOfVertices = sizeof(vertices);
		glBufferData(GL_ARRAY_BUFFER, sizeOfVertices, vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(inPosition);
		glVertexAttribPointer(inPosition, 4, GL_FLOAT, 0, 0, nullptr);

		if (m_normalBuffer != -1)
		{
			glDeleteBuffers(1, &m_normalBuffer);
		}

		glGenBuffers(1, &m_normalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
		
		int sizeOfNormals = sizeof(normals);
		glBufferData(GL_ARRAY_BUFFER, sizeOfNormals, normals, GL_STATIC_DRAW);
		glEnableVertexAttribArray(inNormal);
		glVertexAttribPointer(inNormal, 4, GL_FLOAT, 0, 0, nullptr);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	float length = 0.5f;
	float height = 0.5f;
	float width = 0.5f;

	// Create the cube static buffer
	m_vertexBuffer[0].x = length;	m_vertexBuffer[0].y = -height;	m_vertexBuffer[0].z = -width;
	m_vertexBuffer[0].nx = 0.0f;	m_vertexBuffer[0].ny = 0.0f;	m_vertexBuffer[0].nz = -1.0f;

	m_vertexBuffer[1].x = -length;	m_vertexBuffer[1].y = -height;	m_vertexBuffer[1].z = -width;
	m_vertexBuffer[1].nx = 0.0f;	m_vertexBuffer[1].ny = 0.0f;	m_vertexBuffer[1].nz = -1.0f;

	m_vertexBuffer[2].x = -length;	m_vertexBuffer[2].y = height;	m_vertexBuffer[2].z = -width;
	m_vertexBuffer[2].nx = 0.0f;	m_vertexBuffer[2].ny = 0.0f;	m_vertexBuffer[2].nz = -1.0f;

	m_vertexBuffer[3].x = length;	m_vertexBuffer[3].y = height;	m_vertexBuffer[3].z = -width;
	m_vertexBuffer[3].nx = 0.0f;	m_vertexBuffer[3].ny = 0.0f;	m_vertexBuffer[3].nz = -1.0f;

	m_vertexBuffer[4].x = -length;	m_vertexBuffer[4].y = -height;	m_vertexBuffer[4].z = width;
	m_vertexBuffer[4].nx = 0.0f;	m_vertexBuffer[4].ny = 0.0f;	m_vertexBuffer[4].nz = 1.0f;

	m_vertexBuffer[5].x = length;	m_vertexBuffer[5].y = -height;	m_vertexBuffer[5].z = width;
	m_vertexBuffer[5].nx = 0.0f;	m_vertexBuffer[5].ny = 0.0f;	m_vertexBuffer[5].nz = 1.0f;

	m_vertexBuffer[6].x = length;	m_vertexBuffer[6].y = height;	m_vertexBuffer[6].z = width;
	m_vertexBuffer[6].nx = 0.0f;	m_vertexBuffer[6].ny = 0.0f;	m_vertexBuffer[6].nz = 1.0f;

	m_vertexBuffer[7].x = -length;	m_vertexBuffer[7].y = height;	m_vertexBuffer[7].z = width;
	m_vertexBuffer[7].nx = 0.0f;	m_vertexBuffer[7].ny = 0.0f;	m_vertexBuffer[7].nz = 1.0f;

	m_vertexBuffer[8].x = length;	m_vertexBuffer[8].y = -height;	m_vertexBuffer[8].z = width;
	m_vertexBuffer[8].nx = 1.0f;	m_vertexBuffer[8].ny = 0.0f;	m_vertexBuffer[8].nz = 0.0f;

	m_vertexBuffer[9].x = length;	m_vertexBuffer[9].y = -height;	m_vertexBuffer[9].z = -width;
	m_vertexBuffer[9].nx = 1.0f;	m_vertexBuffer[9].ny = 0.0f;	m_vertexBuffer[9].nz = 0.0f;

	m_vertexBuffer[10].x = length;	m_vertexBuffer[10].y = height;	m_vertexBuffer[10].z = -width;
	m_vertexBuffer[10].nx = 1.0f;	m_vertexBuffer[10].ny = 0.0f;	m_vertexBuffer[10].nz = 0.0f;

	m_vertexBuffer[11].x = length;	m_vertexBuffer[11].y = height;	m_vertexBuffer[11].z = width;
	m_vertexBuffer[11].nx = 1.0f;	m_vertexBuffer[11].ny = 0.0f;	m_vertexBuffer[11].nz = 0.0f;

	m_vertexBuffer[12].x = -length; m_vertexBuffer[12].y = -height; m_vertexBuffer[12].z = -width;
	m_vertexBuffer[12].nx = -1.0f;	m_vertexBuffer[12].ny = 0.0f;	m_vertexBuffer[12].nz = 0.0f;

	m_vertexBuffer[13].x = -length; m_vertexBuffer[13].y = -height; m_vertexBuffer[13].z = width;
	m_vertexBuffer[13].nx = -1.0f;	m_vertexBuffer[13].ny = 0.0f;	m_vertexBuffer[13].nz = 0.0f;

	m_vertexBuffer[14].x = -length; m_vertexBuffer[14].y = height;	m_vertexBuffer[14].z = width;
	m_vertexBuffer[14].nx = -1.0f;	m_vertexBuffer[14].ny = 0.0f;	m_vertexBuffer[14].nz = 0.0f;

	m_vertexBuffer[15].x = -length; m_vertexBuffer[15].y = height;	m_vertexBuffer[15].z = -width;
	m_vertexBuffer[15].nx = -1.0f;	m_vertexBuffer[15].ny = 0.0f;	m_vertexBuffer[15].nz = 0.0f;

	m_vertexBuffer[16].x = -length; m_vertexBuffer[16].y = -height; m_vertexBuffer[16].z = -width;
	m_vertexBuffer[16].nx = 0.0f;	m_vertexBuffer[16].ny = -1.0f;	m_vertexBuffer[16].nz = 0.0f;

	m_vertexBuffer[17].x = length;	m_vertexBuffer[17].y = -height; m_vertexBuffer[17].z = -width;
	m_vertexBuffer[17].nx = 0.0f;	m_vertexBuffer[17].ny = -1.0f;	m_vertexBuffer[17].nz = 0.0f;

	m_vertexBuffer[18].x = length;	m_vertexBuffer[18].y = -height; m_vertexBuffer[18].z = width;
	m_vertexBuffer[18].nx = 0.0f;	m_vertexBuffer[18].ny = -1.0f;	m_vertexBuffer[18].nz = 0.0f;

	m_vertexBuffer[19].x = -length; m_vertexBuffer[19].y = -height; m_vertexBuffer[19].z = width;
	m_vertexBuffer[19].nx = 0.0f;	m_vertexBuffer[19].ny = -1.0f;	m_vertexBuffer[19].nz = 0.0f;

	m_vertexBuffer[20].x = length;	m_vertexBuffer[20].y = height;	m_vertexBuffer[20].z = -width;
	m_vertexBuffer[20].nx = 0.0f;	m_vertexBuffer[20].ny = 1.0f;	m_vertexBuffer[20].nz = 0.0f;

	m_vertexBuffer[21].x = -length; m_vertexBuffer[21].y = height;	m_vertexBuffer[21].z = -width;
	m_vertexBuffer[21].nx = 0.0f;	m_vertexBuffer[21].ny = 1.0f;	m_vertexBuffer[21].nz = 0.0f;

	m_vertexBuffer[22].x = -length; m_vertexBuffer[22].y = height;	m_vertexBuffer[22].z = width;
	m_vertexBuffer[22].nx = 0.0f;	m_vertexBuffer[22].ny = 1.0f;	m_vertexBuffer[22].nz = 0.0f;

	m_vertexBuffer[23].x = length;	m_vertexBuffer[23].y = height;	m_vertexBuffer[23].z = width;
	m_vertexBuffer[23].nx = 0.0f;	m_vertexBuffer[23].ny = 1.0f;	m_vertexBuffer[23].nz = 0.0f;

	for (int i = 0; i < 24; ++i)
	{
		m_vertexBuffer[i].r = 1.0f;
		m_vertexBuffer[i].g = 1.0f;
		m_vertexBuffer[i].b = 1.0f;
		m_vertexBuffer[i].a = 1.0f;
	}
}

// Accessors
int BlockParticleManager::GetNumBlockParticleEffects() const
{
	int numEffects = m_vpBlockParticleEffectsList.size();

	return numEffects;
}

int BlockParticleManager::GetNumBlockParticleEmitters() const
{
	int numEmitters = m_vpBlockParticleEmittersList.size();

	return numEmitters;
}

int BlockParticleManager::GetNumBlockParticles() const
{
	int numParticles = m_vpBlockParticlesList.size();

	return numParticles;
}

int BlockParticleManager::GetNumRenderableParticles(bool noWorldOffset)
{
	int numparticlesToRender = 0;
	for (int i = 0; i < m_vpBlockParticlesList.size(); ++i)
	{
		// If we are a emitter creation particle, don't render.
		if (m_vpBlockParticlesList[i]->m_isCreateEmitters == true)
		{
			continue;
		}

		// If we are to be erased, don't render
		if (m_vpBlockParticlesList[i]->m_isErase == true)
		{
			continue;
		}

		// If we are rendering the special viewport particles and our parent particle effect viewport flag isn't set, don't render.
		if (noWorldOffset)
		{
			if (m_vpBlockParticlesList[i]->m_pParent == nullptr ||
				m_vpBlockParticlesList[i]->m_pParent->m_pParent == nullptr ||
				m_vpBlockParticlesList[i]->m_pParent->m_pParent->m_isRenderNoWoldOffsetViewport == false)
			{
				continue;
			}
		}

		numparticlesToRender++;
	}

	return numparticlesToRender;
}

// Creation
BlockParticle* BlockParticleManager::CreateBlockParticleFromEmitterParams(BlockParticleEmitter* pEmitter)
{
	std::random_device rd;
	std::mt19937 mtRand(rd());

	glm::vec3 posToSpawn = pEmitter->m_position;

	glm::vec3 posOffset;

	if (pEmitter->m_emitterType == EmitterType::Point)
	{
		// No position offset since we are emitting from a point
	}
	else if (pEmitter->m_emitterType == EmitterType::Square)
	{
		if (pEmitter->m_isSpawnOutline)
		{
			float randPoint = static_cast<float>(mtRand() % 100) / 100.0f;
			int randSide = mtRand() % 4;

			glm::vec3 squarePosition;
			float halfLength = pEmitter->m_emitterLengthX;
			float halfWidth = pEmitter->m_emitterLengthZ;
			
			if (randSide == 0)
			{
				squarePosition = glm::vec3(pEmitter->m_emitterLengthX * 2.0f * randPoint - halfLength, 0.0f, -halfWidth);
			}
			else if (randSide == 1)
			{
				squarePosition = glm::vec3(pEmitter->m_emitterLengthX * 2.0f * randPoint - halfLength, 0.0f, halfWidth);
			}
			else if (randSide == 2)
			{
				squarePosition = glm::vec3(-halfLength, 0.0f, pEmitter->m_emitterLengthZ * 2.0f * randPoint - halfWidth);
			}
			else if (randSide == 3)
			{
				squarePosition = glm::vec3(halfLength, 0.0f, pEmitter->m_emitterLengthZ * 2.0f * randPoint - halfWidth);
			}

			posOffset = squarePosition;
		}
		else
		{
			posOffset = glm::vec3(GetRandomNumber(-1, 1, 2) * pEmitter->m_emitterLengthX, 0.0f, GetRandomNumber(-1, 1, 2) * pEmitter->m_emitterLengthZ);
		}
	}
	else if (pEmitter->m_emitterType == EmitterType::Cube)
	{
		// Get a random point around the cube

		float randPoint = static_cast<float>(mtRand() % 100) / 100.0f;
		int randEdge = mtRand() % 12;

		if (pEmitter->m_isSpawnOutline)
		{
			glm::vec3 cubePosition;
			float halfLength = pEmitter->m_emitterLengthX;
			float halfHeight = pEmitter->m_emitterLengthY;
			float halfWidth = pEmitter->m_emitterLengthZ;

			if (randEdge == 0)
			{
				cubePosition = glm::vec3(pEmitter->m_emitterLengthX * 2.0f * randPoint - halfLength, -halfHeight, -halfWidth);
			}
			else if (randEdge == 1)
			{
				cubePosition = glm::vec3(pEmitter->m_emitterLengthX * 2.0f * randPoint - halfLength, -halfHeight, halfWidth);
			}
			else if (randEdge == 2)
			{
				cubePosition = glm::vec3(-halfLength, -halfHeight, pEmitter->m_emitterLengthZ * 2.0f * randPoint - halfWidth);
			}
			else if (randEdge == 3)
			{
				cubePosition = glm::vec3(halfLength, -halfHeight, pEmitter->m_emitterLengthZ * 2.0f * randPoint - halfWidth);
			}
			else if (randEdge == 4)
			{
				cubePosition = glm::vec3(pEmitter->m_emitterLengthX * 2.0f * randPoint - halfLength, halfHeight, -halfWidth);
			}
			else if (randEdge == 5)
			{
				cubePosition = glm::vec3(pEmitter->m_emitterLengthX * 2.0f * randPoint - halfLength, halfHeight, halfWidth);
			}
			else if (randEdge == 6)
			{
				cubePosition = glm::vec3(-halfLength, halfHeight, pEmitter->m_emitterLengthZ * 2.0f * randPoint - halfWidth);
			}
			else if (randEdge == 7)
			{
				cubePosition = glm::vec3(halfLength, halfHeight, pEmitter->m_emitterLengthZ * 2.0f * randPoint - halfWidth);
			}
			else if (randEdge == 8)
			{
				cubePosition = glm::vec3(-halfLength, pEmitter->m_emitterLengthY * 2.0f * randPoint - halfHeight, -halfWidth);
			}
			else if (randEdge == 9)
			{
				cubePosition = glm::vec3(-halfLength, pEmitter->m_emitterLengthY * 2.0f * randPoint - halfHeight, halfWidth);
			}
			else if (randEdge == 10)
			{
				cubePosition = glm::vec3(halfLength, pEmitter->m_emitterLengthY * 2.0f * randPoint - halfHeight, -halfWidth);
			}
			else if (randEdge == 11)
			{
				cubePosition = glm::vec3(halfLength, pEmitter->m_emitterLengthY * 2.0f * randPoint - halfHeight, halfWidth);
			}

			posOffset = cubePosition;
		}
		else
		{
			posOffset = glm::vec3(GetRandomNumber(-1, 1, 2) * pEmitter->m_emitterLengthX, GetRandomNumber(-1, 1, 2) * pEmitter->m_emitterLengthY, GetRandomNumber(-1, 1, 2) * pEmitter->m_emitterLengthZ);
		}
	}
	else if (pEmitter->m_emitterType == EmitterType::Circle)
	{
		// Get a random point int the circle
		float randPoint = static_cast<float>(mtRand() % 100) / 100.0f;
		float angle = DegreeToRadian(360 * randPoint);

		if (pEmitter->m_isSpawnOutline)
		{
			posOffset = glm::vec3(cos(angle) * pEmitter->m_emitterRadius, 0.0f, sin(angle) * pEmitter->m_emitterRadius);
		}
		else
		{
			float randDistance = pEmitter->m_emitterRadius * (static_cast<float>(mtRand() % 100) / 100.0f);
			posOffset = glm::vec3(cos(angle) * randDistance, 0.0f, sin(angle) * randDistance);
		}
	}
	else if (pEmitter->m_emitterType == EmitterType::Sphere)
	{
		// Get a random point around the sphere
		float z = 2.0f * static_cast<float>((mtRand() % 1000) / 1000.0f) - 1.0f;
		float t = 2.0f * PI * static_cast<float>((mtRand() % 1000) / 1000.0f);
		float w = sqrt(1.0f - z * z);
		float x = w * cos(t);
		float y = w * sin(t);
		glm::vec3 outlinePoint = glm::vec3(x, y, z);

		if (pEmitter->m_isSpawnOutline)
		{
			posOffset = outlinePoint * pEmitter->m_emitterRadius;
		}
		else
		{
			float randomDist = GetRandomNumber(0, 1, 2);
			posOffset = outlinePoint * pEmitter->m_emitterRadius * randomDist;
		}
	}
	else if (pEmitter->m_emitterType == EmitterType::Mesh)
	{
		// Do nothing
	}

	posToSpawn += posOffset;

	glm::vec3 posToSpawnNoWorldOffset = posToSpawn;

	if (pEmitter->m_shouldParticlesFollowEmitter)
	{
		posToSpawn = glm::vec3(0.0f, 0.0f, 0.0f);
		posToSpawnNoWorldOffset = glm::vec3(0.0f, 0.0f, 0.0f);
	}
	else if (pEmitter->m_pParent != nullptr && pEmitter->m_pParentParticle == nullptr)
	{
		// If our emitter's parent effect has a position offset
		posToSpawn += pEmitter->m_pParent->m_position;
		posToSpawnNoWorldOffset += pEmitter->m_pParent->m_positionNoWorldOffset;
	}

	// Get the create emitter
	BlockParticleEmitter* pCreatedEmitter = nullptr;

	if (pEmitter->m_isCreateEmitters)
	{
		BlockParticleEmitter* pCreateEmitterParam = pEmitter->m_pParent->GetEmitter(pEmitter->m_createEmitterName);

		pCreatedEmitter = CreateBlockParticleEmitter("CreatedEmitter", glm::vec3(0.0f, 0.0f, 0.0f));
		pCreatedEmitter->CopyParams(pCreateEmitterParam);
	}

	BlockParticle* pBlockParticle = CreateBlockParticle(posToSpawn, posToSpawnNoWorldOffset, pEmitter->m_gravityDirection, pEmitter->m_gravityMultiplier, pEmitter->m_pointOrigin,
		pEmitter->m_startScale, pEmitter->m_startScaleVariance, pEmitter->m_endScale, pEmitter->m_endScaleVariance,
		pEmitter->m_startRed, pEmitter->m_startGreen, pEmitter->m_startBlue, pEmitter->m_startAlpha,
		pEmitter->m_startRedVariance, pEmitter->m_startGreenVariance, pEmitter->m_startBlueVariance, pEmitter->m_startAlphaVariance,
		pEmitter->m_endRed, pEmitter->m_endGreen, pEmitter->m_endBlue, pEmitter->m_endAlpha,
		pEmitter->m_endRedVariance, pEmitter->m_endGreenVariance, pEmitter->m_endBlueVariance, pEmitter->m_endAlphaVariance,
		pEmitter->m_lifeTime, pEmitter->m_lifeTimeVariance,
		pEmitter->m_velocityTowardsPoint, pEmitter->m_accelerationTowardsPoint,
		pEmitter->m_startVelocity, pEmitter->m_startVelocityVariance,
		pEmitter->m_startAngularVelocity, pEmitter->m_startAngularVelocityVariance,
		pEmitter->m_tangentialVelocityXY, pEmitter->m_tangentialAccelerationXY, pEmitter->m_tangentialVelocityXZ, pEmitter->m_tangentialAccelerationXZ, pEmitter->m_tangentialVelocityYZ, pEmitter->m_tangentialAccelerationYZ,
		pEmitter->m_isRandomStartRotation, pEmitter->m_startRotation, pEmitter->m_isCheckWorldCollisions, pEmitter->m_isDestoryOnCollision, pEmitter->m_isStartLifeDecayOnCollision,
		pEmitter->m_isCreateEmitters, pCreatedEmitter);

	if (pBlockParticle != nullptr)
	{
		// Set parent to emitter
		pBlockParticle->m_pParent = pEmitter;
	}

	return pBlockParticle;
}

BlockParticle* BlockParticleManager::CreateBlockParticle(glm::vec3 pos, glm::vec3 posNoWorldOffset, glm::vec3 gravityDir, float gravityMultiplier, glm::vec3 pointOrigin,
	float startScale, float startScaleVariance, float endScale, float endScaleVariance,
	float startR, float startG, float startB, float startA,
	float startRVariance, float startGVariance, float startBVariance, float startAVariance,
	float endR, float endG, float endB, float endA,
	float endRVariance, float endGVariance, float endBVariance, float endAVariance,
	float lifetime, float lifetimeVariance,
	float velocityTowardPoint, float accelerationTowardsPoint,
	glm::vec3 startVelocity, glm::vec3 startVelocityVariance,
	glm::vec3 startAngularVelocity, glm::vec3 startAngularVelocityVariance,
	float tangentialVelocityXY, float tangentialAccelerationXY, float tangentialVelocityXZ, float tangentialAccelerationXZ, float tangentialVelocityYZ, float tangentialAccelerationYZ,
	bool randomStartRotation, glm::vec3 startRotation, bool worldCollision, bool destoryOnCollision, bool startLifeDecayOnCollision,
	bool createEmitters, BlockParticleEmitter* pCreatedEmitter)
{
	BlockParticle* pBlockParticle = new BlockParticle();
	pBlockParticle->m_pChunkManager = m_pChunkManager;

	pBlockParticle->m_position = pos;
	pBlockParticle->m_positionNoWorldOffset = posNoWorldOffset;
	pBlockParticle->m_gravityDirection = gravityDir;
	pBlockParticle->m_gravityMultiplier = gravityMultiplier;

	pBlockParticle->m_startScale = startScale;
	pBlockParticle->m_startScaleVariance = startScaleVariance;
	pBlockParticle->m_endScale = endScale;
	pBlockParticle->m_endScaleVariance = endScaleVariance;

	pBlockParticle->m_startRed = startR;
	pBlockParticle->m_startRedVariance = startRVariance;
	pBlockParticle->m_startGreen = startG;
	pBlockParticle->m_startGreenVariance = startGVariance;
	pBlockParticle->m_startBlue = startB;
	pBlockParticle->m_startBlueVariance = startBVariance;
	pBlockParticle->m_startAlpha = startA;
	pBlockParticle->m_startAlphaVariance = startAVariance;
	pBlockParticle->m_endRed = endR;
	pBlockParticle->m_endRedVariance = endRVariance;
	pBlockParticle->m_endGreen = endG;
	pBlockParticle->m_endGreenVariance = endGVariance;
	pBlockParticle->m_endBlue = endB;
	pBlockParticle->m_endBlueVariance = endBVariance;
	pBlockParticle->m_endAlpha = endA;
	pBlockParticle->m_endAlphaVariance = endAVariance;

	pBlockParticle->m_pointOrigin = pointOrigin;
	pBlockParticle->m_velocityTowardsPoint = velocityTowardPoint;
	pBlockParticle->m_accelerationTowardsPoint = accelerationTowardsPoint;

	pBlockParticle->m_tangentialVelocityXY = tangentialVelocityXY;
	pBlockParticle->m_tangentialAccelerationXY = tangentialAccelerationXY;
	pBlockParticle->m_tangentialVelocityXZ = tangentialVelocityXZ;
	pBlockParticle->m_tangentialAccelerationXZ = tangentialAccelerationXZ;
	pBlockParticle->m_tangentialVelocityYZ = tangentialVelocityYZ;
	pBlockParticle->m_tangentialAccelerationYZ = tangentialAccelerationYZ;

	pBlockParticle->m_velocity = startVelocity;
	pBlockParticle->m_startVelocityVariance = startVelocityVariance;

	pBlockParticle->m_angularVelocity = startAngularVelocity;
	pBlockParticle->m_startAngularVelocityVariance = startAngularVelocityVariance;

	pBlockParticle->m_lifeTime = lifetime;
	pBlockParticle->m_maxLifeTime = lifetime;
	pBlockParticle->m_lifeTimeVariance = lifetimeVariance;

	pBlockParticle->m_isCheckWorldCollisions = worldCollision;
	pBlockParticle->m_isDestoryOnCollision = destoryOnCollision;
	pBlockParticle->m_isStartLifeDecayOnCollision = startLifeDecayOnCollision;

	pBlockParticle->m_startRotation = startRotation;
	pBlockParticle->m_isRandomStartRotation = randomStartRotation;

	pBlockParticle->m_isCreateEmitters = createEmitters;
	pBlockParticle->m_pCreatedEmitter = pCreatedEmitter;

	pBlockParticle->m_isErase = false;

	pBlockParticle->CreateStartingParams();

	m_vpBlockParticlesList.push_back(pBlockParticle);

	return pBlockParticle;
}

BlockParticleEmitter* BlockParticleManager::CreateBlockParticleEmitter(std::string name, glm::vec3 pos)
{
	BlockParticleEmitter* pBlockParticleEmitter = new BlockParticleEmitter(m_pRenderer, this);

	pBlockParticleEmitter->m_emitterName = name;
	pBlockParticleEmitter->m_startingPosition = pos;
	pBlockParticleEmitter->m_position = pos;

	pBlockParticleEmitter->m_isErase = false;

	m_vpBlockParticleEmittersAddList.push_back(pBlockParticleEmitter);

	return pBlockParticleEmitter;
}

BlockParticleEffect* BlockParticleManager::ImportParticleEffect(std::string fileName, glm::vec3 pos, unsigned int* particleEffectId)
{
	BlockParticleEffect* pBlockParticleEffect = new BlockParticleEffect(m_pRenderer, this);

	pBlockParticleEffect->Import(fileName.c_str());

	pBlockParticleEffect->m_effectName = fileName;
	pBlockParticleEffect->m_position = pos;

	pBlockParticleEffect->m_particleEffectID = m_particleEffectCounter;
	*particleEffectId = m_particleEffectCounter;
	m_particleEffectCounter++;

	pBlockParticleEffect->m_isErase = false;

	m_vpBlockParticleEffectsList.push_back(pBlockParticleEffect);

	pBlockParticleEffect->PlayEffect();

	return pBlockParticleEffect;
}

void BlockParticleManager::DestroyParticleEffect(unsigned int particleEffectID)
{
	for (size_t i = 0; i < m_vpBlockParticleEffectsList.size(); ++i)
	{
		if (m_vpBlockParticleEffectsList[i]->m_particleEffectID == particleEffectID)
		{
			m_vpBlockParticleEffectsList[i]->ClearEmitters();
			m_vpBlockParticleEffectsList[i]->m_isErase = true;
		}
	}
}

void BlockParticleManager::UpdateParticleEffectPosition(unsigned int particleEffectID, glm::vec3 position, glm::vec3 positionNoWorldOffset)
{
	for (size_t i = 0; i < m_vpBlockParticleEffectsList.size(); ++i)
	{
		if (m_vpBlockParticleEffectsList[i]->m_particleEffectID == particleEffectID)
		{
			m_vpBlockParticleEffectsList[i]->m_position = position;
			m_vpBlockParticleEffectsList[i]->m_positionNoWorldOffset = positionNoWorldOffset;

			return;
		}
	}
}

void BlockParticleManager::SetRenderNoWoldOffsetViewport(unsigned int particleEffectID, bool renderNoWoldOffsetViewport)
{
	for (size_t i = 0; i < m_vpBlockParticleEffectsList.size(); ++i)
	{
		if (m_vpBlockParticleEffectsList[i]->m_particleEffectID == particleEffectID)
		{
			m_vpBlockParticleEffectsList[i]->m_isRenderNoWoldOffsetViewport = renderNoWoldOffsetViewport;

			return;
		}
	}
}

void BlockParticleManager::ExplodeQubicleBinary(QubicleBinary* pQubicleBinary, float scale, int particleSpawnChance)
{
	if (pQubicleBinary != nullptr)
	{
		for (int i = 0; i < pQubicleBinary->GetNumMatrices(); ++i)
		{
			QubicleMatrix* pMatrix = pQubicleBinary->GetQubicleMatrix(i);

			float r = 1.0f;
			float g = 1.0f;
			float b = 1.0f;
			float a = 1.0f;
			bool singleMeshColor = pQubicleBinary->GetSingleMeshColor(&r, &g, &b, &a);
			
			ExplodeQubicleMatrix(pMatrix, scale, particleSpawnChance, singleMeshColor, r, g, b, a);
		}
	}
}

void BlockParticleManager::ExplodeQubicleMatrix(QubicleMatrix* pMatrix, float scale, int particleSpawnChance, bool allSameColor, float sameR, float sameG, float sameB, float sameA)
{
	if (pMatrix != nullptr)
	{
		float r = 1.0f;
		float g = 1.0f;
		float b = 1.0f;
		float a = 1.0f;

		for (size_t x = 0; x < pMatrix->m_matrixSizeX; ++x)
		{
			for (size_t y = 0; y < pMatrix->m_matrixSizeY; ++y)
			{
				for (size_t z = 0; z < pMatrix->m_matrixSizeZ; ++z)
				{
					if (pMatrix->GetActive(x, y, z) == true)
					{
						if (GetRandomNumber(0, 100) > particleSpawnChance)
						{
							// Don't always spawn all blocks.
							continue;
						}

						if (allSameColor)
						{
							r = sameR;
							g = sameG;
							b = sameB;
							a = sameA;
						}
						else
						{
							pMatrix->GetColor(x, y, z, &r, &g, &b, &a);
						}

						a = 1.0f;

						glm::vec3 blockPosition = glm::vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));

						float startScale = scale;
						float endScale = scale;
						startScale *= GetRandomNumber(90, 100, 2) * 0.01f;
						endScale *= GetRandomNumber(25, 75, 2) * 0.01f;

						glm::vec3 newBlockPosition;
						Matrix4::Multiply(pMatrix->m_modelMatrix, blockPosition, newBlockPosition);
						blockPosition = newBlockPosition;

						float rotX;
						float rotY;
						float rotZ;
						pMatrix->m_modelMatrix.GetEuler(&rotX, &rotY, &rotZ);

						glm::vec3 center = glm::vec3(static_cast<float>(pMatrix->m_matrixSizeX) * 0.5f, static_cast<float>(pMatrix->m_matrixSizeY) * 0.5f, static_cast<float>(pMatrix->m_matrixSizeZ) * 0.5f);

						glm::vec3 newCenter;
						Matrix4::Multiply(pMatrix->m_modelMatrix, center, newCenter);
						center = newCenter;

						glm::vec3 toOrigin = normalize(center - blockPosition);

						float lifeTime = 4.0f + GetRandomNumber(-100, 200, 1) * 0.0075f;

						glm::vec3 gravity = glm::vec3(0.0f, -1.0f, 0.0f);
						glm::vec3 pointOrigin = glm::vec3(0.0f, 0.0f, 0.0f);
						BlockParticle* pParticle = CreateBlockParticle(blockPosition, blockPosition, gravity, 1.5f, pointOrigin, startScale, 0.0f, endScale, 0.0f, r, g, b, a, 0.0f, 0.0f, 0.0f, 0.0f, r, g, b, a, 0.0f, 0.0f, 0.0f, 0.0f, lifeTime, 0.0f, 0.0f, 0.0f, -toOrigin + glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.85f, 2.0f, 0.85f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(180.0f, 180.0f, 180.0f), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false, glm::vec3(rotX, rotY, rotZ), true, false, false, false, nullptr);
						
						if (pParticle != nullptr)
						{
							pParticle->m_isAllowFloorSliding = false;
						}
					}
				}
			}
		}
	}
}

bool isNeedErasingBlockParticleEffect(BlockParticleEffect* effect)
{
	bool isNeedErase = effect->m_isErase;

	if (isNeedErase == true)
	{
		delete effect;
	}

	return isNeedErase;
}

bool isNeedErasingBlockParticleEmitter(BlockParticleEmitter* emitter)
{
	bool isNeedErase = emitter->m_isErase;

	if (isNeedErase == true)
	{
		delete emitter;
	}

	return isNeedErase;
}

bool isNeedErasingBlockParticle(BlockParticle* particle)
{
	bool isNeedErase = particle->m_isErase;

	if (isNeedErase == true)
	{
		delete particle;
	}

	return isNeedErase;
}

// Rendering modes
void BlockParticleManager::SetWireFrameRender(bool wireframe)
{
	m_renderWireFrame = wireframe;
}

void BlockParticleManager::SetInstancedRendering(bool instance)
{
	m_instanceRendering = instance;
}

// Update
void BlockParticleManager::Update(float dt)
{
	// Update block particle emitters
	m_vpBlockParticleEmittersList.erase(remove_if(m_vpBlockParticleEmittersList.begin(), m_vpBlockParticleEmittersList.end(), isNeedErasingBlockParticleEmitter), m_vpBlockParticleEmittersList.end());

	// Add any new emitters
	for (size_t i = 0; i < m_vpBlockParticleEmittersAddList.size(); ++i)
	{
		BlockParticleEmitter* pAddedBlockParticleEmitter = m_vpBlockParticleEmittersAddList[i];

		m_vpBlockParticleEmittersList.push_back(pAddedBlockParticleEmitter);
	}

	m_vpBlockParticleEmittersAddList.clear();

	for (size_t i = 0; i < m_vpBlockParticleEmittersList.size(); ++i)
	{
		BlockParticleEmitter* pBlockParticleEmitter = m_vpBlockParticleEmittersList[i];

		pBlockParticleEmitter->Update(dt);
	}

	// Update particle effects
	m_vpBlockParticleEffectsList.erase(remove_if(m_vpBlockParticleEffectsList.begin(), m_vpBlockParticleEffectsList.end(), isNeedErasingBlockParticleEffect), m_vpBlockParticleEffectsList.end());

	for (size_t i = 0; i < m_vpBlockParticleEffectsList.size(); ++i)
	{
		BlockParticleEffect* pBlockParticleEffect = m_vpBlockParticleEffectsList[i];

		pBlockParticleEffect->Update(dt);
	}

	// Update block particles
	for (size_t i = 0; i < m_vpBlockParticlesList.size(); ++i)
	{
		BlockParticle* pBlockParticle = m_vpBlockParticlesList[i];

		if (pBlockParticle->m_lifeTime <= 0.0f)
		{
			if (pBlockParticle->m_waitAfterUpdateCompleteTimer <= 0.0f)
			{
				// If we have elapsed our timer, then erase the effect and continue
				pBlockParticle->m_isErase = true;

				continue;
			}
		}

		pBlockParticle->Update(dt);
	}

	m_vpBlockParticlesList.erase(remove_if(m_vpBlockParticlesList.begin(), m_vpBlockParticlesList.end(), isNeedErasingBlockParticle), m_vpBlockParticlesList.end());
}

// Rendering
void BlockParticleManager::Render(bool noWorldOffset)
{
	if (m_instanceRendering && m_instanceShader != -1)
	{
		RenderInstanced(noWorldOffset);
	}
	else
	{
		RenderDefault(noWorldOffset);
	}
}

void BlockParticleManager::RenderInstanced(bool noWorldOffset)
{
	glShader* pShader = m_pRenderer->GetShader(m_instanceShader);

	// GLint inPosition = glGetAttribLocation(pShader->GetProgramObject(), "in_position");
	GLint inColor = glGetAttribLocation(pShader->GetProgramObject(), "in_color");
	GLint inModelMatrix = glGetAttribLocation(pShader->GetProgramObject(), "in_model_matrix");

	int numBlockParticles = m_vpBlockParticlesList.size();
	int numBlockParticlesRender = GetNumRenderableParticles(noWorldOffset);
	
	if (numBlockParticlesRender > 0)
	{
		float* newMatrices = new float[16 * numBlockParticlesRender];
		float* newColors = new float[4 * numBlockParticlesRender];

		int counter = 0;

		for (int i = 0; i < numBlockParticles; ++i)
		{
			// If we are a emitter creation particle, don't render.
			if (m_vpBlockParticlesList[i]->m_isCreateEmitters == true)
			{
				continue;
			}

			// If we are to be erased, don't render
			if (m_vpBlockParticlesList[i]->m_isErase == true)
			{
				continue;
			}

			// If we are rendering the special viewport particles and our parent particle effect viewport flag isn't set, don't render.
			if (noWorldOffset)
			{
				if (m_vpBlockParticlesList[i]->m_pParent == nullptr ||
					m_vpBlockParticlesList[i]->m_pParent->m_pParent == nullptr ||
					m_vpBlockParticlesList[i]->m_pParent->m_pParent->m_isRenderNoWoldOffsetViewport == false)
				{
					continue;
				}
			}

			newColors[counter + 0] = m_vpBlockParticlesList[i]->m_currentRed;
			newColors[counter + 1] = m_vpBlockParticlesList[i]->m_currentGreen;
			newColors[counter + 2] = m_vpBlockParticlesList[i]->m_currentBlue;
			newColors[counter + 3] = m_vpBlockParticlesList[i]->m_currentAlpha;
			counter += 4;
		}

		counter = 0;

		for (int i = 0; i < numBlockParticles; ++i)
		{
			// If we are a emitter creation particle, don't render.
			if (m_vpBlockParticlesList[i]->m_isCreateEmitters == true)
			{
				continue;
			}

			// If we are to be erased, don't render
			if (m_vpBlockParticlesList[i]->m_isErase == true)
			{
				continue;
			}

			// If we are rendering the special viewport particles and our parent particle effect viewport flag isn't set, don't render.
			if (noWorldOffset)
			{
				if (m_vpBlockParticlesList[i]->m_pParent == nullptr ||
					m_vpBlockParticlesList[i]->m_pParent->m_pParent == nullptr ||
					m_vpBlockParticlesList[i]->m_pParent->m_pParent->m_isRenderNoWoldOffsetViewport == false)
				{
					continue;
				}
			}

			m_vpBlockParticlesList[i]->CalculateWorldTransformMatrix();

			if (noWorldOffset)
			{
				newMatrices[counter + 0] = m_vpBlockParticlesList[i]->m_worldMatrixNoPositionOffset.m_data[0];
				newMatrices[counter + 1] = m_vpBlockParticlesList[i]->m_worldMatrixNoPositionOffset.m_data[1];
				newMatrices[counter + 2] = m_vpBlockParticlesList[i]->m_worldMatrixNoPositionOffset.m_data[2];
				newMatrices[counter + 3] = m_vpBlockParticlesList[i]->m_worldMatrixNoPositionOffset.m_data[3];
				newMatrices[counter + 4] = m_vpBlockParticlesList[i]->m_worldMatrixNoPositionOffset.m_data[4];
				newMatrices[counter + 5] = m_vpBlockParticlesList[i]->m_worldMatrixNoPositionOffset.m_data[5];
				newMatrices[counter + 6] = m_vpBlockParticlesList[i]->m_worldMatrixNoPositionOffset.m_data[6];
				newMatrices[counter + 7] = m_vpBlockParticlesList[i]->m_worldMatrixNoPositionOffset.m_data[7];
				newMatrices[counter + 8] = m_vpBlockParticlesList[i]->m_worldMatrixNoPositionOffset.m_data[8];
				newMatrices[counter + 9] = m_vpBlockParticlesList[i]->m_worldMatrixNoPositionOffset.m_data[9];
				newMatrices[counter + 10] = m_vpBlockParticlesList[i]->m_worldMatrixNoPositionOffset.m_data[10];
				newMatrices[counter + 11] = m_vpBlockParticlesList[i]->m_worldMatrixNoPositionOffset.m_data[11];
				newMatrices[counter + 12] = m_vpBlockParticlesList[i]->m_worldMatrixNoPositionOffset.m_data[12];
				newMatrices[counter + 13] = m_vpBlockParticlesList[i]->m_worldMatrixNoPositionOffset.m_data[13];
				newMatrices[counter + 14] = m_vpBlockParticlesList[i]->m_worldMatrixNoPositionOffset.m_data[14];
				newMatrices[counter + 15] = m_vpBlockParticlesList[i]->m_worldMatrixNoPositionOffset.m_data[15];
			}
			else
			{
				newMatrices[counter + 0] = m_vpBlockParticlesList[i]->m_worldMatrix.m_data[0];
				newMatrices[counter + 1] = m_vpBlockParticlesList[i]->m_worldMatrix.m_data[1];
				newMatrices[counter + 2] = m_vpBlockParticlesList[i]->m_worldMatrix.m_data[2];
				newMatrices[counter + 3] = m_vpBlockParticlesList[i]->m_worldMatrix.m_data[3];
				newMatrices[counter + 4] = m_vpBlockParticlesList[i]->m_worldMatrix.m_data[4];
				newMatrices[counter + 5] = m_vpBlockParticlesList[i]->m_worldMatrix.m_data[5];
				newMatrices[counter + 6] = m_vpBlockParticlesList[i]->m_worldMatrix.m_data[6];
				newMatrices[counter + 7] = m_vpBlockParticlesList[i]->m_worldMatrix.m_data[7];
				newMatrices[counter + 8] = m_vpBlockParticlesList[i]->m_worldMatrix.m_data[8];
				newMatrices[counter + 9] = m_vpBlockParticlesList[i]->m_worldMatrix.m_data[9];
				newMatrices[counter + 10] = m_vpBlockParticlesList[i]->m_worldMatrix.m_data[10];
				newMatrices[counter + 11] = m_vpBlockParticlesList[i]->m_worldMatrix.m_data[11];
				newMatrices[counter + 12] = m_vpBlockParticlesList[i]->m_worldMatrix.m_data[12];
				newMatrices[counter + 13] = m_vpBlockParticlesList[i]->m_worldMatrix.m_data[13];
				newMatrices[counter + 14] = m_vpBlockParticlesList[i]->m_worldMatrix.m_data[14];
				newMatrices[counter + 15] = m_vpBlockParticlesList[i]->m_worldMatrix.m_data[15];
			}
			counter += 16;
		}

		glBindVertexArray(m_vertexArray);

		if (m_colorBuffer != -1)
		{
			glDeleteBuffers(1, &m_colorBuffer);
		}

		// Bind buffer for colors and copy data into buffer
		glGenBuffers(1, &m_colorBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * numBlockParticlesRender, newColors, GL_STATIC_READ);
		glEnableVertexAttribArray(inColor);
		glVertexAttribPointer(inColor, 4, GL_FLOAT, GL_FALSE, 4 * 4, nullptr);
		glVertexAttribDivisor(inColor, 1);

		if (m_matrixBuffer != -1)
		{
			glDeleteBuffers(1, &m_matrixBuffer);
		}

		// Bind buffer for matrix and copy data into buffer
		glGenBuffers(1, &m_matrixBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_matrixBuffer);

		for (int i = 0; i < 4; ++i)
		{
			glVertexAttribPointer(inModelMatrix + i,	// Location
				4, GL_FLOAT, GL_FALSE,					// vec4
				4 * 16,									// Stride
				reinterpret_cast<void *>(16 * i));		// Start offset

			glEnableVertexAttribArray(inModelMatrix + i);
			glVertexAttribDivisor(inModelMatrix + i, 1);
		}

		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16 * numBlockParticlesRender, newMatrices, GL_STATIC_READ);

		delete[] newColors;
		delete[] newMatrices;
	}

	// Render the block particle instances
	m_pRenderer->BeginGLSLShader(m_instanceShader);

	GLint projMatrixLoc = glGetUniformLocation(pShader->GetProgramObject(), "projMatrix");
	GLint viewMatrixLoc = glGetUniformLocation(pShader->GetProgramObject(), "viewMatrix");

	Matrix4 projMat;
	Matrix4 viewMat;
	m_pRenderer->GetProjectionMatrix(&projMat);
	m_pRenderer->GetModelViewMatrix(&viewMat);

	glUniformMatrix4fv(projMatrixLoc, 1, false, projMat.m_data);
	glUniformMatrix4fv(viewMatrixLoc, 1, false, viewMat.m_data);

	// GLint inLightPosition = glGetUniformLocation(pShader->GetProgramObject(), "in_light_position");
	// GLint inLightConstA = glGetUniformLocation(pShader->GetProgramObject(), "in_light_const_a");
	// GLint inLightLinearA = glGetUniformLocation(pShader->GetProgramObject(), "in_light_linear_a");
	// GLint inLightQuadA = glGetUniformLocation(pShader->GetProgramObject(), "in_light_quad_a");
	// GLint inLightAmbient = glGetUniformLocation(pShader->GetProgramObject(), "in_light_ambient");
	// GLint inLightDiffuse = glGetUniformLocation(pShader->GetProgramObject(), "in_light_diffuse");

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

	glDrawElementsInstanced(GL_QUADS, 24, GL_UNSIGNED_INT, indices, numBlockParticlesRender);

	m_pRenderer->DisableTransparency();

	m_pRenderer->EndGLSLShader(m_instanceShader);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void BlockParticleManager::RenderDefault(bool noWorldOffset)
{
	// Render all block particles
	for (auto iter = m_vpBlockParticlesList.begin(); iter != m_vpBlockParticlesList.end(); ++iter)
	{
		BlockParticle* pBlockParticle = *iter;

		// Update the block's alpha depending on the life left
		for (int i = 0; i < 24; ++i)
		{
			m_vertexBuffer[i].r = pBlockParticle->m_currentRed;
			m_vertexBuffer[i].g = pBlockParticle->m_currentGreen;
			m_vertexBuffer[i].b = pBlockParticle->m_currentBlue;
			m_vertexBuffer[i].a = pBlockParticle->m_currentAlpha;
		}

		if (m_renderWireFrame)
		{
			m_pRenderer->SetLineWidth(1.0f);
			m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
			m_pRenderer->SetCullMode(CullMode::NOCULL);
		}
		else
		{
			m_pRenderer->SetRenderMode(RenderMode::SOLID);
		}

		RenderBlockParticle(pBlockParticle, noWorldOffset);
	}
}

void BlockParticleManager::RenderBlockParticle(BlockParticle* pBlockParticle, bool noWorldOffset) const
{
	pBlockParticle->CalculateWorldTransformMatrix();

	m_pRenderer->PushMatrix();

	if (noWorldOffset)
	{
		m_pRenderer->MultiplyWorldMatrix(pBlockParticle->m_worldMatrixNoPositionOffset);
	}
	else
	{
		m_pRenderer->MultiplyWorldMatrix(pBlockParticle->m_worldMatrix);
	}

	Matrix4 worldMatrix;

	m_pRenderer->GetModelMatrix(&worldMatrix);

	m_pRenderer->PushTextureMatrix();

	m_pRenderer->MultiplyWorldMatrix(worldMatrix);

	m_pRenderer->PushMatrix();

	m_pRenderer->SetPrimitiveMode(PrimitiveMode::QUADS);
	m_pRenderer->RenderFromArray(VertexType::POSITION_NORMAL_COLOR, m_blockMaterialID, 0, 24, 24, 0, &m_vertexBuffer, nullptr, nullptr);

	m_pRenderer->PopMatrix();

	m_pRenderer->PopTextureMatrix();

	m_pRenderer->PopMatrix();
}

void BlockParticleManager::RenderDebug()
{
	RenderEmitters();
	RenderEffects();
}

void BlockParticleManager::RenderEmitters()
{
	for (size_t i = 0; i < m_vpBlockParticleEmittersList.size(); ++i)
	{
		BlockParticleEmitter* lpBlockParticleEmitter = m_vpBlockParticleEmittersList[i];

		lpBlockParticleEmitter->Render();
	}
}

void BlockParticleManager::RenderEffects()
{
	for (size_t i = 0; i < m_vpBlockParticleEffectsList.size(); ++i)
	{
		BlockParticleEffect* lpBlockParticleEffect = m_vpBlockParticleEffectsList[i];

		lpBlockParticleEffect->Render();
	}
}
