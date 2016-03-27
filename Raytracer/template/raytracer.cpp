// Template for GP1, version 2
// IGAD/NHTV - Jacco Bikker - 2006-2014
#pragma once
#include "template.h"
#include "surface.h"
#include <random>
using namespace Tmpl8;
using namespace glm;

#include "glm\gtc\matrix_transform.hpp"
#include <cmath>
#include "raytracer.h"
#include "game.h"

extern Surface* screen; // defined in template.cpp

// == HELPER FUNCTIONS ========================================================
//    ----------------

// visible world space definition
#define WXMIN	-20.0f// left edge of visible world
#define WYMIN	-20.0f// top edge of visible world
#define WXMAX	 20.0f// right edge of visible world
#define WYMAX	 20.0f	// bottom edge of visible world

// world space to screen space conversion
#define SX(x) ((((x)-(WXMIN))/((WXMAX)-(WXMIN)))*(SCRWIDTH/2)+SCRWIDTH/2)
#define SY(y) ((((-y)-(WYMIN))/((WYMAX)-(WYMIN)))*SCRHEIGHT) // trust me
std::mt19937 generator(rand() % 195892);
std::uniform_real_distribution<float> floatGen;
const vec3 BackGroundColor(0.2f, .0f, .1f);
// draw a line using world space coordinates
void Line2D(float x1, float y1, float x2, float y2, unsigned int c)
{
	// convert world space coordinates to screen space and draw line
	x1 = SX(x1), y1 = SY(y1), x2 = SX(x2), y2 = SY(y2);
	screen->ClipTo(SCRWIDTH / 2 + 2, 0, SCRWIDTH - 1, SCRHEIGHT - 1);
	screen->Line(x1, y1, x2, y2, c);
}

// plot a pixel using world space coordinates
void Plot2D(float x1, float y1, unsigned int c)
{
	// convert world space coordinates to screen space and plot
	screen->Plot((int)SX(x1), (int)SY(y1), c);
}

// convert a color from float to 32-bit integer RGB
unsigned int ConvertColor(vec3& _Color)
{
	// convert a vec3 color to 32-bit integer
	int red = (int)(255.0f * min(1.0f, _Color.x));
	int green = (int)(255.0f * min(1.0f, _Color.y));
	int blue = (int)(255.0f * min(1.0f, _Color.z));
	return (red << 16) + (green << 8) + blue;
}
void UnpackColor(unsigned long _color, vec3& color)
{
	color.x = ((_color & 0xff0000) >> 16) / 255.0f;
	color.y = ((_color & 0x00ff00) >> 8) / 255.0f;
	color.z = (_color & 0x0000ff) / 255.0f;
}

vec3 RandomVec(std::mt19937& generator)
{
	static const std::uniform_real_distribution<float> distr(-1, 1);
	vec3 v;
	do
	{
		v = vec3(distr(generator), distr(generator), distr(generator));
	} while (glm::dot(v, v) > 1);
	return glm::normalize(v);
}


PointLight::PointLight(vec3 pos, vec3 color, float inensity, float Range)
{
	m_position = pos;
	m_range = Range;
	m_color = color;
	m_intensity = m_intensity;
}


// == Ray class ===============================================================
//    ---------
void Ray::Draw2D(int color)
{
	screen->ClipTo(SCRWIDTH / 2, 0, SCRWIDTH - 1, SCRHEIGHT - 1);
	//Line2D( O.x , O.z , O.x + 10000 * D.x , O.z + 10000 + D.z , 0x111111 );
	//Line2D( O.x , O.z , O.x + D.x , O.z + D.z , 0x666666 );
	Line2D(O.x, O.z, O.x + t * D.x, O.z + t * D.z, color);
	Plot2D(O.x + t * D.x, O.z + t * D.z, 0xff0000);

	// reset clip window
	screen->ClipTo(0, 0, SCRWIDTH - 1, SCRHEIGHT - 1);
}

// == Plane class =============================================================
//    -----------

