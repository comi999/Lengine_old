#include "Colour.h"

Colour::Colour()
{
	r = 0;
	g = 0;
	b = 0;
	a = 255;
}

Colour::Colour(int R, int G, int B, int A)
{
	r = (unsigned char)R;
	g = (unsigned char)G;
	b = (unsigned char)B;
	a = (unsigned char)A;
}

Colour::Colour(float R, float G, float B, float A)
{
	r = (unsigned char)R;
	g = (unsigned char)G;
	b = (unsigned char)B;
	a = (unsigned char)A;
}

void Colour::Premultiply()
{
	if (a == 255)
		return;
	if (a == 0)
	{
		r = 0;
		g = 0;
		b = 0;
		return;
	}
	r *= a * 0.0039f;
	g *= a * 0.0039f;
	b *= a * 0.0039f;
}

Colour Colour::operator +(const Colour& rhs) const
{
	Colour src = rhs;
	Colour dst = *this;

	src.Premultiply();
	dst.Premultiply();
	int invA = 1 - rhs.a;

	return 
	{
		src.r + dst.r * invA,
		src.g + dst.g * invA,
		src.b + dst.b * invA
	};
}

void Colour::operator +=(const Colour& rhs)
{
	*this = *this + rhs;
}

bool Colour::operator ==(const Colour& rhs) const
{
	return (this->r == rhs.r) ? 
		  ((this->g == rhs.g) ? 
	      ((this->b == rhs.b) ? 
		   (this->a == rhs.a) : false) : false) : false;
}