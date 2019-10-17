#pragma once
#include <string>
#include <iostream>
#include <cstdio>
#include <wchar.h>
#include <Windows.h>
#include <stdarg.h>
#include <ctype.h>
#include <assert.h>

//flow control
#define TOGGLE(A,B,flag)			if(flag) A(); else B(); flag = !flag;

#define DELTA_TIME_STEP				0.02f
#define LOG(X)						OutputDebugString(X)
#define LOG_CMD(X)					std::cout<<X<<std::endl;
#define LOAD_CHECK(X, Path)			if(!X) { std::string _s = "Cannot find file "; _s+=Path; _s+=". Load failed.\n"; LOG(_s.c_str());}
#define CORNFLOWER_BLUE				sf::Color(100, 149, 237)

#define UNIT						32.0f
#define UNIT_ZERO_VECTOR			sf::Vector2f(0.0f, 0.0f)
#define UNIT_VECTOR					sf::Vector2f(32.0f,32.0f)
#define UNIT_BOX2D_TO_SFML(X)		((X)*32.0f)
#define UNIT_SFML_TO_BOX2D(X)		((X)/32.0f)
#define UNIT_BOX2D_TO_SFML_VEC(X,Y)	sf::Vector2f((X)*32.0f), ((Y)*32.0f)
#define UNIT_SFML_TO_BOX2D_VEC(X,Y)	sf::Vector2f((X)/32.0f), ((Y)/32.0f)

#define GAMETAG_STATIC_OBJECT	1

#define GAMETAG_PLAYER_BODY		11
#define GAMETAG_PLAYER_FOOT		12


#define TEXT_SIZE_S 9
#define TEXT_SIZE_M 16
#define TEXT_SIZE_L 30

// PS4 Gamepad
#define PS4_Square				0
#define PS4_Cross				1
#define PS4_Circle				2
#define PS4_Triangle			3
#define PS4_LeftShoulder		4
#define PS4_RightShoulder		5
#define PS4_LeftTrigger			6
#define PS4_RightTrigger		7
#define PS4_Share				8
#define PS4_Option				9
#define PS4_LeftThumb			10
#define PS4_RightThumb			11
#define PS4_Special				12
#define PS4_TouchPad			13
#define PS4_LeftXAxis			(sf::Joystick::Axis)0 //-100 Left, 100 Right
#define PS4_LeftYAxis			(sf::Joystick::Axis)1 //-100 Up, 100 Down
#define PS4_RightXAxis			(sf::Joystick::Axis)2 //-100 Left, 100 Right
#define PS4_RightYAxis			(sf::Joystick::Axis)3 //-100 Up, 100 Down
#define PS4_LeftTriggerAxis		(sf::Joystick::Axis)4 //-100 Release, 100 Press
#define PS4_RightTriggerAxis	(sf::Joystick::Axis)5 //-100 Release, 100 Press	
#define PS4_DPadX				(sf::Joystick::Axis)6 //-100 Left, 100 Right
#define PS4_DPadY				(sf::Joystick::Axis)7 //-100 Up, 100 Down


#define DEBUG_ARROW_SIZE					sf::Vector2f(100.0f,43.f)

// Resources Path Defines.
#define RESOURCES_FONT_DEFAULT				"Resources/arial.ttf"
#define RESOURCES_FONT_PIXEL				"Resources/04b03.ttf"
#define RESOURCES_FONT_MOLOT				"Resources/Molot.otf"
#define RESOURCES_TEXTURE_DEBUG_ARROW		"Resources/debug_arrow.png"



