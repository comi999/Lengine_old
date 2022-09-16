#pragma once
#include <string>

#include "Shader.hpp"
#include "Texture.hpp"

class MaterialProperty
{
public:

	enum class Type
	{
		NONE,
		INT,
		FLOAT,
		STRING
	};

	MaterialProperty()
		: m_Name()
		, m_Size( 0 )
		, m_Type( Type::INT )
		, m_Data( nullptr )
		, m_Location( -1 )
	{ }

	MaterialProperty( const MaterialProperty& a_MaterialProperty )
		: m_Name( a_MaterialProperty.m_Name )
		, m_Size( a_MaterialProperty.m_Size )
		, m_Type( a_MaterialProperty.m_Type )
		, m_Data( nullptr )
		, m_Location( -1 )
	{
		if ( !m_Size )
		{
			return;
		}

		size_t TypeSize = 0;

		switch ( m_Type )
		{
			case MaterialProperty::Type::INT:   TypeSize += sizeof( int   ); break;
			case MaterialProperty::Type::FLOAT: TypeSize += sizeof( float ); break;
			case MaterialProperty::Type::STRING:TypeSize += sizeof( char  ); break;
		}

		TypeSize *= m_Size;

		if ( TypeSize > sizeof( m_Data ) )
		{
			m_Data = new uint8_t[ TypeSize ];
			memcpy( m_Data, a_MaterialProperty.m_Data, TypeSize );
		}
		else
		{
			memcpy( &m_Data, &a_MaterialProperty.m_Data, TypeSize );
		}
	}

	MaterialProperty( MaterialProperty&& a_MaterialProperty )
		: m_Name( a_MaterialProperty.m_Name )
		, m_Size( a_MaterialProperty.m_Size )
		, m_Type( a_MaterialProperty.m_Type )
		, m_Data( a_MaterialProperty.m_Data )
		, m_Location( -1 )
	{ }

	~MaterialProperty()
	{
		Clear();
	}

	template < typename T >
	static constexpr Type Convert = Type::INT;

	template <> static constexpr Type Convert< int   > = Type::INT;
	template <> static constexpr Type Convert< float > = Type::FLOAT;
	template <> static constexpr Type Convert< char  > = Type::STRING;

	const Name& GetName() const
	{
		return m_Name;
	}

	void SetName( const Name& a_Name )
	{
		m_Name = a_Name;
	}

	template < typename T >
	void Set( const T& a_Value )
	{
		if constexpr ( std::is_same_v< T, std::string > )
		{
			SetImpl( a_Value.c_str(), a_Value.size() );
			return;
		}

		if constexpr ( std::is_same_v< char*, T > )
		{
			SetImpl( a_Value, strlen( a_Value ) );
			return;
		}

		SetImpl( &a_Value, 1 );
	}

	template < typename T, size_t S >
	void Set( const Vector< T, S >& a_Value )
	{
		SetImpl( &a_Value[ 0 ], S );
	}

	template < typename T >
	void Get( T& o_Value ) const
	{
		if constexpr ( std::is_same_v< T, std::string > )
		{
			o_Value.resize( m_Size );
			GetImpl( o_Value.data(), m_Size );
			return;
		}

		GetImpl( &o_Value, 1 );
	}

	template < typename T >
	inline T* Get()
	{
		return static_cast< T* >( m_Size * sizeof( T ) > sizeof( m_Data ) ? m_Data : &m_Data );
	}

	template < typename T >
	inline const T* Get() const
	{
		return const_cast< MaterialProperty* >( this )->Get< T >();
	}

	template < typename T, size_t S >
	void Get( Vector< T, S >& o_Value ) const
	{
		GetImpl( &o_Value[ 0 ], S );
	}

	Type GetType() const
	{
		return m_Type;
	}

	size_t GetSize() const
	{
		return m_Size;
	}

