#pragma once
class Sprite
{
public:
	int* data;
	int spriteWidth;
	int spriteHeight;
	int rawSize;
	int trimSize;

	Sprite();
	~Sprite();

	bool GenerateSprite(const Texture& texture, const Rect& rect, float rotation = 0);

	bool Rotate(float rotation);
	bool FlipVertical();
	bool FlipHorizontal();
	bool Tint(const Colour& colour);
	bool InvertColour();

	bool CopyToTexture(Texture* sprite);
	bool CopyFromTexture(Texture* sprite);
};
