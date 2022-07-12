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
		, m_Resource{}
		, m_Handle( 0 )
		, m_Location( -1 )
	{ }

	const Name& GetName() const
	{
		return m_Name;
	}

	void SetName( const Name& a_Name )
	{
		m_Name = a_Name;
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
	TextureHandle               m_Handle;
	int32_t                     m_Location;
};

class Material : public Resource
{
public:

	Material()
		: m_Shader( &Shader::Default )
		, m_FoundLocations( false )
	{}

	template < typename T >
	inline void AddProperty( const Name& a_Key, const T& a_Value )
	{
		auto& NewProperty = m_Attributes[ a_Key.HashCode() ];
		NewProperty.SetName( a_Key );
		NewProperty.Set( a_Value );

		if ( !m_Shader->GetProgramHandle() )
		{
			return;
		}

		NewProperty.m_Location = Rendering::GetUniformLocation( m_Shader->GetProgramHandle(), a_Key.Data() );
	}

	template < typename T >
	bool GetProperty( Hash a_Key, T& o_Value ) const
	{
		auto Iter = m_Attributes.find( a_Key );

		if ( Iter != m_Attributes.end() )
		{
			Iter->second.Get( o_Value );
			return true;
		}

		return false;
	}

	template < typename T >
	bool SetProperty( Hash a_Key, const T& a_Value )
	{
		auto Iter = m_Attributes.find( a_Key );

		if ( Iter != m_Attributes.end() )
		{
			Iter->second.Set( a_Value );
			return true;
		}

		return false;
	}

	bool RemoveProperty( Hash a_Key )
	{
		auto Iter = m_Attributes.find( a_Key );

		if ( Iter != m_Attributes.end() )
		{
			m_Attributes.erase( Iter );
			return true;
		}

		return false;
	}

	inline bool HasProperty( Hash a_Key ) const
	{
		return m_Attributes.find( a_Key ) != m_Attributes.end();
	}