AABB::AABB(vec3 _center, float _xWidth, float _yWidth, float _zWidth)
{
	center = _center;
	widths.x = _xWidth / 2;
	widths.y = _yWidth / 2;
	widths.z = _zWidth / 2;
	CalcCorners();
	material = new Material();
}
void AABB::CalcCorners()
{

	Corners[0] = vec3(center.x - widths.x, center.y + widths.y, center.z - widths.z);
	Corners[1] = vec3(center.x - widths.x, center.y - widths.y, center.z - widths.z);
	Corners[2] = vec3(center.x + widths.x, center.y + widths.y, center.z - widths.z);
	Corners[3] = vec3(center.x + widths.x, center.y - widths.y, center.z - widths.z);

	Corners[4] = vec3(center.x - widths.x, center.y + widths.y, center.z + widths.z);
	Corners[5] = vec3(center.x - widths.x, center.y - widths.y, center.z + widths.z);
	Corners[6] = vec3(center.x + widths.x, center.y + widths.y, center.z + widths.z);
	Corners[7] = vec3(center.x + widths.x, center.y - widths.y, center.z + widths.z);
}

void AABB::Intersect(Ray& _ray)
{
	_ray.intersection.Mat = material;
}

void AABB::Draw2D()
{
	Line2D(center.x + widths.x, center.z + widths.z, center.x + widths.x, center.y - widths.y, ConvertColor(material->color));
	Line2D(center.x - widths.x, center.z + widths.z, center.x - widths.x, center.y - widths.y, ConvertColor(material->color));
	Line2D(center.x + widths.x, center.z - widths.z, center.x - widths.x, center.y - widths.y, ConvertColor(material->color));
	Line2D(center.x + widths.x, center.z + widths.z, center.x - widths.x, center.y + widths.y, ConvertColor(material->color));
}

Plane::Plane(vec3 _V0, vec3 _V1, vec3 _V2)
{
	// construct an infinite plane based on three points
	const vec3 e1 = _V1 - _V0, e2 = _V2 - _V0;
	N = normalize(cross(e1, e2));
	dist = dot(_V0, N);
	material = 0;
}

void Plane::Intersect(Ray& _Ray)
{
	// find the intersection of a ray and an infinite plane
	float t = (-dot(_Ray.O, N) + dist) / dot(_Ray.D, N);
	// store distance in ray if smaller than previous found distance
	if ((t < _Ray.t) && (t >= 0))
	{
		_Ray.t = t;
		_Ray.intersection.N = N;
		_Ray.intersection.Pos = _Ray.O + _Ray.t * _Ray.D;
		_Ray.intersection.Mat = material;
		_Ray.intersection.prim = this;
		if (material->texture != nullptr)
		{
			if (Uaxis == Vaxis)
			{
				_Ray.intersection.Color = vec3(1, 0, 1);
				return;
			}
			float u, v;
			u = glm::dot(_Ray.intersection.Pos, Uaxis);
			v = glm::dot(_Ray.intersection.Pos, Vaxis);
			vec3 pos = _Ray.O + u * Uaxis + v * Vaxis;
			int tempu = (int)(u * 500);
			int tempv = (int)(v * 500);
			if (tempu == INT_MIN)
			{
				_Ray.intersection.Color = vec3(1, 0, 1);
				return;
			}
			if (tempv == INT_MAX)
			{
				_Ray.intersection.Color = vec3(1, 0, 1);
				return;
			}
			unsigned int temptempu = std::abs(tempu);
			unsigned int temptempv = std::abs(tempv);
			unsigned int u1 = std::abs(int(temptempu)) % material->texture->GetWidth();
			unsigned int v1 = std::abs(int(temptempv)) % material->texture->GetHeight();
			//printf("u1 %d\t v1%d\n", u1, v1);
			UnpackColor(material->texture->GetBuffer()[u1 + v1*material->texture->GetPitch()], _Ray.intersection.Color);
		}
		else
			_Ray.intersection.Color = material->color;

	}
}

void Plane::Draw2D()
{
	// visualize plane
	if (fabs(N.y) < EPSILON) // we can only draw a line for vertical planes
	{
		// get a point on the plane
		vec3 p0 = N * dist;
		// determine vector perpendicular to N (and thus parallel to the plane)
		vec3 B(-N.z, 0.0f, N.x);
		// calculate two points on the plane, far apart
		vec3 p1 = p0 + B * 10000.0f;
		vec3 p2 = p0 - B * 10000.0f;
		// connect these points with a line
		Line2D(p1.x, p1.z, p2.x, p2.z, 0xffffff);

	}
}

