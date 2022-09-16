#pragma once
#include <string>
#include <vector>

#include "Math.hpp"
#include "Hash.hpp"
#include "RenderingAPI.hpp"
#include "Resource.hpp"
#include "Pointer.hpp"
#include "Name.hpp"
#include "Delegate.hpp"

class Shader;

class ShaderInput
{
private:

	ShaderInput( const Shader& a_Shader, int32_t a_Index, const Name& a_Name )
		: m_Shader( &a_Shader )
		, m_Index( a_Index )
		, m_Name( a_Name )
	{ }

public:

	inline const Name& GetName() const { return m_Name; }
	inline const Shader& GetShader() const { return *m_Shader; }
	inline int32_t GetIndex() const { return m_Index; }

	template < typename T >
	bool SetValue( const T* a_Values, size_t a_Count = 1 ) const;

	bool operator==( const ShaderInput& a_ShaderInput ) const;
	inline bool operator!=( const ShaderInput& a_ShaderInput ) const { return !( *this == a_ShaderInput ); }

private:

	friend class Shader;

	const Shader* m_Shader;
	int32_t       m_Index;
	Name          m_Name;
};

class ShaderIterator
{
public:

	using iterator_category = std::forward_iterator_tag;
	using value_type        = const ShaderInput;
	using difference_type   = std::ptrdiff_t;
	using pointer           = value_type*;
	using reference         = value_type&;

private:

	friend class Shader;
	friend class ShaderInput;

	ShaderIterator( const ShaderInput& a_ShaderInput );

public:

	ShaderIterator() = default;
	ShaderIterator( const ShaderIterator& a_ShaderIterator ) = default;
	ShaderIterator& operator++() { ++m_ShaderInput; }
	ShaderIterator operator++( int );
	inline pointer operator->() const { return m_ShaderInput; }
	inline reference operator*() const { return *m_ShaderInput; }
	inline bool operator==( const ShaderIterator& a_ShaderIterator ) const { return *m_ShaderInput == *a_ShaderIterator.m_ShaderInput; }
	inline bool operator!=( const ShaderIterator& a_ShaderIterator ) const { return !( *this == a_ShaderIterator ); }

private:

	const ShaderInput* m_ShaderInput = nullptr;
};

class Shader : public Resource
{
public:

	using Iterator = ShaderIterator;
	using CIterator = ShaderIterator;

	static Shared< Shader > Create( const std::string& a_Source );

	Shader() = delete;
	const std::string& GetSource() const { return m_Source; }
	void SetSource( const std::string& a_Source ) { m_Source = a_Source; }
	const ShaderInput& GetInput( int32_t a_Index ) const { return m_Inputs[ a_Index ]; }
	size_t GetInputCount() const { return m_Inputs.size(); }
	int32_t GetIndex( Hash a_Name ) const;
	int32_t GetIndex( const char* a_Name ) const { GetIndex( CRC32_RT( a_Name ) ); }
	ShaderIterator Begin() const { return ShaderIterator( GetInput( 0 ) ); }
	ShaderIterator End() const { return ShaderIterator( GetInput( GetInputCount() ) ); }
	bool SetPVM( const Matrix4& a_P, const Matrix4& a_V, const Matrix4& a_M ) const;
	bool Compile();
	bool Decompile();
	virtual RenderingAPI GetAPI() const = 0;
	virtual bool IsCompiled() const = 0;
	virtual bool Use() const = 0;
	virtual bool SetValue( int32_t a_Index, const int32_t*  a_Values, size_t a_Count = 1 ) const = 0;
	virtual bool SetValue( int32_t a_Index, const uint32_t* a_Values, size_t a_Count = 1 ) const = 0;
	virtual bool SetValue( int32_t a_Index, const float*    a_Values, size_t a_Count = 1 ) const = 0;

protected:

	virtual void OnCreate() const { };
	virtual bool OnCompile() = 0;
	virtual bool OnDecompile() = 0;
	void AddShaderInput( const Name& a_Name ) const;

private:

	std::string                m_Source;
	std::vector< ShaderInput > m_Inputs;

	const ShaderInput* m_P;
	const ShaderInput* m_V;
	const ShaderInput* m_M;
	const ShaderInput* m_PV;
	const ShaderInput* m_VM;
	const ShaderInput* m_PVM;
};

template <>
class ResourceLoader< Shader >
{
public:

	using result_type = std::shared_ptr< Shader >;

	result_type operator()( Hash a_Name ) const
	{
		std::string Source;

		if ( !Resource::LoadBin( a_Name, Source ) )
		{
			return nullptr;
		}

		return Shader::Create( Source );
	}
};

namespace std
{
	auto begin( const Shader& a_Shader )
	{
		return a_Shader.Begin();
	}
	
	inline auto cbegin( const Shader& a_Shader )
	{
		return begin( a_Shader );
	}

	auto end( const Shader& a_Shader )
	{
		return a_Shader.End();
	}

	inline auto cend( const Shader& a_Shader )
	{
		return end( a_Shader );
	}
};

template < typename T >
bool ShaderInput::SetValue( const T* a_Values, size_t a_Count ) const
{
	return m_Shader->SetValue( m_Index, a_Values, a_Count );
}