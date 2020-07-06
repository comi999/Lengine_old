#pragma once

class Colour
{
public:
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	Colour();
	Colour(int R, int G, int B, int A = 255);
	Colour(float R, float G, float B, float A = 255);

	void Premultiply();

	Colour operator +(const Colour& rhs) const;
	void operator +=(const Colour& rhs);
	bool operator ==(const Colour& rhs) const;
};

