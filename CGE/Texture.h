#pragma once

class Texture
{
public:
	Colour* data;
	int textureWidth;
	int textureHeight;

	Texture();
	~Texture();

	bool LoadTexture(const std::string& filePath, float opacityMultiplier = 1);
	bool LoadTexture(const Image& image, float opacityMultiplier = 1);

	bool Rotate(float rotation);
	bool FlipVertical();
	bool FlipHorizontal();
	bool Tint(const Colour& colour);
	bool InvertColour();

	bool CopyToSprite(Sprite* sprite);
	bool CopyFromSprite(Sprite* sprite);
};