	void Clear()
	{
		if ( !m_Size )
		{
			return;
		}

		switch ( m_Type )
		{
			case Type::INT:    if ( sizeof( int   ) * m_Size > sizeof( m_Data ) ) delete[] m_Data; break;
			case Type::FLOAT:  if ( sizeof( float ) * m_Size > sizeof( m_Data ) ) delete[] m_Data; break;
			case Type::STRING: if ( sizeof( char  ) * m_Size > sizeof( m_Data ) ) delete[] m_Data; break;
		}

		m_Size = 0;
		m_Type = Type::NONE;
	}

private:

	friend class Serialization;
	friend class Material;

	template < typename T >
	void SetImpl( const T* a_Values, size_t a_Count )
	{
		Clear();
		m_Size = a_Count;

		if ( sizeof( T ) * a_Count > sizeof( m_Data ) )
		{
			m_Data = new T[ a_Count ];
			memcpy( m_Data, a_Values, a_Count * sizeof( T ) );
		}
		else
		{
			memcpy( &m_Data, a_Values, sizeof( T ) * a_Count );
		}

		m_Type = Convert< T >;
	}

	template < typename T >
	void GetImpl( T* a_Values, size_t a_Count ) const
	{
		size_t Count = a_Count < m_Size ? a_Count : m_Size;
		bool InPlace = false;

		switch ( m_Type )
		{
			case Type::INT:    InPlace = sizeof( int   ) * m_Size <= sizeof( m_Data ); break;
			case Type::FLOAT:  InPlace = sizeof( float ) * m_Size <= sizeof( m_Data ); break;
			case Type::STRING: InPlace = sizeof( char  ) * m_Size <= sizeof( m_Data ); break;
		}

		const void* Data = InPlace ? &m_Data : m_Data;

		switch ( m_Type )
		{
			case Type::INT:
			{
				for ( size_t i = 0; i < Count; ++i )
				{
					a_Values[ i ] = reinterpret_cast< const int* >( Data )[ i ];
				}

				break;
			}
			case Type::FLOAT:
			{
				for ( size_t i = 0; i < Count; ++i )
				{
					a_Values[ i ] = reinterpret_cast< const float* >( Data )[ i ];
				}

				break;
			}
			case Type::STRING:
			{
				for ( size_t i = 0; i < Count; ++i )
				{
					a_Values[ i ] = reinterpret_cast< const char* >( Data )[ i ];
				}

				break;
			}
		}
	}

	template < typename T >
	void Serialize( T& a_Serializer ) const
	{
		a_Serializer << m_Name << m_Size << m_Type;

		switch ( m_Type )
		{
			case MaterialProperty::Type::INT:
			{
				std::vector< int > Data;
				Data.resize( m_Size );
				GetImpl( Data.data(), m_Size );
				a_Serializer << Data;
				break;
			}
			case MaterialProperty::Type::FLOAT:
			{
				std::vector< float > Data;
				Data.resize( m_Size );
				GetImpl( Data.data(), m_Size );
				a_Serializer << Data;
				break;
			}
			case MaterialProperty::Type::STRING:
			{
				std::vector< char > Data;
				Data.resize( m_Size );
				GetImpl( Data.data(), m_Size );
				a_Serializer << Data;
				break;
			}
		}
	}

	template < typename T >
	void Deserialize( T& a_Deserializer )
	{
		a_Deserializer >> m_Name >> m_Size >> m_Type;

		switch ( m_Type )
		{
			case MaterialProperty::Type::INT:
			{
				std::vector< int > Data;
				a_Deserializer >> Data;
				SetImpl( Data.data(), m_Size );
				break;
			}
			case MaterialProperty::Type::FLOAT:
			{
				std::vector< float > Data;
				a_Deserializer >> Data;
				SetImpl( Data.data(), m_Size );
				break;
			}
			case MaterialProperty::Type::STRING:
			{
				std::vector< char > Data;
				a_Deserializer >> Data;
				SetImpl( Data.data(), m_Size );
				break;
			}
		}
	}

	template < typename T >
	void SizeOf( T& a_Sizer ) const
	{
		a_Sizer & m_Size & m_Type;
		size_t TypeSize = 0;

		switch ( m_Type )
		{
			case Type::INT:    TypeSize = sizeof( int   ); break;
			case Type::FLOAT:  TypeSize = sizeof( float ); break;
			case Type::STRING: TypeSize = sizeof( char  ); break;
		}

		a_Sizer += TypeSize * m_Size;
	}

