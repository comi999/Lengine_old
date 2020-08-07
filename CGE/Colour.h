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

const Colour WHITE        (255, 255, 255);
const Colour BLACK        (0, 0, 0);
const Colour GREY         (128, 128, 128);
const Colour LIGHT_GREY   (196, 196, 196);
const Colour DARK_GREY    (64, 64, 64);
const Colour BROWN        (100, 50, 20);
const Colour RED          (255, 0, 0);
const Colour LIGHT_RED    (255, 64, 64);
const Colour DARK_RED     (128, 0, 0);
const Colour ORANGE       (255, 64, 0);
const Colour LIGHT_ORANGE (255, 128, 0);
const Colour DARK_ORANGE  (196, 32, 0);
const Colour YELLOW       (196, 196, 0);
const Colour LIGHT_YELLOW (255, 255, 32);
const Colour DARK_YELLOW  (153, 153, 0);
const Colour GREEN        (0, 255, 0);
const Colour LIGHT_GREEN  (64, 255, 64);
const Colour DARK_GREEN   (0, 128, 0);
const Colour BLUE         (0, 0, 255);
const Colour DARK_BLUE    (0, 0, 96);
const Colour LIGHT_BLUE   (51, 153, 255);
const Colour PURPLE       (64, 0, 255);
const Colour DARK_PURPLE  (28, 0, 91);
const Colour LIGHT_PURPLE (153, 153, 255);
const Colour PINK         (255, 0, 255);
const Colour LIGHT_PINK   (255, 96, 229);
const Colour DARK_PINK    (140, 0, 140);