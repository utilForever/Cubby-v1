/*************************************************************************
> File Name: FrontendScreens.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    An enum list of all the different front-end screens, these correspond to
>    the front-end page classes.
> Created Time: 2016/09/03
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_FRONTEND_SCREENS_H
#define CUBBY_FRONTEND_SCREENS_H

enum class FrontendScreen
{
	None = 0,

	Intro,

	MainMenu,
	SelectCharacter,
	CreateCharacter,
	SelectWorld,
	CreateWorld,
	Credits,

	PauseMenu,
	OptionsMenu,
	ModMenu,

	QuitPopup,
};

#endif