/*************************************************************************
> File Name: CubbyRender.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Implement CubbyGame's methods related to rendering.
> Created Time: 2016/07/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "CubbyGame.h"

// Rendering
void CubbyGame::PreRender()
{

}

void CubbyGame::Render()
{
	if (m_pCubbyWindow->GetMinimized())
	{
		// Don't call any render functions if minimized
		return;
	}

	// Begin rendering
	m_pRenderer->BeginScene(true, true, true);

	// TODO: Shadow rendering to the shadow frame buffer

	// ---------------------------------------
	// Render 3D
	// ---------------------------------------
	m_pRenderer->PushMatrix();

	// Set the default projection mode
	m_pRenderer->SetProjectionMode(ProjectionMode::PERSPECTIVE, m_defaultViewport);

	// Set back culling as default
	m_pRenderer->SetCullMode(CullMode::BACK);

	// Set default depth test
	m_pRenderer->EnableDepthTest(DepthTest::LESS);

	// Enable the lights
	m_pRenderer->PushMatrix();
	m_pRenderer->EnableLight(m_defaultLight, 0);
	m_pRenderer->PopMatrix();

	// Multisampling MSAA
	if (m_isMultiSampling)
	{
		m_pRenderer->EnableMultiSampling();
	}
	else
	{
		m_pRenderer->DisableMultiSampling();
	}

	// TODO: Water reflections

	// TODO: SSAO frame buffer rendering start

	m_pRenderer->SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	m_pRenderer->ClearScene(true, true, true);

	// Render the lights (DEBUG)
	m_pRenderer->PushMatrix();
	m_pRenderer->SetCullMode(CullMode::BACK);
	m_pRenderer->SetRenderMode(RenderMode::SOLID);
	m_pRenderer->RenderLight(m_defaultLight);
	m_pRenderer->PopMatrix();

	// TODO: Render the skybox
	
	// TODO: Render the chunks

	// TODO: NPC sub selection - For character creation screen

	// TODO: Render items outline and silhouette before the world/chunks
	
	// TODO: NPCs (only non outline and hover)
	
	// TODO: Enemies outline
	
	// TODO: Render the scene (Scenery, Projectiles, Items, NPCs, Enemies, NPCs (only outline and hover)

	// TODO: Player selection block

	// TODO: Render the block particles

	// TODO: Render the instanced objects

	// TODO: Frontend
	
	// TODO: Render the player

	// TODO: Render the transparency items above the water render, so that they appear properly under water

	// TODO: Debug rendering

	// TODO: Render player first person viewport

	// SSAO frame buffer rendering stop
	if (m_isDeferredRendering)
	{
		m_pRenderer->StopRenderingToFrameBuffer(m_SSAOFrameBuffer);
	}
	m_pRenderer->PopMatrix();

	// TODO: Render the deferred lighting pass

	// TODO: Render other viewports
	// TODO: Paperdoll for CharacterGUI
	// TODO: Portrait for HUD

	// ---------------------------------------
	// TODO: Render transparency
	// ---------------------------------------
	
	// TODO: Render the SSAO texture
	
	// ---------------------------------------
	// Render 2D
	// ---------------------------------------
	m_pRenderer->PushMatrix();
	
	// Disable multisampling for 2D GUI and text
	m_pRenderer->DisableMultiSampling();

	// TODO: Crosshair
	
	// TODO: Text effects
	
	// TODO: Cinematic mode (letter box)

	// TODO: Render the HUD

	m_pRenderer->PopMatrix();

	// TODO: Render other deferred rendering pipelines
	// TODO: Paperdoll SSAO for CharacterGUI

	// TODO: Portrait SSAO for HUD

	// TODO: Render the chunks 2D (debug text information)
	
	// TODO: Frontend 2D
	
	// TODO: Render the GUI

	// TODO: Custom cursor

	// TODO: Render debug information and text

	// TODO: Update the NPC screen positions for select character screen

	// End rendering
	m_pRenderer->EndScene();

	// Pass render call to the window class, allow to swap buffers
	m_pCubbyWindow->Render();
}