// == Sphere class ============================================================
//    ------------
void Sphere::Intersect(Ray& _Ray)
{
	bool inside = false;
	// find the intersection of a ray and a sphere
	// http://www.ccs.neu.edu/home/fell/CSU540/programs/RayTracingFormulas.htm
	float t = 1e34;
	float t2;
	float b = 2.0f * dot(_Ray.D, _Ray.O - P);
	float c = dot(P, P) + dot(_Ray.O, _Ray.O) - 2.0f * dot(P, _Ray.O) - r * r;
	float d = b * b - 4 * c;
	bool inverted = false;
	if (d >= 0)
	{
		float sqrD = sqrtf(d);
		float t1 = (-b - sqrD) / 2;
		t2 = (-b + sqrD) / 2;

		if ((t1 < t) && (t1 >= 0))
			t = t1;
		if ((t2 < t) && (t2 >= 0))
		{
			inside = true;
			t = t2;
		}
	}
	// store distance in ray if smaller than previous found distance
	if (t < _Ray.t)
	{

		_Ray.t = t;
		vec3 pointOnSphere = _Ray.O + _Ray.D * _Ray.t;
		_Ray.intersection.N = (glm::normalize(pointOnSphere - P));
		if (inside)
		{
			_Ray.intersection.N *= -1;
			_Ray.intersection.inside = true;
		}
		_Ray.intersection.Pos = pointOnSphere;
		_Ray.intersection.Mat = material;
		_Ray.intersection.Color = material->color;
		_Ray.intersection.prim = this;
	}
}

void Sphere::Draw2D()
{
	// visualize a sphere by drawing 72 line segments around the center
	float x1 = r * sinf(0) + P.x, y1 = r * cosf(0) + P.z, x2, y2;
	for (int i = 1; i <= 72; i++, x1 = x2, y1 = y2)
	{
		float degrees = i * 5.0f, radians = degrees * PI / 180;
		x2 = r * sinf(radians) + P.x, y2 = r * cosf(radians) + P.z;
		Line2D(x1, y1, x2, y2, 0xffffff);
	}
}

// == Triangle class ==========================================================
//    --------------
Triangle::Triangle(vec3 _V0, vec3 _V1, vec3 _V2) : v0(_V0), v1(_V1), v2(_V2)
{
	// set triangle vertices and calculate its normal
	const vec3 e1 = v1 - v0, e2 = v2 - v0;
	N = normalize(cross(e1, e2));
	material = 0;
}

void Triangle::Intersect(Ray& _Ray)
{
	// find the intersection of a ray and a triangle
	float t = 0; // todo: find intersection distance
	if (t < _Ray.t)
	{
		_Ray.t = t;
		_Ray.intersection.N = N;
	}
}

void Triangle::Draw2D()
{
	// draw triangle edges											
	Line2D(v0.x, v0.z, v1.x, v1.z, 0xffffff);
	Line2D(v1.x, v1.z, v2.x, v2.z, 0xffffff);
	Line2D(v2.x, v2.z, v0.x, v0.z, 0xffffff);
}

// == Camera class ============================================================
//    ------------
void Camera::Draw2D()
{
	// draw view frustum
	Line2D(eyePos.x, eyePos.z, p1.x, p1.z, 0xaa);
	Line2D(eyePos.x, eyePos.z, p2.x, p2.z, 0xaa);
	Line2D(p1.x, p1.z, p2.x, p2.z, 0xFF00FF);
}

void Camera::Set(vec3 _Pos, vec3 _Direction)
{
	// set position and view direction, then calculate screen corners
	focus = 2.0f;
	eyePos = _Pos;
	V = _Direction;
	p1 = (vec3(-0.5f, -0.5f, -2.2f) + eyePos)* quatRotation; // todo: calculate screen corners using P and V
	p2 = (vec3(0.5f, -0.5f, -2.2f) + eyePos*_Direction)* quatRotation; // todo: calculate screen corners using P and V
	p3 = (vec3(0.5f, 0.5f, -2.2f) + eyePos*_Direction)* quatRotation; // todo: calculate screen corners using P and V
	p4 = (vec3(-0.5f, 0.5f, -2.2f) + eyePos*_Direction)*quatRotation; // todo: ca lculate screen corners using P and V
}

void Camera::Rotate(float _Rotation)
{
	Rotation += _Rotation;

	quatRotation = glm::quat(cos(Rotation / 2), 0, sin(Rotation / 2), 0);
	Update();
	V = vec3(0.0f,0.0f,1.0f) * quatRotation;

}

