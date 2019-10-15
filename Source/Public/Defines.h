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
#define TOGGLE(A,B,flag) if(flag) A(); else B(); flag = !flag;

#define DELTA_TIME_STEP 0.02f
#define LOG(X) OutputDebugString(X)
#define LOG_CMD(X) std::cout<<X<<std::endl;
#define LOAD_CHECK(X, Path) if(!X) { std::string _s = "Cannot find file "; _s+=Path; _s+=". Load failed.\n"; LOG(_s.c_str());}
#define CORNFLOWER_BLUE sf::Color(100, 149, 237)

#define GAMETAG_PLAYER_FOOT						1 

#define TEXT_SIZE_S 9
#define TEXT_SIZE_M 16
#define TEXT_SIZE_L 30

#define DEBUG_ARROW_SIZE					sf::Vector2f(100.0f,43.f)

// Resources Path Defines.

#define RESOURCES_FONT_DEFAULT				"Resources/arial.ttf"
#define RESOURCES_FONT_PIXEL				"Resources/04b03.ttf"
#define RESOURCES_FONT_MOLOT				"Resources/Molot.otf"

#define RESOURCES_TEXTURE_DEBUG_ARROW		"Resources/debug_arrow.png"

#define RESOURCES_TEXTURE_CHALKBOARD		"Resources/chalkboard.jpg"



