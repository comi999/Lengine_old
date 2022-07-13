#pragma once

#include "Resource.hpp"

class RawFileResource : public Resource
{
public:
    RawFileResource() : data(nullptr), length(0) {}

	~RawFileResource()
	{
		if (data)
		{
			delete[] data;
			data = nullptr;
		}
	}

	void LoadFromFile(const char* path)
	{
        File inputFile(path);
        inputFile.Open();
        length = inputFile.Size();
        data = new unsigned char[length];
        inputFile.Read(data, length);
	}

	template < typename _Serializer >
	void Serialize(_Serializer& a_Serializer) const
	{
		a_Serializer << *static_cast<const Resource*>(this);
		a_Serializer << length;
		a_Serializer.Stream().Write(data, length);
	}

	template < typename _Deserializer >
	void Deserialize(_Deserializer& a_Deserializer)
	{
		a_Deserializer >> *static_cast<Resource*>(this);
		a_Deserializer >> length;
		data = new unsigned char[length];
		a_Deserializer.Stream().Read(data, length);
	}

	template < typename _Sizer >
	void SizeOf(_Sizer& a_Sizer) const
	{
		a_Sizer & *static_cast<const Resource*>(this);
		a_Sizer & length;
		a_Sizer += length;
	}

	unsigned char* data;
	size_t length;
};
