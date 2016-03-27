// Template for GP1, version 2
// IGAD/NHTV - Jacco Bikker - 2006-2014

#pragma once

#define EPSILON		0.0001f
#define SHADOWRAYSPERFRAM 4.0f
#define DofFactor 0.05f
#define SOFTSHADOW true
#include "glm\gtc\quaternion.hpp"
#include <vector>
#include <mutex>
class Light;

namespace Tmpl8 {
class Primitive;
// == class Material ==========================================================
//    --------------
class Material
{
public:
	Material( )
	{
		texture = nullptr;
	}
	~Material(){
		delete texture;
	}
	void SetColor( vec3 _color )
	{
		color = _color;
	}
	void setReflective( float _reflective )
	{
		reflective = _reflective;
	}
	void setSpecular(float _specular)
	{
		specular = _specular;
	}
	void setDiffuse( float _diffuse )
	{
		diffuse = _diffuse;
	}
	void setTransperancy( float _transperancy )
	{
		transperancy = _transperancy;
	}
	void setRefractionIndex( float _refractionIndex )
	{
		refractionIndex = _refractionIndex;
	}
	void setTexture(char* FileName)
	{
		texture = new Surface(FileName);
	}
	
	vec3 color;						// color of material
	float reflective;
	float specular;
	float diffuse;
	float transperancy;
	float refractionIndex;
	Surface* texture;
};

class Light
{
public:
	float m_range;
	float m_intensity;
	vec3 m_color;
	vec3 m_position;
	float radius;
	bool AreaLight;
};

class PointLight : public Light
{
public:
	PointLight(vec3 pos, vec3 color, float intensity, float Range);


};


// == class Intersection ======================================================
//    ------------------
class Intersection
{
public:
	Intersection()
	{
		prim = nullptr;
		Mat = nullptr;
		inside = false;
	}
	vec3 N;							// normal at intersection point
	vec3 Pos;						// position of the intersecting primitive
	vec3 Color;
	float result;
	Material* Mat;
	Primitive* prim;
	bool inside; 
};



// == class Ray ===============================================================
//    ---------
class Ray
{
public:
	// methods
	Ray( vec3 origin , vec3 direction , float length,bool _shadow = false ) :O( origin ) , D( direction ) , t(length )
	{
		shadow = _shadow;
	}
	Ray()
	{
	};
	void Draw2D(int color);
	// data members
	vec3 O;							// ray origin
	vec3 D;							// normalized ray direction
	float t;						// distance of intersection along ray ( >= 0 )
	Intersection intersection;		// data for closest intersection point
	bool shadow;

};

// == class Primitive =========================================================
//    ---------------
class Primitive
{
public:
	Primitive(){ light = nullptr; }
	virtual void Intersect( Ray& _Ray ) = 0;
	virtual void Draw2D() = 0;
	Material* material;	
	Light* light;
};

// == class Plane =============================================================
//    -----------

class AABB : public Primitive
{
public:
	AABB();
	AABB(vec3 _center, float _xWidth, float _yWidth, float _zWidth);
	void Intersect(Ray& _ray);
	void Draw2D();
	vec3 widths;
	vec3 center;
	vec3 Corners[8];
private:
	void CalcCorners();
};

class Plane : public Primitive
{
public:
	// constructors
	Plane() {}
	Plane(vec3& _N, vec3& _tangentToN, float _Dist) : N(_N), dist(_Dist), Uaxis(_tangentToN)
	{
		material = new Material( );
		//Uaxis = glm::normalize(vec3(-_N.y, _N.x, _N.z));
		Vaxis = glm::normalize(glm::cross(_N, Uaxis));
		//Uaxis = glm::normalize(glm::cross(_N, Vaxis));
	}
	Plane( vec3 _V0, vec3 _V1, vec3 _V2 );
	// methods
	void Intersect( Ray& _Ray );
	void Draw2D();
	// data members
	vec3 N;							// plane normal
	vec3 Uaxis;
	vec3 Vaxis;
	float dist;						// distance of plane from (0,0,0)
};

// == class Sphere ============================================================
//    ------------

class Sphere : public Primitive
{
public:
	// constructors
	Sphere() {}
	Sphere( vec3 _Pos, float _Radius )
		: P( _Pos ), r( _Radius )
	{
		material = new Material();
	}
	// methods
	void Intersect( Ray& _Ray );
	void Draw2D();
	// data members
	vec3 P;							// centre of sphere
	float r;						// sphere radius
	
};



// == class Triangle ==========================================================
//    --------------
class Triangle : public Primitive
{
public:
	// constructors
	Triangle() {}
	Triangle( vec3 _V0, vec3 _V1, vec3 _V2 );
	// methods
	void Intersect( Ray& _Ray );
	void Draw2D();
	// data members
	vec3 v0, v1, v2;				// triangle vertices
	vec3 N;							// triangle normal
};

// == class Camera ============================================================
//    ------------
class Camera
{
public:
	// methods
	Ray GenerateRay( int _X, int _Y, bool mouseClick );
	void Set( vec3 _Pos, vec3 _Direction );
	void Move( vec3 _Move );
	void Rotate( float _Rotation );
	void Draw2D();
	void Update();
	// data members
	float Rotation;
	quat quatRotation;
	vec3 eyePos;					// camera position
	vec3 V;							// normalized view direction
	vec3 p1 , p2 , p3 , p4;			// corners of screen plane
	float focus;
};

// == class Scene =============================================================
//    -----------
class Scene
{
public:
	// constructor
	Scene();
	void Draw2D();
	void Intersect( Ray& _Ray );
	// data members
	Primitive** primList;
	Light** m_lightList;
	int primCount, m_lightCount;
};

// == class Renderer ==========================================================
//    --------------
class Renderer
{
public:
	// constructor
	Renderer();
	// methods
	enum
	{
		MAX_RECCURSION = 5

  };
	vec3 Trace(Ray& _Ray, bool _renderRay , int depth = 0, float _refractionIndex = 1.0f);
	void Render(int StartY, int EndY, int StartX, int Endx, int& frames);
	void UpdateDoF(int x, int y)
	{
		Ray ray = camera.GenerateRay(x, y, true);
		scene.Intersect(ray);
		camera.focus = (dot(ray.D*(ray.t), camera.V));
		if (dot(ray.D, camera.V) < 0)
		{
			camera.focus *= -1;
		}
		camera.Update();
	}
	void Render2D();
	// data members
	vec3 lastPos;
	float lastRotation;
	Scene scene;
	Camera camera;
	vec3* m_screen;
	
};

};

// EOF