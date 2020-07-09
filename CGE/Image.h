#pragma once
class Image
{
	int size;
	char* header;
	int headerSize; 
	char* data;
	int dataSize;

	bool LoadImageFile(std::string filePath);
};

