#pragma once
#include "SDL.h"
#include <memory>
class Input
{
public:
	Input();
	~Input();
	static void SetKey( int KeyCode )
	{
		m_keys[ KeyCode ] = true;
	}
	static void ResetKey( int KeyCode )
	{
		m_keys[ KeyCode ] = false;
	}

	static void Reset()
	{
		memset( &m_keys , 0 , sizeof(bool)*SDL_NUM_SCANCODES );
	}

	static bool GetKey( int KeyCode )
	{
		return m_keys[ KeyCode ];
	}

	static bool m_keys[SDL_NUM_SCANCODES];
};

