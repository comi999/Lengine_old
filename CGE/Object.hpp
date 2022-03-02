#pragma once
#include <cstdint>
#include "Name.hpp"

typedef uint32_t ObjectID;

class Object
{
protected:

	Object( const Name& a_Name, ObjectID a_ObjectID )
		: m_Name( a_Name )
		, m_ObjectID( a_ObjectID )
	{ }
	
protected:

	inline const Name& GetName() const
	{
		return m_Name;
	}

	inline void SetName( const Name& a_Name )
	{
		m_Name = a_Name;
	}

	inline ObjectID GetObjectID() const
	{
		return m_ObjectID;
	}

private:

	Name     m_Name;
	ObjectID m_ObjectID;
};