	Name    m_Name;
	size_t  m_Size;
	Type    m_Type;
	void*   m_Data;
	int32_t m_Location;
};

class TextureProperty
{
public:

	TextureProperty()
		: m_Name()
		, m_Resource{}/*
		, m_Handle( 0 )
		, m_Location( -1 )*/
	{ }

	const Name& GetName() const
	{
		return m_Name;
	}

	void SetName( const Name& a_Name )
	{
		m_Name = a_Name;
	}

	Texture2D* GetTexture()
	{
		return m_Resource.Assure();
	}
	
	const Texture2D* GetTexture() const
	{
		return m_Resource.Assure();
	}

private:

	friend class Serialization;
	friend class Material;

	template < typename T >
	void Serialize( T& a_Serializer ) const
	{
		a_Serializer << m_Name << m_Resource;
	}

	template < typename T >
	void Deserialize( T& a_Deserializer )
	{
		a_Deserializer >> m_Name >> m_Resource;
	}

	template < typename T >
	void SizeOf( T& a_Sizer ) const
	{
		a_Sizer& m_Name& m_Resource;
	}

	Name                        m_Name;
	ResourceHandle< Texture2D > m_Resource;
	/*TextureHandle               m_Handle;
	int32_t                     m_Location;
	int32_t                     m_TextureUnit;*/
};

class Material : public Resource
{
public:

	Material()
		: m_Shader( nullptr )
		, FrontFaceCull( false )
		, BackFaceCull( false )
		, AlphaBlending( false )
	{ }

	bool FrontFaceCull : 1;
	bool BackFaceCull  : 1;
	bool AlphaBlending : 1;

	template < typename T >
	void AddProperty( const Name& a_Key, const T& a_Value )
	{
		m_Properties.push_back();
		auto& NewProperty = m_Properties.back();
		NewProperty.SetName( a_Key );
		NewProperty.Set( a_Value );

		if ( !m_Shader || !m_Shader->IsCompiled() )
		{
			return;
		}

		int32_t InputIndex = m_Shader->GetIndex( a_Key.HashCode() );

		if ( InputIndex == -1 )
		{
			return;
		}

		m_Enabled.emplace_back( m_Properties.size() - 1, InputIndex );
	}

	template < typename T >
	bool GetProperty( Hash a_Key, T& o_Value ) const
	{
		auto Iter = std::find( m_Properties.begin(), m_Properties.end(), [&]( const MaterialProperty& a_MaterialProperty ) 
		{ 
			return a_MaterialProperty.GetName().HashCode() == a_Key; 
		} );

		if ( Iter == m_Properties.end() )
		{
			return false;
		}
		
		Iter->Get( o_Value );
		return true;
	}

	template < typename T >
	bool SetProperty( Hash a_Key, const T& a_Value )
	{
		auto Iter = std::find( m_Properties.begin(), m_Properties.end(), [&]( const MaterialProperty& a_MaterialProperty ) 
		{ 
			return a_MaterialProperty.GetName().HashCode() == a_Key; 
		} );

		if ( Iter == m_Properties.end() )
		{
			return false;
		}
		
		Iter->Set( o_Value );
		return true;
	}

	bool RemoveProperty( Hash a_Key )
	{
		auto Iter = std::find( m_Properties.begin(), m_Properties.end(), [&]( const MaterialProperty& a_MaterialProperty )
		{
			return a_MaterialProperty.GetName().HashCode() == a_Key;
		} );

		if ( Iter == m_Properties.end() )
		{
			return false;
		}

		int32_t PropertyIndex = Iter - m_Properties.begin();

		auto ToRemove = std::find( m_Enabled.begin(), m_Enabled.end(), [&]( const std::pair< int32_t, int32_t >& a_IndexPair )
		{
			return a_IndexPair.first == PropertyIndex;
		} );

		if ( ToRemove != m_Enabled.end() )
		{
			m_Enabled.erase( ToRemove );
		}

		return true;
	}

