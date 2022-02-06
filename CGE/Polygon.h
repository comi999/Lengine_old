#pragma once
#include "Colour.h"
#include "Math.h"

struct Point2D
{
	Vector2 position;
	Colour colour;
};

struct Point3D
{
	Vector3 position;
	Colour colour;
};

struct Vertex2D
{
	Vector2 position;
	Vector2 texel;
};

struct Vertex3D
{
	Vector3 position;
	Vector2 texel;
};

struct Line
{
	Vector2 point[2];
};

struct Edge2D
{
	Point2D point[2];
};

struct Edge3D
{
	Point3D point[2];
};

struct Triangle
{
	Vector2 position;
	Vector2 point[3];
};

struct Rect
{
	Vector2 position;
	Vector2 size;
};

struct Circle
{
	Vector2 position;
	float radius;
};

struct Triangle2D
{
	Vector2 position;
	Point2D point[3];
};

struct vTriangle2D
{
	Vector2 position;
	Vertex2D vertex[3];
};

struct Triangle3D
{
	Vector3 position;
	Point3D point[3];
};

struct vTriangle3D
{
	Vector3 position;
	Vertex3D vertex[3];
};

struct Rectangle2D
{
	Rect rect;
	Colour colour[4];
};

struct vRectangle2D
{
	Rect rect;
	Vector2 texel[4];
};
	
struct Rectangle3D
{
	Vector3 pos;
	Vector2 size;
	Colour colour[4];
};

struct vRectangle3D
{
	Vector3 pos;
	Vector2 size;
	Vector2 texel[4];
};

template <int s>
class Poly
{
public:
	int sides;
	Vector2 position;
	Vector2 point[s];

	Poly(Vector2 position, float radians = 0)
	{
		sides = s;
		this->position = position;
		float angle = 6.283f / s;
		float cos = cosf(angle);
		float sin = sinf(angle);
		Matrix2 m(cos, sin, -sin, cos);
		point[0] = { cosf(radians), sinf(radians) };
		for (int i = 1; i < sides; i++)
			point[i] = m * point[i - 1];
	}

	void Rotate(float radians)
	{
		float cos = cosf(radians);
		float sin = sinf(radians);
		Matrix2 m(cos, sin, -sin, cos);

		for (int i = 0; i < sides; i++)
			point[i] = m * point[i];
	}

	void MakeReg(float radians = 0)
	{
		float angle = 6.283f / s;
		float cos = cosf(angle);
		float sin = sinf(angle);
		Matrix2 m(cos, sin, -sin, cos);
		point[0] = { cosf(radians), sinf(radians) };
		for (int i = 1; i < sides; i++)
			point[i] = m * point[i - 1];
	}
};

template <int s>
class Shape
{
public:
	int sides;
	Vector2 position;
	Point2D point[s];

	Shape(Vector2 position, float radians = 0)
	{
		sides = s;
		this->position = position;
		float angle = 6.283f / s;
		float cos = cosf(angle);
		float sin = sinf(angle);
		Matrix2 m(cos, sin, -sin, cos);
		point[0].position = { cosf(radians), sinf(radians) };
		for (int i = 1; i < sides; i++)
			point[i].position = m * point[i - 1].position;
	}

	void Rotate(float radians)
	{
		float cos = cosf(radians);
		float sin = sinf(radians);
		Matrix2 m(cos, sin, -sin, cos);

		for (int i = 0; i < sides; i++)
			point[i].position = m * point[i].position;
	}

	void MakeReg(float radians = 0)
	{
		float angle = 6.283f / s;
		float cos = cosf(angle);
		float sin = sinf(angle);
		Matrix2 m(cos, sin, -sin, cos);
		point[0].position = { cosf(radians), sinf(radians) };
		for (int i = 1; i < sides; i++)
			point[i].position = m * point[i - 1].position;
	}
};

template <int s>
class vShape
{
public:
	int sides = s;
	Vector2 position;
	Vertex2D vertex[s];

	vShape(Vector2 position, float radians = 0)
	{
		sides = s;
		this->position = position;
		float angle = 6.283f / sides;
		float cos = cosf(angle);
		float sin = sinf(angle);
		Matrix2 m(cos, sin, -sin, cos);
		vertex[0].position = { sinf(radians), cosf(radians) };
		for (int i = 1; i < sides; i++)
			vertex[i].position = m * vertex[i - 1].position;
	}

	void Rotate(float radians)
	{
		float cos = cosf(radians);
		float sin = sinf(radians);
		Matrix2 m(cos, sin, -sin, cos);

		for (int i = 0; i < sides; i++)
			vertex[i].position = m * vertex[i].position;
	}

	void MakeReg(float radians = 0)
	{
		float angle = 6.283f / sides;
		float cos = cosf(angle);
		float sin = sinf(angle);
		Matrix2 m(cos, sin, -sin, cos);
		vertex[0].position = { sinf(radians), cosf(radians) };
		for (int i = 1; i < sides; i++)
			vertex[i].position = m * vertex[i - 1].position;
	}
};