void Camera::Update()
{
	float minRes = (float)min(SCRWIDTH, SCRHEIGHT);
	float minX = -0.5f * ((float)SCRWIDTH / minRes);
	float maxX = 00.5f * ((float)SCRWIDTH / minRes);
	float minY = 0.5f * ((float)SCRHEIGHT / minRes);
	float maxY = -00.5f * ((float)SCRHEIGHT / minRes);

	p1 = eyePos + quatRotation * (vec3(minX, maxY, 0.8f) * focus);
	p2 = eyePos + quatRotation * (vec3(maxX, maxY, 0.8f) * focus);
	p3 = eyePos + quatRotation * (vec3(maxX, minY, 0.8f) * focus);
	p4 = eyePos + quatRotation * (vec3(minX, minY, 0.8f) * focus);
	//V = vec3(0.0f, 0.0f, 1.0f) * quatRotation;
}

void Camera::Move(vec3 _Move)
{
	vec3 newMove = quatRotation * _Move;
	eyePos += newMove;
	p1 += newMove;
	p2 += newMove;
	p3 += newMove;
	p4 += newMove;
}

Ray Camera::GenerateRay(int _X, int _Y, bool mouseClick)
{
	vec3 DoF(0,0,0);
	float dff = DofFactor;
	float fx, fy;
	// calculate position on virtual screen plane floating in front of camera
	if (!mouseClick)
	{
		DoF = (quatRotation * vec3(floatGen(generator) * dff - dff / 2.0f, floatGen(generator)* dff - dff / 2, 0)); // credit Ruben Rutten for assistance on this formula
		fx = ((float)_X + floatGen(generator)) / (SCRWIDTH), fy = ((float)_Y + floatGen(generator)) / SCRHEIGHT;
	}
	else
	{
		fx = (float)_X / (SCRWIDTH), fy = (float)_Y / SCRHEIGHT;
	}
		vec3 P = p1 + (p2 - p1) * fx + (p4 - p1) * fy;
	// generate a ray starting at the camera, going through a pixel
	Ray ray;
	ray.shadow = false;
	ray.O = eyePos + DoF;
	ray.D = normalize(P - eyePos);
	ray.t = 1e34;
	return ray;
}

