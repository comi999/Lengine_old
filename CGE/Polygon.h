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
	Colour colour;
};

struct Square
{
	Vector2 position;
	float s;
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

struct Ellipse
{
	Vector2 position;
	Vector2 size;
};

struct Edge2D
{
	Point2D point[2];
};

struct Edge3D
{
	Point3D point[2];
};

struct Triangle2D
{
	Point2D point[3];
};

struct vTriangle2D
{
	Vertex2D vertex[3];
};

struct Triangle3D
{
	Point3D point[3];
};

struct vTriangle3D
{
	Vertex3D vertex[3];
};

struct Rectangle2D
{
	Point2D point[4];
};

struct vRectangle2D
{
	Vertex2D vertex[4];
};
	
struct Rectangle3D
{
	Point3D point[4];
};

struct vRectangle3D
{
	Vertex3D vertex[4];
};

template <int s>
struct Polygon
{
	Point2D point[s];
};

template <int s>
struct vPolygon
{
	Vertex2D vertex[s];
};

template <int s>
struct RegPolygon
{
	int sides = s;
	Vector2 position;
	Colour colour[s];
};

template <int s>
struct vRegPolygon
{
	int sides = s;
	Vector2 position;
	Vector2 texel[s];
};