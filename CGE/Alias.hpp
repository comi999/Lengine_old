#pragma once
#include "Component.hpp"
#include "Name.hpp"

template < typename T >
class IAlias;

typedef IAlias< void > Alias;

template < typename T >
class IAlias : public IComponent< Alias >
{
public:

	const Name& GetName() const
	{
		return m_Name;
	}

	void SetName( const Name& a_Name )
	{
		//implement
	}

private:

	static GameObjectID FindByName( Hash a_Name )
	{
		return GameObjectID( -1 );
	}

	friend class GameObject;

	Name m_Name;
	bool m_ToDestroy;
};