// == Scene class =============================================================
//    -----------
Scene::Scene()
{
	// scene initialization
	primList = new Primitive*[128]; // todo: this is going to bite us one day
	m_lightList = new Light*[2];
	m_lightCount = 2;
	primCount = 0;
#pragma region Lights
#pragma region LightOne
	m_lightList[0] = new PointLight(vec3(-2.5f, -3.0f, -2.5f), vec3(1.0f, 1.0f, 1.00f), 10.0f, 20.0f);
	m_lightList[0]->AreaLight = SOFTSHADOW;
	m_lightList[0]->radius = 0.5;
	primList[primCount] = new Sphere(vec3(-2.5f, -3.0f, -2.5f), 0.5);
	primList[primCount]->material->SetColor(vec3(1.0f, 1.0f, 1.00f));
	primList[primCount]->material->setDiffuse(0.0f);
	primList[primCount]->material->setSpecular(0);
	primList[primCount]->material->setReflective(1.0);
	primList[primCount]->material->setTransperancy(0.0);
	primList[primCount]->light = m_lightList[0];
	primList[primCount++]->material->setRefractionIndex(0.0);
#pragma endregion LightOne
#pragma region LightTwo
	m_lightList[1] = new PointLight(vec3(6.0f, -2.0f, -3.0f), vec3(1.0f, 1.0f, 1.0f), 10.0f, 10.0f);
	m_lightList[1]->AreaLight = SOFTSHADOW;
	m_lightList[1]->radius = 0.5;
	primList[primCount] = new Sphere(vec3(6.0f, -2.0f, -3.0f), 0.5);
	primList[primCount]->material->SetColor(vec3(1.0f, 1.0f, 1.00f));
	primList[primCount]->material->setDiffuse(0.0f);
	primList[primCount]->material->setSpecular(0);
	primList[primCount]->material->setReflective(1.0);
	primList[primCount]->material->setTransperancy(0.0);
	primList[primCount]->light = m_lightList[1];
	primList[primCount++]->material->setRefractionIndex(0.0);
#pragma endregion LightTwo
#pragma endregion Lights
#pragma region Spheres
#pragma region refractive Sphere
	primList[primCount] = new Sphere(vec3(0.0f,-0.5 , -5.0f), 1);
	primList[primCount]->material->SetColor(vec3(0.7f, 0.7f, 0.7f));
	primList[primCount]->material->setDiffuse(0.0f);
	primList[primCount]->material->setSpecular(0.0f);
	primList[primCount]->material->setReflective(0.4);
	primList[primCount]->material->setTransperancy(0.0f);
	primList[primCount++]->material->setRefractionIndex(1.6);	//refraction sphere
#pragma endregion Refractive Sphere
#pragma region Reflective Sphere
	primList[primCount] = new Sphere(vec3(-2.5f, -0.5f, -2.5f), 1);
	primList[primCount]->material->SetColor(vec3(0.7f, 0.7f, 0.7f));
	primList[primCount]->material->setDiffuse(0.0f);
	primList[primCount]->material->setSpecular(0);
	primList[primCount]->material->setReflective(1.0);
	primList[primCount]->material->setTransperancy(0.0);
	primList[primCount++]->material->setRefractionIndex(0.0);
#pragma endregion ReflectiveSphere
#pragma region RedSphere
	primList[primCount] = new Sphere(vec3(0.f, -0.5, 0), 1);//, 0.9f, 0.01f, 1.0f, 0.5f, 0, 1.33f );
	primList[primCount]->material->SetColor(vec3(1.0f, 0.0f, 0.0f));
	primList[primCount]->material->setDiffuse(1);
	primList[primCount]->material->setSpecular(0);
	primList[primCount]->material->setReflective(0.0);
	primList[primCount]->material->setTransperancy(0.0f);
	primList[primCount++]->material->setRefractionIndex(0);
#pragma endregion RedShpere
#pragma region YellowSphere
	primList[primCount] = new Sphere(vec3(-5.0f, -0.5, 0), 1);//, 0.9f, 0.01f, 1.0f, 0.5f, 0, 1.33f );
	primList[primCount]->material->SetColor(vec3(.5f, .5f, 0.0f));
	primList[primCount]->material->setDiffuse(1);
	primList[primCount]->material->setSpecular(0);
	primList[primCount]->material->setReflective(0);
	primList[primCount]->material->setTransperancy(0.0f);
	primList[primCount++]->material->setRefractionIndex(0);
#pragma endregion YellowSPhere
#pragma region GreenSphere
	primList[primCount] = new Sphere(vec3(-5.0f, -0.5, -5.0f), 1);//, 0.9f, 0.01f, 1.0f, 0.5f, 0, 1.33f );
	primList[primCount]->material->SetColor(vec3(.3f, .5f, 0.3f));
	primList[primCount]->material->setDiffuse(1);
	primList[primCount]->material->setSpecular(0);
	primList[primCount]->material->setReflective(0);
	primList[primCount]->material->setTransperancy(0.0f);
	primList[primCount++]->material->setRefractionIndex(0);
#pragma endregion GreenSphere
#pragma endregion Spheres
#pragma region Planes

#pragma region FloorPlane
	primList[primCount] = new Plane(vec3(0., -1, 0), vec3(1,0,0), -0.7f);//, 0.9f , 0.01f , 0.f , 0.0f , 0 , 1.0f );
	primList[primCount]->material->SetColor(vec3(1.0f, 1.0f, 1.f));
	primList[primCount]->material->setDiffuse(1.0f);
	primList[primCount]->material->setSpecular(0.0f);
	primList[primCount]->material->setReflective(0.0f);
	primList[primCount]->material->setTransperancy(0.0f);
	primList[primCount]->material->setTexture("assets/image1.jpg");
	primList[primCount++]->material->setRefractionIndex(0.0);
#pragma endregion FloorPlane
#pragma region FrontPlane
	primList[primCount] = new Plane(vec3(0, 0, -1), vec3(0, 1, 0), -1.5f);// , 0.9f, 0.01f, 0.f, 0.0f, 0, 1.0f );
	primList[primCount]->material->SetColor(vec3(0.0f, 0.0f, 1.0f));
	primList[primCount]->material->setDiffuse(1.0f);
	primList[primCount]->material->setSpecular(0.0f);
	primList[primCount]->material->setReflective(0.0f);
	primList[primCount]->material->setTransperancy(0.0f);
	//primList[primCount]->material->setTexture("assets/image1.jpg");
	primList[primCount++]->material->setRefractionIndex(0);
#pragma endregion FrontPlane
#pragma region BackPlane
	primList[primCount] = new Plane(vec3(0, 0, 1), vec3(0, 1, 0), -10.0f);// , 0.9f, 0.01f, 0.f, 0.0f, 0, 1.0f );
	primList[primCount]->material->SetColor(vec3(0.f, 0.2f, .5f));
	primList[primCount]->material->setDiffuse(1.0f);
	primList[primCount]->material->setSpecular(0.0f);
	primList[primCount]->material->setReflective(0.0f);
	primList[primCount]->material->setTransperancy(0.0f);
	primList[primCount]->material->setTexture("assets/image1.jpg");
	primList[primCount++]->material->setRefractionIndex(0);
#pragma endregion BackPlane
#pragma region LeftPlane
	primList[primCount] = new Plane(vec3(1, 0, 0), vec3(0, 0, 1), -10.0f);// , 0.9f, 0.01f, 0.f, 0.0f, 0, 1.0f );
	primList[primCount]->material->SetColor(vec3(0.f, 0.2f, .5f));
	primList[primCount]->material->setDiffuse(1.0);
	primList[primCount]->material->setSpecular(0.0f);
	primList[primCount]->material->setReflective(0.0f);
	primList[primCount]->material->setTransperancy(0.0f);
	//primList[primCount]->material->setTexture("assets/image1.jpg");
	primList[primCount++]->material->setRefractionIndex(0);
#pragma endregion LeftPlane
#pragma region RightPlane
	primList[primCount] = new Plane(vec3(-1, 0, 0), vec3(1, 0, 1), -10.0f);// , 0.9f, 0.01f, 0.f, 0.0f, 0, 1.0f );
	primList[primCount]->material->SetColor(vec3(0.0f, 0.2f, .5f));
	primList[primCount]->material->setDiffuse(1.0);
	primList[primCount]->material->setSpecular(0.0f);
	primList[primCount]->material->setReflective(0.0f);
	primList[primCount]->material->setTransperancy(0.0f);
	//primList[primCount]->material->setTexture("assets/image1.jpg");
	primList[primCount++]->material->setRefractionIndex(0);
#pragma endregion RightPlane
#pragma region RoofPlane
	primList[primCount] = new Plane(vec3(0, 1, 0), vec3(1, 0, 0), -10.0f);// , 0.9f, 0.01f, 0.f, 0.0f, 0, 1.0f );
	primList[primCount]->material->SetColor(vec3(0.f, 0.2f, .5f));
	primList[primCount]->material->setDiffuse(1.0f);
	primList[primCount]->material->setSpecular(0.0f);
	primList[primCount]->material->setReflective(0.0f);
	primList[primCount]->material->setTransperancy(0.0f);
	//primList[primCount]->material->setTexture("assets/image1.jpg");
	primList[primCount++]->material->setRefractionIndex(0);
#pragma endregion RoofPlane
#pragma endregion Planes



}

