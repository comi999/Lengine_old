#pragma once
#include <any>
#include <variant>

class Any
{
	enum class Operator
	{
		Copy,
		Move,
		Destruct,
		Equality,
	};

	using Vtable = void*( * )( Operator, void*, void* );

	template < typename T >
	static void* VtableImpl( Operator a_Operation, void* a_From, void* a_To )
	{
		switch ( a_Operation )
		{
		case Any::Operator::Copy:
		{


			break;
		}
		case Any::Operator::Move:
			break;
		case Any::Operator::Destruct:
			break;
		case Any::Operator::Equality:
			break;
		default:
			break;
		}
	};
	
	void*  m_Data;
	Vtable m_Vtable;
};