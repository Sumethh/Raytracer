// Template for GP1, version 2
// IGAD/NHTV - Jacco Bikker - 2006-2014

#include "string.h"
#include "game.h"
#include "surface.h"
#include "stdlib.h"
#include "template.h"
#include "SDL.h"
#include "Input.h"
#include "ThreadJobSystem.h"
using namespace Tmpl8;
using namespace glm;

bool Input::m_keys[ SDL_NUM_SCANCODES ];
#include "raytracer.h"

Renderer renderer;
int currentLine = 0;
const float moveSpeed = 0.1;
const float RotationSpeed = 0.05f;
int frame = 0;

// == Game::Init ==============================================================
void Game::Init()
{
	jobSystem = new ThreadJobSystem(8);
	renderer.camera.Rotation = 0.0f;
	renderer.camera.Rotate(0.0f);
	renderer.camera.V = vec3(0, 0, 1.0f);
	renderer.m_screen = new vec3[SCRHEIGHT*SCRWIDTH];
	mouseX = 0;
	mouseY = 0;

	// initialization code goes here
}

// == Game::Draw2DView ========================================================
void Game::Draw2DView()
{
	screen->ClipTo( SCRWIDTH / 2 , 0 , SCRWIDTH - 1 , SCRHEIGHT - 1 );
	renderer.scene.Draw2D();
	renderer.camera.Draw2D();
}

// == Game::Init ==============================================================

void Game::MouseDown(int button)
{
	if (Input::GetKey(SDL_SCANCODE_H))
	{
		renderer.UpdateDoF(mouseX, mouseY);
		frame = 0;
	}
}
void Game::Tick( float dt )
{
	frame++;
	screen->Clear( 0 );	

	if( Input::GetKey( SDL_SCANCODE_A ) )
	{
		renderer.camera.Move( vec3( -moveSpeed , 0 , 0 ) );
		frame = 1;
	}
	else if( Input::GetKey( SDL_SCANCODE_D ) )
	{
		renderer.camera.Move(vec3(moveSpeed, 0, 0));
		frame = 1;
	}
	if( Input::GetKey( SDL_SCANCODE_W ) )
	{
		renderer.camera.Move(vec3(0, 0, moveSpeed));
		frame = 1;
	}
	else if( Input::GetKey( SDL_SCANCODE_S ) )
	{
		renderer.camera.Move(vec3(0, 0, -moveSpeed));
		frame = 1;
	}

	if (Input::GetKey(SDL_SCANCODE_SPACE))
	{
		renderer.camera.Move(vec3(0, -moveSpeed, 0));
		frame = 1;

	}
	else if (Input::GetKey(SDL_SCANCODE_LSHIFT))
	{
		renderer.camera.Move(vec3(0, moveSpeed, 0));
		frame = 1;
	}

	if (Input::GetKey(SDL_SCANCODE_Q))
	{
		renderer.camera.Rotate(-RotationSpeed);
		frame = 1;
	}
	else if (Input::GetKey(SDL_SCANCODE_E))
	{
		renderer.camera.Rotate(RotationSpeed);
		frame = 1;
	}
	//Draw2DView();
	Renderer* ptr = &renderer;
	int startY = 0;
	int endY = 0;
	int startX = 0;
	int endX = 0;
	int yStep = 128;
	int xStep = 128;
	int ss = 0;
	for( int y = 0; y < SCRHEIGHT; y+= yStep)
	{
		startY = y;
		endY = y + yStep;
		for( int x = 0; x < SCRWIDTH; x += xStep)
		{
			startX = x;
			endX = x + xStep;
			jobSystem->QueueJob( [=](){ptr->Render( startY , endY , startX , endX,frame ); } ); // std::function<void()>
			ss++;
			//printf("%d\n",ss );
			//printf( "Startx:%d\t EndX:%d\n StartY:%d\t EndY:%d\n" , startX , endX , startY , endY );
		}
	}
	//printf("%d\n", ss);
	jobSystem->WaitForJobsToFinish();

/*
	std::thread t1( &Renderer::Render  , &renderer, 0 , SCRHEIGHT / 2 , 0 , SCRWIDTH / 4 );
	std::thread t2( &Renderer::Render , &renderer , 0 , SCRHEIGHT / 2 , SCRWIDTH / 4 , SCRWIDTH / 2 );
	std::thread t3( &Renderer::Render , &renderer , SCRHEIGHT / 2 , SCRHEIGHT , 0 , SCRWIDTH / 4 );
	std::thread t4( &Renderer::Render , &renderer , SCRHEIGHT / 2 , SCRHEIGHT , SCRWIDTH / 4 , SCRWIDTH / 2 );
	t1.join();
	t2.join();
	t3.join();
	t4.join();*/
	//renderer.Render( 0 , SCRHEIGHT , 0 , SCRWIDTH/2 );
	/*for(int i = 0; i < SCRHEIGHT; i++ )
	{
		renderer.Render( i );	
		.
	}*/
	
	
	//printf( "%d\n" , ss );
	//printf( "Hello" );

}