void Scene::Draw2D()
{
	// draw arrows
	const int cx = SCRWIDTH / 2, w = SCRWIDTH / 2, y2 = SCRHEIGHT - 1;
	screen->ClipTo(0, 0, SCRWIDTH - 1, SCRHEIGHT - 1);
	screen->Line(cx, 0, cx, y2, 0xffffff);
	screen->Line(cx + 9, y2 - 6, cx + 9, y2 - 40, 0xffffff);
	screen->Line(cx + 9, y2 - 40, cx + 6, y2 - 37, 0xffffff);
	screen->Line(cx + 9, y2 - 40, cx + 12, y2 - 37, 0xffffff);
	screen->Line(cx + 9, y2 - 6, cx + 43, y2 - 6, 0xffffff);
	screen->Line(cx + 43, y2 - 6, cx + 40, y2 - 3, 0xffffff);
	screen->Line(cx + 43, y2 - 6, cx + 40, y2 - 9, 0xffffff);
	screen->Print("z", cx + 7, y2 - 50, 0xffffff);
	screen->Print("x", cx + 48, y2 - 8, 0xffffff);
	// 2D viewport
	Line2D(0, -10000, 0, 10000, 0x004400);
	Line2D(-10000, 0, 10000, 0, 0x004400);
	char t[128];
	sprintf(t, "%4.1f", WXMIN);
	screen->Print(t, SCRWIDTH / 2 + 2, (int)SY(0) - 7, 0x006600);
	sprintf(t, "%4.1f", WXMAX);
	screen->Print(t, SCRWIDTH - 30, (int)SY(0) - 7, 0x006600);
	sprintf(t, "%4.1f", WYMAX);
	screen->Print(t, (int)SX(0) + 2, 2, 0x006600);
	sprintf(t, "%4.1f", WYMIN);
	screen->Print(t, (int)SX(0) + 2, SCRHEIGHT - 8, 0x006600);
	for (int i = 0; i < primCount; i++) primList[i]->Draw2D();
}

