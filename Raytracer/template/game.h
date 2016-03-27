// Template for GP1, version 2
// IGAD/NHTV - Jacco Bikker - 2006-2014

#pragma once
class ThreadJobSystem;
namespace Tmpl8 {

#define SCRWIDTH	512
#define SCRHEIGHT	512

class Surface;
class Game
{
public:
	void SetTarget( Surface* a_Surface ) { screen = a_Surface; }
	void Init();
	void Draw2DView();
	void Tick( float dt );
	void Shutdown() { /* implement if you want code to be executed upon app exit */ };
	void MouseUp( int button ) { /* implement if you want code to be executed upon app exit */ };
	void MouseDown(int button);
	void MouseMove(int x, unsigned int y) { mouseX = x; mouseY = y; };
	void KeyUp( int code )
	{
	}
	void KeyDown( int code )
	{
	}
private:
	Surface* screen;
	ThreadJobSystem* jobSystem;
	float mouseX; 
	float mouseY;
};


}; // namespace Tmpl8