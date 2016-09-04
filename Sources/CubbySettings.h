/*************************************************************************
> File Name: CubbySettings.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>   Cubby settings, initalized at the application creation and contains all of
>   the run time settings and configuration that is loaded for Cubby.
> Created Time: 2016/07/09
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_SETTINGS_H
#define CUBBY_SETTINGS_H

// Forward declaration
class CubbyGame;

struct CubbySettings
{
	// Constructor, Destructor
	CubbySettings() = default;
	~CubbySettings() = default;

	// Load settings
	void LoadSettings();
	// Save settings
	void SaveSettings() const;

	// Load options
	void LoadOptions();
	// Save options
	void SaveOptions() const;

	// Options ini file
	// Gameplay
	bool m_invertedMouse;
	float m_mouseSensitivity;
	float m_gamepadSensitivity;

	// Sound
	bool m_audio;
	float m_audioVolume;
	bool m_music;
	float m_musicVolume;

	// Interface
	bool m_renderGUI;
	bool m_renderCrosshair;
	bool m_HUDAnimations;
	bool m_customCursors;
	bool m_equipHelper;
	bool m_tooltipDelay;
	bool m_confirmItemDelete;

	// Settings ini file
	// Graphics
	int m_windowWidth;
	int m_windowHeight;
	bool m_vsync;
	bool m_fullscreen;
	bool m_deferredRendering;
	bool m_shadows;
	bool m_blur;
	bool m_ssao;
	bool m_dynamicLighting;
	bool m_msaa;
	bool m_instancedParticles;
	bool m_faceMerging;
	bool m_fogRendering;
	bool m_waterRendering;

	// Landscape generation
	float m_landscapeOctaves;
	float m_landscapePersistence;
	float m_landscapeScale;
	float m_mountainOctaves;
	float m_mountainPersistence;
	float m_mountainScale;
	float m_mountainMultiplier;

	// Debug
	float m_loaderRadius;
	bool m_debugRendering;
	bool m_wireframeRendering;
	bool m_stepUpdating;
	bool m_showDebugGUI;
	std::string m_gameMode;
	std::string m_version;
};

#endif