void Scene::Intersect(Ray& _Ray)
{
	// intersect ray with each primitive in the scene
	if (!_Ray.shadow)
		for (int i = 0; i < primCount; i++)
		{
			if (primList[i]->light == nullptr)
				primList[i]->Intersect(_Ray);
			else if (primList[i]->light->AreaLight)
				primList[i]->Intersect(_Ray);
		}
	else
		for (int i = 0; i < primCount; i++)
		{
		if (primList[i]->light == nullptr)
			primList[i]->Intersect(_Ray);
		}
	

}

// == Renderer class ==========================================================
//    --------------
Renderer::Renderer()
{
	camera.Set(vec3(0, 0, -8), vec3(0, 0, 1));
	lastPos = camera.eyePos;
	lastRotation = camera.Rotation;
}

vec3 Renderer::Trace(Ray& _Ray, bool _renderRay, int depth, float _refractionIndex)
{
	// intersect a single ray with the objects in the scene
	scene.Intersect(_Ray);

	if (_Ray.intersection.prim == nullptr)
		return BackGroundColor;
	if (_Ray.intersection.prim->light != nullptr)
		return _Ray.intersection.prim->light->m_color;

	Material* mat = _Ray.intersection.Mat;
	Intersection* icolor = &_Ray.intersection;
	// get the distance of the nearest intersection
	float distance = _Ray.t;

	vec3 N = _Ray.intersection.N;
	vec3 RayPos = _Ray.intersection.Pos;
	vec3 D = _Ray.D;
	vec3 color(0, 0, 0);
	float kd = _Ray.intersection.Mat->diffuse;


	//	reflection
	float wRef = 0;
#pragma region Reflection+Refraction
	float transperancyValue = _Ray.intersection.Mat->transperancy;
	float refractionValue = _Ray.intersection.Mat->refractionIndex;
	float reflectionValue = _Ray.intersection.Mat->reflective;
#pragma region Refraction
	if (refractionValue > 0.0f)
	{
		if (depth < MAX_RECCURSION)
		{
			float nnt;
			if (_Ray.intersection.inside)
				nnt = _refractionIndex / 1.0f;
			else
				nnt = _refractionIndex / refractionValue;
			float ddn = glm::dot(N, D);
			float cos2t = 1.0f - (nnt * nnt) * (1.0f - (ddn * ddn));
			float r0 = 0;
			if (_Ray.intersection.inside)
				r0 = powf(((nnt - 1.0f) / (nnt + 1.0f)), 2);
			else
				r0 = powf(((1.0f - nnt) / (1.0f + nnt)), 2);
			wRef = r0 + (1.0f - r0) * powf(1.0f + ddn, 5);
			float wRefract = (1.0f - wRef);
			if (cos2t > 0.0f)
			{
				//vec3 refractionDirection = glm::normalize(D * nnt - (ddn * nnt + std::sqrt(cos2t) * N));
				vec3 refractionDirection = glm::normalize(nnt * D - (nnt*ddn + sqrtf(cos2t)) *N);
				Ray ray(RayPos + refractionDirection*EPSILON, refractionDirection, 1e34);

				vec3 Tracedcolor = Trace(ray, _renderRay, depth + 1, refractionValue) * wRefract;
				vec3 finalColor(1, 1, 1);
				if (_Ray.intersection.inside)
				{
					vec3 Asorabance = ray.intersection.Color * 0.02f * -ray.t;
					finalColor = vec3(exp(Asorabance.r),
						exp(Asorabance.g),
						exp(Asorabance.b));
					
				}
				color += Tracedcolor*finalColor;
				//if (_renderRay)ray.Draw2D(0x00aa00);
			}

		}
#pragma endregion refraction
#pragma region reflection
		if (wRef > 0.0f && depth < MAX_RECCURSION)
		{
			vec3 reflect = glm::reflect(D, N);
			Ray ray(RayPos + reflect * 0.001f, reflect, 1e34);
			color += Trace(ray, _renderRay, depth + 1) * wRef;
			if (_renderRay)ray.Draw2D(0x0000aa);
		}
#pragma endregion reflection
	}

	//reflection for non dialetric surfaces
	else if (reflectionValue > 0.0f  && depth < MAX_RECCURSION )
	{
		vec3 reflect = glm::reflect(D, N);
		Ray ray(RayPos + reflect * 0.001f, reflect, 1e34);
		color += Trace(ray, _renderRay, depth + 1) * reflectionValue;
		if (_renderRay)ray.Draw2D(0x0000aa);
	}
	//if (color == vec3(0,0,0))
		//printf("color %f \t %f \t %f\n", color.x, color.y, color.z);
#pragma endregion Reflection and refraction code


	vec3 shadowColor;
	shadowColor.x = 0;
	shadowColor.y = 0;
	shadowColor.z = 0;
	vec3 intersection = _Ray.intersection.Pos;
	for (int i = 0; i < scene.m_lightCount; i++)
	{
		Light* light = scene.m_lightList[i];
		vec3 position = light->m_position;
		vec3 diffuse(0,0,0);

		float lightRange = scene.m_lightList[i]->m_range;

#pragma region diffuseSpecular
		kd = _Ray.intersection.Mat->diffuse;
		
		//diffuse += kd * factor * icolor->Color * light->m_color;
		//shadows
		
#pragma endregion 
		if (_Ray.intersection.Mat->transperancy == 0.0f)
		{
			if (!light->AreaLight)
			{
				vec3 lightVector = ((scene.m_lightList[i]->m_position) - _Ray.intersection.Pos);
				float lengthSqrd = glm::dot(lightVector, lightVector);
				float length = glm::sqrt(lengthSqrd);
				vec3 unitVector = glm::normalize(lightVector);
				float factor = glm::dot(N, unitVector);
				float distanceAtin = (lightRange / lengthSqrd);
				factor = clamp(factor, 0.0f, 1.0f);
				Ray shadowRay(_Ray.intersection.Pos + 0.01f*unitVector, unitVector, 1e34,true);
				factor *= distanceAtin;
				scene.Intersect(shadowRay);
				if (shadowRay.t >= length )
				{
					color += kd * factor * light->m_color;
				}
			}
			else
			{
				for (int z = 0; z < SHADOWRAYSPERFRAM; z++)
				{
					vec3 offset = RandomVec(generator);
					vec3 PosOnLight = (scene.m_lightList[i]->m_position + offset*light->radius);
					vec3 lightVector = ( PosOnLight- _Ray.intersection.Pos);

					float lengthSqrd = glm::dot(lightVector, lightVector);
					float length = glm::length(lightVector);
#pragma region NdotLAttenuation
					vec3 lightUnitVector = glm::normalize(lightVector);
					float factor = glm::dot(N, lightUnitVector);

					float distanceAtin = (lightRange / lengthSqrd);
#pragma endregion NdotLAttenuation
					factor = clamp(factor, 0.0f, 1.0f);
					Ray shadowRay(_Ray.intersection.Pos + 0.01f*lightUnitVector, lightUnitVector, 1e34, true);
					factor *= distanceAtin;

					scene.Intersect(shadowRay);
					if (shadowRay.t >= distance)
						color += (kd * factor * light->m_color) /SHADOWRAYSPERFRAM;
				}
				//else 
				//	return vec3(1, 0, 1);

			}
		//shadowColor = (shadowColor*_Ray.intersection.Mat->diffuse) + light->m_color * (1.0f- _Ray.intersection.Mat->diffuse);
		//color += diffuse;
		}
	}
	return (color* _Ray.intersection.Color);
}