	void AddTexture( const Name& a_Key, ResourceHandle< Texture2D > a_Texture )
	{
		auto& NewProperty = m_Textures[ a_Key ];
		NewProperty.m_Name = a_Key;
		NewProperty.m_Resource = a_Texture;

		if ( !m_Shader->GetProgramHandle() )
		{
			return;
		}

		NewProperty.m_Location = Rendering::GetUniformLocation( m_Shader->GetProgramHandle(), a_Key.Data() );
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

	const Shader& GetShader() const
	{
		return m_Shader ? *m_Shader : Shader::Default;
	}

	void SetShader( const Shader& a_Shader )
	{
		m_Shader = &a_Shader;
	}

private:

	friend class Serialization;
	friend class ResourcePackager;
	friend class RenderPipeline;

	// All of the uniform functions really should be delegated to the Shader object.
	void Apply() const
	{
		if ( !m_FoundLocations )
		{
			if ( !m_Shader->GetProgramHandle() )
			{
				const_cast< Shader* >( const_cast< Material* >( this )->m_Shader )->Compile();
			}

			const_cast< Material* >( this )->FindLocations();
			Rendering::UseProgram( GetShader().GetProgramHandle() );
		}

		for ( auto Begin = m_Attributes.begin(), End = m_Attributes.end(); Begin != End; ++Begin )
		{
			if ( Begin->second.m_Location < 0 )
			{
				continue;
			}

			auto Type = Begin->second.GetType();

			if ( Type == MaterialProperty::Type::STRING )
			{
				continue;
			}

			switch ( Type )
			{
				case MaterialProperty::Type::INT:
				{
					switch ( Begin->second.m_Size )
					{
						case 1:
						{
							Rendering::Uniform1iv( Begin->second.m_Location, Begin->second.m_Size, Begin->second.Get< int32_t >() );
							break;
						}
						case 2:
						{
							Rendering::Uniform2iv( Begin->second.m_Location, Begin->second.m_Size, Begin->second.Get< int32_t >() );
							break;
						}
						case 3:
						{
							Rendering::Uniform3iv( Begin->second.m_Location, Begin->second.m_Size, Begin->second.Get< int32_t >() );
							break;
						}
						case 4:
						{
							Rendering::Uniform4iv( Begin->second.m_Location, Begin->second.m_Size, Begin->second.Get< int32_t >() );
							break;
						}
						default:
							break;
					}
					break;
				}
				case MaterialProperty::Type::FLOAT:
				{
					switch ( Begin->second.m_Size )
					{
						case 1:
						{
							Rendering::Uniform1fv( Begin->second.m_Location, Begin->second.m_Size, Begin->second.Get< float >() );
							break;
						}
						case 2:
						{
							Rendering::Uniform2fv( Begin->second.m_Location, Begin->second.m_Size, Begin->second.Get< float >() );
							break;
						}
						case 3:
						{
							Rendering::Uniform3fv( Begin->second.m_Location, Begin->second.m_Size, Begin->second.Get< float >() );
							break;
						}
						case 4:
						{
							Rendering::Uniform4fv( Begin->second.m_Location, Begin->second.m_Size, Begin->second.Get< float >() );
							break;
						}
						default:
							break;
					}
					break;
				}
			}
		}

		for ( auto
			  Begin = const_cast< Material* >( this )->m_Textures.begin(),
			  End = const_cast< Material* >( this )->m_Textures.end();
			  Begin != End; ++Begin )
		{
			if ( Begin->second.m_Location < 0 )
			{
				continue;
			}

			if ( !Begin->second.m_Handle )
			{
				Rendering::GenTextures( 1, &Begin->second.m_Handle );
				Rendering::ActiveTexture( 0 );
				Rendering::BindTexture( TextureTarget::TEXTURE_2D, Begin->second.m_Handle );
				Rendering::TexImage2D( TextureTarget::TEXTURE_2D, 0, TextureFormat( 0 ), Begin->second.m_Resource.Assure()->GetWidth(), Begin->second.m_Resource.Assure()->GetHeight(), 0, TextureFormat( 0 ), TextureSetting( 0 ), Begin->second.m_Resource.Assure()->GetData() );
			}

			Rendering::Uniform1i( Begin->second.m_Location, 0 );
		}
	}

	// There should be an unapply function too to free all texture handles etc.

	void FindLocations()
	{
		for ( auto Begin = m_Attributes.begin(), End = m_Attributes.end(); Begin != End; ++Begin )
		{
			Begin->second.m_Location = Rendering::GetUniformLocation( m_Shader->GetProgramHandle(), Begin->second.GetName().Data() );
		}

		for ( auto Begin = m_Textures.begin(), End = m_Textures.end(); Begin != End; ++Begin )
		{
			Begin->second.m_Location = Rendering::GetUniformLocation( m_Shader->GetProgramHandle(), Begin->second.GetName().Data() );
		}
	}

	template < typename _Serializer >
	void Serialize( _Serializer& a_Serializer ) const
	{
		a_Serializer << *static_cast< const Resource* >( this );
		a_Serializer << m_Attributes << m_Textures;
	}

	template < typename _Deserializer >
	void Deserialize( _Deserializer& a_Deserializer )
	{
		a_Deserializer >> *static_cast< Resource* >( this );
		a_Deserializer >> m_Attributes >> m_Textures;
	}

	template < typename _Sizer >
	void SizeOf( _Sizer& a_Sizer ) const
	{
		a_Sizer&* static_cast< const Resource* >( this );
		a_Sizer& m_Attributes& m_Textures;
	}

	const Shader*                      m_Shader;
	bool                               m_FoundLocations;
	std::map< Hash, MaterialProperty > m_Attributes;
	std::map< Hash, TextureProperty  > m_Textures;

public:

	static Material UnlitFlatColour;
	static Material LitFlatColour;
};