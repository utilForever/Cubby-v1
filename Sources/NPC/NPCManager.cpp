/*************************************************************************
> File Name: NPCManager.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    Manager class to control all of the NPC creation, updating, rendering and
>    general management of NPC characters.
> Created Time: 2016/08/06
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <Player/Player.h>

#include "NPCManager.h"
#include <CubbyGame.h>
#include <algorithm>

// Constants
float NPCManager::NPC_INTERACTION_DISTANCE = 4.5f;
float NPCManager::NPC_INTERACTION_RADIUS_CHECK = 0.65f;

// Constructor, Destructor
NPCManager::NPCManager(Renderer* pRenderer, ChunkManager* pChunkManager) :
	m_pRenderer(pRenderer), m_pChunkManager(pChunkManager),
	m_pLightingManager(nullptr), m_pPlayer(nullptr), m_pBlockParticleManager(nullptr),
	m_pTextEffectsManager(nullptr), m_pItemManager(nullptr), m_pProjectileManager(nullptr),
	m_pQubicleBinaryManager(nullptr), m_pEnemyManager(nullptr), m_numRenderNPCs(0)
{

}

NPCManager::~NPCManager()
{
	ClearNPCs();
}

void NPCManager::SetPlayer(Player* pPlayer)
{
	m_pPlayer = pPlayer;
}

void NPCManager::SetLightingManager(LightingManager* pLightingManager)
{
	m_pLightingManager = pLightingManager;

	m_NPCMutex.lock();

	for (size_t i = 0; i < m_vpNPCList.size(); ++i)
	{
		m_vpNPCList[i]->SetLightingManager(m_pLightingManager);
	}

	m_NPCMutex.unlock();
}

void NPCManager::SetBlockParticleManager(BlockParticleManager* pBlockParticleManager)
{
	m_pBlockParticleManager = pBlockParticleManager;
}

void NPCManager::SetTextEffectsManager(TextEffectsManager* pTextEffectsManager)
{
	m_pTextEffectsManager = pTextEffectsManager;
}

void NPCManager::SetItemManager(ItemManager* pItemManager)
{
	m_pItemManager = pItemManager;
}

void NPCManager::SetProjectileManager(ProjectileManager* pProjectileManager)
{
	m_pProjectileManager = pProjectileManager;
}

void NPCManager::SetEnemyManager(EnemyManager* pEnemyManager)
{
	m_pEnemyManager = pEnemyManager;
}

void NPCManager::SetQubicleBinaryManager(QubicleBinaryManager* pQubicleBinaryManager)
{
	m_pQubicleBinaryManager = pQubicleBinaryManager;
}

// Clearing
void NPCManager::ClearNPCs()
{
	m_NPCMutex.lock();

	for (size_t i = 0; i < m_vpNPCList.size(); ++i)
	{
		delete m_vpNPCList[i];
		m_vpNPCList[i] = nullptr;
	}
	
	m_vpNPCList.clear();
	
	m_NPCMutex.unlock();
}

void NPCManager::ClearNPCChunkCacheForChunk(Chunk* pChunk)
{
	m_NPCMutex.lock();

	for (size_t i = 0; i < m_vpNPCList.size(); ++i)
	{
		m_vpNPCList[i]->ClearChunkCacheForChunk(pChunk);
	}

	m_NPCMutex.unlock();
}

// Creation
NPC* NPCManager::CreateNPC(const char* name, std::string typeName, std::string modelName, glm::vec3 position, float scale, bool characterSelectScreen, bool useQubicleManager)
{
	// Check if we already have an NPC with the same name
	m_NPCMutex.lock();

	for (size_t i = 0; i < m_vpNPCList.size(); ++i)
	{
		if (m_vpNPCList[i]->GetName() == name)
		{
			m_NPCMutex.unlock();
			return nullptr; // Don't allow creation of 2 same named NPCs
		}
	}

	m_NPCMutex.unlock();

	NPC* pNewNPC = new NPC(m_pRenderer, m_pChunkManager, m_pPlayer, m_pLightingManager, m_pBlockParticleManager, m_pTextEffectsManager, m_pItemManager, m_pProjectileManager, m_pEnemyManager, m_pQubicleBinaryManager, name, typeName, modelName, characterSelectScreen, useQubicleManager);

	pNewNPC->SetLightingManager(m_pLightingManager);
	pNewNPC->SetPosition(position);

	float randomScaleAddition = 0.0f;
	// Can have a random scale difference of scale of +- 5%
	//float randomScaleAddition = (scale * 0.05f) * GetRandomNumber(-100, 100, 2) * 0.01f;
	pNewNPC->SetScale(scale + randomScaleAddition);

	pNewNPC->SetMoveBackToPosition(position - glm::vec3(0.0f, pNewNPC->GetRadius(), 0.0f));

	m_NPCMutex.lock();

	m_vpNPCList.push_back(pNewNPC);

	m_NPCMutex.unlock();

	return pNewNPC;
}

void NPCManager::DeleteNPC(std::string name)
{
	NPC* pDeleteObject = nullptr;
	
	// Find
	m_NPCMutex.lock();

	for (size_t i = 0; i < m_vpNPCList.size(); ++i)
	{
		if (m_vpNPCList[i]->GetName() == name)
		{
			pDeleteObject = m_vpNPCList[i];
		}
	}

	// Erase from container
	auto iter = find(m_vpNPCList.begin(), m_vpNPCList.end(), pDeleteObject);
	if (iter != m_vpNPCList.end())
	{
		m_vpNPCList.erase(iter);
	}

	m_NPCMutex.unlock();

	// Delete
	if (pDeleteObject != nullptr)
	{
		delete pDeleteObject;
	}
}

NPC* NPCManager::GetNPC(int index)
{
	m_NPCMutex.lock();

	NPC* pReturn = m_vpNPCList[index];

	m_NPCMutex.unlock();

	return pReturn;
}

NPC* NPCManager::GetNPCByName(std::string name)
{
	m_NPCMutex.lock();

	for (size_t i = 0; i < m_vpNPCList.size(); ++i)
	{
		NPC* pNPC = m_vpNPCList[i];

		if (pNPC->GetName() == name)
		{
			m_NPCMutex.unlock();
			return pNPC;
		}
	}

	m_NPCMutex.unlock();

	return nullptr;
}

int NPCManager::GetNumNPCs()
{
	m_NPCMutex.lock();

	int numNPCs = static_cast<int>(m_vpNPCList.size());

	m_NPCMutex.unlock();

	return numNPCs;
}

int NPCManager::GetNumRenderNPCs() const
{
	return m_numRenderNPCs;
}

void NPCManager::ResetNumRenderNPCs()
{
	m_numRenderNPCs = 0;
}

// Enemy Died
void NPCManager::SetEnemyDied(Enemy* pEnemy)
{
	m_NPCMutex.lock();
	for (unsigned int i = 0; i < m_vpNPCList.size(); i++)
	{
		NPC* pNPC = m_vpNPCList[i];

		pNPC->SetEnemyDied(pEnemy);
	}
	m_NPCMutex.unlock();
}

// Collision
void NPCManager::PushCollisions(NPC* pPushingNPC, glm::vec3 position, float radius)
{
	m_NPCMutex.lock();

	for (auto iter = m_vpNPCList.begin(); iter != m_vpNPCList.end(); ++iter)
	{
		NPC* pNPC = *iter;

		if (pNPC == pPushingNPC)
		{
			continue;  // Can't push ourselves
		}

		if (pNPC->IsPushingCollisionEnabled() == false)
		{
			continue;
		}

		glm::vec3 distance = pNPC->GetCenter() - position;
		float lengthValue = length(distance) - radius - pNPC->GetRadius();

		if (lengthValue < 0.0f)
		{
			glm::vec3 pushVector = distance;
			pushVector.y = 0.0f;  // Don't push in Y direction
			pushVector *= lengthValue;
			pNPC->SetPosition(pNPC->GetPosition() - pushVector);
		}
	}

	m_NPCMutex.unlock();
}

// Interaction
NPC* NPCManager::CheckNPCPlayerInteraction()
{
	// Check if any NPCs are within interaction range
	m_NPCMutex.lock();

	for (auto iter = m_vpNPCList.begin(); iter != m_vpNPCList.end(); ++iter)
	{
		NPC* pNPC = *iter;

		glm::vec3 distance = pNPC->GetCenter() - m_pPlayer->GetCenter();
		float lengthValue = length(distance);

		glm::vec3 direction = normalize(distance);
		glm::vec3 facing = m_pPlayer->GetForwardVector();

		float dotProduct = dot(direction, facing);

		if (lengthValue > pNPC->GetRadius() * 2.0f && lengthValue < NPC_INTERACTION_DISTANCE + pNPC->GetRadius())
		{
			// Make sure we are facing
			if (dotProduct > NPC_INTERACTION_RADIUS_CHECK)
			{
				m_NPCMutex.unlock();
				return pNPC;
			}
		}
	}

	m_NPCMutex.unlock();

	return nullptr;
}

// Rendering Helpers
void NPCManager::SetWireFrameRender(bool wireframe)
{
	m_NPCMutex.lock();

	for (size_t i = 0; i < m_vpNPCList.size(); ++i)
	{
		NPC* pNPC = m_vpNPCList[i];

		pNPC->SetWireFrameRender(wireframe);
	}

	m_NPCMutex.unlock();
}

// Updating
void NPCManager::UpdateNamePickingSelection(int pickingID)
{
	m_NPCMutex.lock();
	for (unsigned int i = 0; i < m_vpNPCList.size(); i++)
	{
		NPC* pNPC = m_vpNPCList[i];

		if (pNPC->IsCreditsNPC())
		{
			// Don't allow selection of credits NPCs for credits screen
			continue;
		}

		if (pNPC->GetSubSelectionRender())
		{
			pNPC->UpdateSubSelectionNamePicking(pickingID, true);
		}
		else
		{
			if (pickingID == Player::PLAYER_NAME_PICKING + 100 + i)
			{
				if (pNPC->HasReachedTargetPosition())
				{
					pNPC->SetOutlineRender(true);

					// Character selection and creation screens
					if (CubbyGame::GetInstance()->GetGameMode() == GameMode::FrontEnd)
					{
						CubbyGame::GetInstance()->GetFrontendManager()->SetSelectedNPC(pNPC);
					}
				}
			}
			else
			{
				pNPC->SetOutlineRender(false);
			}
		}
	}

	m_NPCMutex.unlock();

	if (pickingID == -1)
	{
		// Character selection and creation screens
		if (CubbyGame::GetInstance()->GetGameMode() == GameMode::FrontEnd)
		{
			CubbyGame::GetInstance()->GetFrontendManager()->SetSelectedNPC(nullptr);
		}
	}
}

void NPCManager::UpdateHoverNamePickingSelection(int pickingID)
{
	m_NPCMutex.lock();

	for (size_t i = 0; i < m_vpNPCList.size(); ++i)
	{
		NPC* pNPC = m_vpNPCList[i];

		if (pNPC->IsCreditsNPC())
		{
			// Don't allow hover of credits NPCs for credits screen
			continue;
		}

		if (pNPC->GetSubSelectionRender())
		{
			pNPC->UpdateSubSelectionNamePicking(pickingID, false);
		}
		else
		{
			if (pickingID == Player::PLAYER_NAME_PICKING + 100 + i)
			{
				if (pNPC->HasReachedTargetPosition())
				{
					pNPC->SetHoverRender(true);

					// Character selection and creation screens
					if (CubbyGame::GetInstance()->GetGameMode() == GameMode::FrontEnd)
					{
						CubbyGame::GetInstance()->GetFrontendManager()->SetHoverNPC(pNPC);
					}
				}
			}
			else
			{
				pNPC->SetHoverRender(false);
			}
		}
	}

	m_NPCMutex.unlock();

	if (pickingID == -1)
	{
		// Character selection and creation screens
		if (CubbyGame::GetInstance()->GetGameMode() == GameMode::FrontEnd)
		{
			CubbyGame::GetInstance()->GetFrontendManager()->SetHoverNPC(nullptr);
		}
	}
}

void NPCManager::UpdateWeaponLights()
{
	m_NPCMutex.lock();

	for (size_t i = 0; i < m_vpNPCList.size(); ++i)
	{
		NPC* pNPC = m_vpNPCList[i];

		// TODO: Loader radius culling
		//if(m_pChunkManager->IsInsideLoader(pNPC->GetCenter()) == false)
		//{
		//	continue;
		//}

		pNPC->UpdateWeaponLights();
	}
	
	m_NPCMutex.unlock();
}

void NPCManager::UpdateWeaponParticleEffects()
{
	m_NPCMutex.lock();

	for (size_t i = 0; i < m_vpNPCList.size(); ++i)
	{
		NPC* pNPC = m_vpNPCList[i];

		// TODO : Loader radius culling
		//if(m_pChunkManager->IsInsideLoader(pNPC->GetCenter()) == false)
		//{
		//	continue;
		//}

		pNPC->UpdateWeaponParticleEffects();
	}

	m_NPCMutex.unlock();
}

bool NPCNeedsErase(NPC* npc)
{
	bool needErase = npc->NeedErase();

	if (needErase == true)
	{
		delete npc;
	}

	return needErase;
}

void NPCManager::Update(float dt)
{
	// Remove any NPC that need to be erased
	m_NPCMutex.lock();

	m_vpNPCList.erase(remove_if(m_vpNPCList.begin(), m_vpNPCList.end(), NPCNeedsErase), m_vpNPCList.end());
	
	m_NPCMutex.unlock();

	// Update the mouse hover NPC selection
	UpdateHoverNPCs();

	// Update all NPCs
	m_NPCMutex.lock();

	for (size_t i = 0; i < m_vpNPCList.size(); ++i)
	{
		NPC* pNPC = m_vpNPCList[i];

		// TODO: Loader radius culling
		//if(m_pChunkManager->IsInsideLoader(pNPC->GetCenter()) == false)
		//{
		//	continue;
		//}

		pNPC->Update(dt);

		m_NPCMutex.unlock();

		// Allow NPCs to push each other away (simple collision).
		PushCollisions(pNPC, pNPC->GetCenter(), pNPC->GetRadius());

		m_NPCMutex.lock();
	}

	m_NPCMutex.unlock();

	// Update weapon lights
	UpdateWeaponLights();

	// Update weapon particle effects
	UpdateWeaponParticleEffects();

	// Check the projectile collisions
	UpdateNPCProjectileCheck();
}

void NPCManager::UpdateScreenCoordinates2d(Camera* pCamera)
{
	m_NPCMutex.lock();

	for (size_t i = 0; i < m_vpNPCList.size(); ++i)
	{
		NPC* pNPC = m_vpNPCList[i];

		// TODO: Loader radius culling
		//if(m_pChunkManager->IsInsideLoader(pNPC->GetCenter()) == false)
		//{
		//	continue;
		//}

		pNPC->UpdateScreenCoordinates2d(pCamera);
	}

	m_NPCMutex.unlock();
}

void NPCManager::UpdateHoverNPCs()
{
	// Check if any item are within interaction range
	m_NPCMutex.lock();

	for (auto iter = m_vpNPCList.begin(); iter != m_vpNPCList.end(); ++iter)
	{
		NPC* pNPC = *iter;

		if (pNPC->GetOutlineRender() && pNPC->IsFrontEndNPC() == false)
		{
			pNPC->SetOutlineRender(false);
		}

		if (pNPC->GetState() == NPCState::Combat)
		{
			continue;
		}

		glm::vec3 distance = pNPC->GetCenter() - m_pPlayer->GetCenter();
		float lengthValue = length(distance);

		glm::vec3 direction = normalize(distance);
		glm::vec3 facing = m_pPlayer->GetForwardVector();

		float dotProduct = dot(direction, facing);

		if (lengthValue > pNPC->GetRadius()*2.0f && lengthValue < NPC_INTERACTION_DISTANCE + pNPC->GetRadius())
		{
			// Make sure we are facing
			if (dotProduct > NPC_INTERACTION_RADIUS_CHECK)
			{
				pNPC->SetOutlineRender(true);
			}
		}
	}

	m_NPCMutex.unlock();
}

void NPCManager::UpdateNPCProjectileCheck()
{
	m_NPCMutex.lock();

	for (size_t i = 0; i < m_vpNPCList.size(); ++i)
	{
		NPC* pNPC = m_vpNPCList[i];

		if (pNPC->NeedErase() == true)
		{
			continue;
		}

		// TODO: Loader radius culling
		//if(m_pChunkManager->IsInsideLoader(pNPC->GetCenter()) == false)
		//{
		//	continue;
		//}

		for (int j = 0; j < m_pProjectileManager->GetNumProjectiles(); ++j)
		{
			Projectile* pProjectile = m_pProjectileManager->GetProjectile(j);

			if (pProjectile != nullptr && pProjectile->GetErase() == false)
			{
				pNPC->CheckProjectileDamageRadius(pProjectile);
			}
		}
	}

	m_NPCMutex.unlock();
}

void NPCManager::CalculateWorldTransformMatrix()
{
	m_NPCMutex.lock();

	for (size_t i = 0; i < m_vpNPCList.size(); ++i)
	{
		NPC* pNPC = m_vpNPCList[i];

		pNPC->CalculateWorldTransformMatrix();
	}

	m_NPCMutex.unlock();
}

// Rendering
void NPCManager::Render(bool outline, bool reflection, bool silhouette, bool renderOnlyOutline, bool renderOnlyNormal, bool shadow)
{
	m_NPCMutex.lock();

	for (size_t i = 0; i < m_vpNPCList.size(); ++i)
	{
		NPC* pNPC = m_vpNPCList[i];

		if (pNPC->GetSubSelectionRender())
		{
			// If we are sub selecting this NPC parts, render this in a different flow
			if (m_pRenderer->SphereInFrustum(CubbyGame::GetInstance()->GetDefaultViewport(), pNPC->GetCenter(), pNPC->GetRadius()))
			{
				pNPC->RenderSubSelection(false, true);
			}
		}
		else
		{
			if (silhouette && (pNPC->GetOutlineRender() == false && pNPC->GetHoverRender() == false))
			{
				continue; // Don't render silhouette unless we are rendering outline
			}

			if (renderOnlyNormal && (pNPC->GetOutlineRender() || pNPC->GetHoverRender()))
			{
				continue;
			}
			if (renderOnlyOutline && pNPC->GetOutlineRender() == false && pNPC->GetHoverRender() == false)
			{
				continue;
			}

			// TODO: Loader radius culling
			//if(m_pChunkManager->IsInsideLoader(pNPC->GetCenter()) == false)
			//{
			//	continue;
			//}

			// Fog
			float toCamera = length(CubbyGame::GetInstance()->GetGameCamera()->GetPosition() - pNPC->GetCenter());
			if (toCamera > m_pChunkManager->GetLoaderRadius() + (Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 5.0f))
			{
				continue;
			}
			if (toCamera > m_pChunkManager->GetLoaderRadius() - Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 3.0f)
			{
				m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
			}

			if (pNPC->GetSubSelectionRender() == false)
			{
				if (shadow || m_pRenderer->SphereInFrustum(CubbyGame::GetInstance()->GetDefaultViewport(), pNPC->GetCenter(), pNPC->GetRadius()))
				{
					pNPC->Render(outline, reflection, silhouette);

					m_numRenderNPCs++;
				}
			}

			m_pRenderer->DisableTransparency();
		}
	}

	m_NPCMutex.unlock();
}

void NPCManager::RenderFaces()
{
	m_NPCMutex.lock();

	for (size_t i = 0; i < m_vpNPCList.size(); ++i)
	{
		NPC* pNPC = m_vpNPCList[i];

		// TODO: Loader radius culling
		//if(m_pChunkManager->IsInsideLoader(pNPC->GetCenter()) == false)
		//{
		//	continue;
		//}

		// Fog
		float toCamera = length(CubbyGame::GetInstance()->GetGameCamera()->GetPosition() - pNPC->GetCenter());
		if (toCamera > m_pChunkManager->GetLoaderRadius() + (Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 5.0f))
		{
			continue;
		}
		if (toCamera > m_pChunkManager->GetLoaderRadius() - Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 3.0f)
		{
			m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
		}

		if (m_pRenderer->SphereInFrustum(CubbyGame::GetInstance()->GetDefaultViewport(), pNPC->GetCenter(), pNPC->GetRadius()))
		{
			pNPC->RenderFace();
		}

		m_pRenderer->DisableTransparency();
	}
	
	m_NPCMutex.unlock();
}

void NPCManager::RenderWeaponTrails()
{
	m_NPCMutex.lock();

	for (size_t i = 0; i < m_vpNPCList.size(); ++i)
	{
		NPC* pNPC = m_vpNPCList[i];

		// TODO: Loader radius culling
		//if(m_pChunkManager->IsInsideLoader(pNPC->GetCenter()) == false)
		//{
		//	continue;
		//}

		// Fog
		float toCamera = length(CubbyGame::GetInstance()->GetGameCamera()->GetPosition() - pNPC->GetCenter());
		if (toCamera > m_pChunkManager->GetLoaderRadius() + (Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 5.0f))
		{
			continue;
		}
		if (toCamera > m_pChunkManager->GetLoaderRadius() - Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 3.0f)
		{
			m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
		}

		if (m_pRenderer->SphereInFrustum(CubbyGame::GetInstance()->GetDefaultViewport(), pNPC->GetCenter(), pNPC->GetRadius()))
		{
			pNPC->RenderWeaponTrails();
		}

		m_pRenderer->DisableTransparency();
	}

	m_NPCMutex.unlock();
}

void NPCManager::RenderNamePicking()
{
	m_NPCMutex.lock();

	for (size_t i = 0; i < m_vpNPCList.size(); ++i)
	{
		NPC* pNPC = m_vpNPCList[i];

		// TODO: Loader radius culling
		//if(m_pChunkManager->IsInsideLoader(pNPC->GetCenter()) == false)
		//{
		//	continue;
		//}

		if (pNPC->GetSubSelectionRender())
		{
			// If we are sub selecting this NPC parts, render the name picking separately
			if (m_pRenderer->SphereInFrustum(CubbyGame::GetInstance()->GetDefaultViewport(), pNPC->GetCenter(), pNPC->GetRadius()))
			{
				pNPC->RenderSubSelectionNamePicking();
			}
		}
		else
		{
			if (m_pRenderer->SphereInFrustum(CubbyGame::GetInstance()->GetDefaultViewport(), pNPC->GetCenter(), pNPC->GetRadius()))
			{
				m_pRenderer->LoadNameOntoStack(Player::PLAYER_NAME_PICKING + 100 + i);
				pNPC->Render(false, false, false);
				m_pRenderer->EndNameStack();
			}
		}
	}
	
	m_NPCMutex.unlock();
}

void NPCManager::RenderOutlineNPCs()
{
	m_NPCMutex.lock();

	for (size_t i = 0; i < m_vpNPCList.size(); ++i)
	{
		NPC* pNPC = m_vpNPCList[i];

		if (pNPC->GetSubSelectionRender())
		{
			// If we are sub selecting this NPC parts, render this in a different flow
			if (m_pRenderer->SphereInFrustum(CubbyGame::GetInstance()->GetDefaultViewport(), pNPC->GetCenter(), pNPC->GetRadius()))
			{
				pNPC->RenderSubSelection(true, false);
			}
		}
		else
		{
			if (pNPC->GetOutlineRender() == false && pNPC->GetHoverRender() == false)
			{
				continue;
			}

			// TODO: Loader radius culling
			//if(m_pChunkManager->IsInsideLoader(pNPC->GetCenter()) == false)
			//{
			//	continue;
			//}

			// Fog
			float toCamera = length(CubbyGame::GetInstance()->GetGameCamera()->GetPosition() - pNPC->GetCenter());
			if (toCamera > m_pChunkManager->GetLoaderRadius() + (Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 5.0f))
			{
				continue;
			}
			if (toCamera > m_pChunkManager->GetLoaderRadius() - Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 3.0f)
			{
				m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
			}

			if (m_pRenderer->SphereInFrustum(CubbyGame::GetInstance()->GetDefaultViewport(), pNPC->GetCenter(), pNPC->GetRadius()))
			{
				pNPC->Render(true, false, false);
			}

			m_pRenderer->DisableTransparency();
		}
	}

	m_NPCMutex.unlock();
}

void NPCManager::RenderSubSelectionNPCs()
{
	m_NPCMutex.lock();

	for (size_t i = 0; i < m_vpNPCList.size(); ++i)
	{
		NPC* pNPC = m_vpNPCList[i];

		// TODO: Loader radius culling
		//if(m_pChunkManager->IsInsideLoader(pNPC->GetCenter()) == false)
		//{
		//	continue;
		//}

		if (pNPC->GetSubSelectionRender())
		{
			if (m_pRenderer->SphereInFrustum(CubbyGame::GetInstance()->GetDefaultViewport(), pNPC->GetCenter(), pNPC->GetRadius()))
			{
				pNPC->Render(false, false, false);
			}
		}
	}

	m_NPCMutex.unlock();
}

void NPCManager::RenderSubSelectionNormalNPCs()
{
	m_NPCMutex.lock();

	for (size_t i = 0; i < m_vpNPCList.size(); ++i)
	{
		NPC* pNPC = m_vpNPCList[i];

		// TODO: Loader radius culling
		//if(m_pChunkManager->IsInsideLoader(pNPC->GetCenter()) == false)
		//{
		//	continue;
		//}

		if (pNPC->GetSubSelectionRender())
		{
			if (m_pRenderer->SphereInFrustum(CubbyGame::GetInstance()->GetDefaultViewport(), pNPC->GetCenter(), pNPC->GetRadius()))
			{
				pNPC->RenderSubSelectionNormal();
			}
		}
	}

	m_NPCMutex.unlock();
}

void NPCManager::RenderSubSelectionOverlayNPCs()
{
	m_NPCMutex.lock();

	for (size_t i = 0; i < m_vpNPCList.size(); ++i)
	{
		NPC* pNPC = m_vpNPCList[i];

		// TODO: Loader radius culling
		//if(m_pChunkManager->IsInsideLoader(pNPC->GetCenter()) == false)
		//{
		//	continue;
		//}

		if (pNPC->GetSubSelectionRender())
		{
			if (m_pRenderer->SphereInFrustum(CubbyGame::GetInstance()->GetDefaultViewport(), pNPC->GetCenter(), pNPC->GetRadius()))
			{
				pNPC->RenderSubSelection(false, false);
			}
		}
	}

	m_NPCMutex.unlock();
}

void NPCManager::RenderDebug()
{
	m_NPCMutex.lock();

	for (size_t i = 0; i < m_vpNPCList.size(); ++i)
	{
		NPC* pNPC = m_vpNPCList[i];

		// TODO: Loader radius culling
		//if(m_pChunkManager->IsInsideLoader(pNPC->GetCenter()) == false)
		//{
		//	continue;
		//}

		// Fog
		float toCamera = length(CubbyGame::GetInstance()->GetGameCamera()->GetPosition() - pNPC->GetCenter());
		if (toCamera > m_pChunkManager->GetLoaderRadius() + (Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 5.0f))
		{
			continue;
		}
		if (toCamera > m_pChunkManager->GetLoaderRadius() - Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 3.0f)
		{
			m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
		}

		if (m_pRenderer->SphereInFrustum(CubbyGame::GetInstance()->GetDefaultViewport(), pNPC->GetCenter(), pNPC->GetRadius()))
		{
			pNPC->RenderDebug();
		}

		m_pRenderer->DisableTransparency();
	}

	m_NPCMutex.unlock();
}