	inline bool HasProperty( Hash a_Key ) const
	{
		return std::find( m_Properties.begin(), m_Properties.end(), [&]( const MaterialProperty& a_MaterialProperty )
		{
			return a_MaterialProperty.GetName().HashCode() == a_Key;
		} ) == m_Properties.end();
	}

	inline auto GetPropertyBegin() const
	{
		return m_Properties.begin();
	}

	inline auto GetPropertyEnd() const
	{
		return m_Properties.begin();
	}

	void AddTexture( const Name& a_Key, ResourceHandle< Texture2D > a_Texture )
	{
		auto& NewProperty = m_Textures[ a_Key ];
		NewProperty.m_Name = a_Key;
		NewProperty.m_Resource = a_Texture;

		/*if ( !m_Shader->GetProgramHandle() )
		{
			return;
		}*/

		//NewProperty.m_Location = Rendering::GetUniformLocation( m_Shader->GetProgramHandle(), a_Key.Data() );
	}

	ResourceHandle< Texture2D > GetTexture( Hash a_Key )
	{
		auto Iter = m_Textures.find( a_Key );
		return Iter != m_Textures.end() ? Iter->second.m_Resource : ResourceHandle< Texture2D >{};
	}

	bool SetTexture( Hash a_Key, ResourceHandle< Texture2D > a_Texture )
	{
		auto Iter = m_Textures.find( a_Key );

		if ( Iter != m_Textures.end() )
		{
			Iter->second.m_Resource = a_Texture;
			return true;
		}

		return false;
	}

	bool RemoveTexture( Hash a_Key )
	{
		auto Iter = m_Textures.find( a_Key );

		if ( Iter != m_Textures.end() )
		{
			m_Textures.erase( Iter );
			return true;
		}

		return false;
	}

	inline bool HasTexture( Hash a_Key )
	{
		return m_Textures.find( a_Key ) != m_Textures.end();
	}

	inline auto GetTextureBegin() const
	{
		return m_Textures.begin();
	}

	inline auto GetTextureEnd() const
	{
		return m_Textures.end();
	}

	const Shader* GetShader() const
	{
		return m_Shader;
	}

	void SetShader( const Shader* a_Shader )
	{
		m_Shader = a_Shader;

		if ( !m_Shader || !m_Shader->IsCompiled() )
		{
			return;
		}

		for ( auto& Input : *m_Shader )
		{
			auto Iter = std::find( m_Properties.begin(), m_Properties.end(), [&]( const MaterialProperty& a_Property ) 
			{ 
				a_Property.GetName().HashCode() == Input.GetName().HashCode(); 
			} );

			if ( Iter != m_Properties.end() )
			{
				m_Enabled.emplace_back( &*Iter, &Input );
			}
		}
	}

	bool ApplyPropertiesToShader()
	{
		if ( !m_Shader || !m_Shader->IsCompiled() )
		{
			return false;
		}

		for ( auto& Pair : m_Enabled )
		{
			auto Type = Pair.first->GetType();

			switch ( Type )
			{
				case MaterialProperty::Type::INT:
				{
					Pair.second->SetValue( Pair.first->Get< int32_t >(), Pair.first->GetSize() );
					break;
				}
				case MaterialProperty::Type::FLOAT:
				{
					Pair.second->SetValue( Pair.first->Get< float >(), Pair.first->GetSize() );
					break;
				}
			}
		}
	}

private:

	friend class Serialization;
	friend class ResourcePackager;
	friend class RenderPipeline;

	template < typename _Serializer >
	void Serialize( _Serializer& a_Serializer ) const
	{
		a_Serializer << *static_cast< const Resource* >( this );
	}

	template < typename _Deserializer >
	void Deserialize( _Deserializer& a_Deserializer )
	{
		a_Deserializer >> *static_cast< Resource* >( this );
	}

	template < typename _Sizer >
	void SizeOf( _Sizer& a_Sizer ) const
	{
		a_Sizer&* static_cast< const Resource* >( this );
	}

	const Shader*                                m_Shader;
	std::vector< MaterialProperty >              m_Properties;
	std::vector< TextureProperty  >              m_Textures;
	std::vector< std::pair< int32_t, int32_t > > m_Enabled;
};