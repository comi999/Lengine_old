#pragma once
#include <string>

class Image
{
public:
	int size;
	char* header;
	int headerSize; 
	char* data;
	int dataSize;

	Image();
	~Image();

	bool LoadImageFile(const std::string& filePath);
};