void Renderer::Render(int StartY, int EndY, int StartX, int EndX, int& _frame)
{
	// ray trace one line of pixels of the final image
	screen->ClipTo(0, 0, SCRWIDTH / 2, SCRHEIGHT);
	for (int y = StartY; y < EndY; y++)
	{
		for (int x = StartX; x < EndX; x++)
		{
			// generate primary ray
			Ray ray = camera.GenerateRay(x, y,false);
			// trace primary ray
			vec3 color(0,0,0);
			//if ((y == (SCRHEIGHT / 2)) && ((x % 32) == 0))
				//color = Trace(ray, true, 0);
			//else
				color += Trace(ray, false, 0);

			// visualize ray in 2D if y == 0, and for every 16th pixel
			if ((y == (SCRHEIGHT / 2)) && ((x % 32) == 0))
			{
			//	ray.Draw2D(0xaaaa00);
			}
			// visualize intersection result
			if (_frame != 1)
			{
				m_screen[y*SCRWIDTH + x] += color;
				screen->GetBuffer()[y*screen->GetPitch() + x] = ConvertColor(m_screen[y*SCRWIDTH + x] / (float)_frame);
			}
			else
			{
				m_screen[y*SCRWIDTH + x] = color;
				screen->GetBuffer()[y*screen->GetPitch() + x] = ConvertColor(m_screen[y*SCRWIDTH + x] / (float)_frame);
			}

			lastPos = camera.eyePos;
			lastRotation = camera.Rotation;
		}
	}
}

// EOF