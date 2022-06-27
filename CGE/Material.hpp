#pragma once
#include <string>
#include "Shader.hpp"
#include "Texture.hpp"

class MaterialProperty
{
public:

	enum class Type
	{
		INT,
		FLOAT,
		STRING
	};

	MaterialProperty()
		: m_Name()
		, m_Size( 0 )
		, m_Type( Type::INT )
		, m_Data( nullptr )
		, m_Location( 0 )
	{ }

	~MaterialProperty()
	{
		Clear();
	}

	template < typename T >
	static constexpr Type Convert = Type::INT;

	template <> static constexpr Type Convert< int         > = Type::INT;
	template <> static constexpr Type Convert< float       > = Type::FLOAT;
	template <> static constexpr Type Convert< std::string > = Type::STRING;

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
	inline const T* Get() const
	{
		return static_cast< const T* >( m_Size * sizeof( T ) < sizeof( m_Data ) ? &m_Data : m_Data );
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
		switch ( m_Type )
		{
			case MaterialProperty::Type::INT: if ( sizeof( int ) * m_Size > sizeof( m_Data ) ) delete[] m_Data; break;
			case MaterialProperty::Type::FLOAT: if ( sizeof( float ) * m_Size > sizeof( m_Data ) ) delete[] m_Data; break;
			case MaterialProperty::Type::STRING: delete[] m_Data; break;
		}
	}

private:

	friend class Serialization;
	friend class Material;

	template < typename T >
	void SetImpl( const T* a_Values, size_t a_Count )
	{
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
			case MaterialProperty::Type::INT:    InPlace = sizeof( int ) * m_Size < sizeof( m_Data ); break;
			case MaterialProperty::Type::FLOAT:  InPlace = sizeof( float ) * m_Size < sizeof( m_Data ); break;
			case MaterialProperty::Type::STRING: InPlace = sizeof( char ) * m_Size < sizeof( m_Data ); break;
		}

		const void* Data = InPlace ? &m_Data : m_Data;

		switch ( m_Type )
		{
			case Material::Property::Type::INT:
			{
				for ( size_t i = 0; i < Count; ++i )
				{
					a_Values[ i ] = reinterpret_cast< const int* >( Data )[ i ];
				}

				break;
			}
			case Material::Property::Type::FLOAT:
			{
				for ( size_t i = 0; i < Count; ++i )
				{
					a_Values[ i ] = reinterpret_cast< const float* >( Data )[ i ];
				}

				break;
			}
			case Material::Property::Type::STRING:
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
		a_Serializer << m_Type << m_Size;

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
		a_Deserializer >> m_Type >> m_Size;

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
		a_Sizer& m_Size& m_Type;
		size_t TypeSize = 0;

		switch ( m_Type )
		{
			case MaterialProperty::Type::INT:    TypeSize = sizeof( int );   break;
			case MaterialProperty::Type::FLOAT:  TypeSize = sizeof( float ); break;
			case MaterialProperty::Type::STRING: TypeSize = sizeof( int );   break;
		}

		a_Sizer += TypeSize * m_Size;
	}

	Name    m_Name;
	size_t  m_Size;
	Type    m_Type;
	void*   m_Data;
	int32_t m_Location;
};

class Material : public Resource
{
private:

	class TextureRef
	{
	public:

		Hash                        Name;
		ResourceHandle< Texture2D > Handle;

	private:

		friend class Serialization;

		template < typename T >
		void Serialize( T& a_Serializer ) const
		{
			a_Serializer << Name;
		}

		template < typename T >
		void Deserialize( T& a_Deserializer )
		{
			a_Deserializer >> Name;
		}

		template < typename T >
		void SizeOf( T& a_Sizer ) const
		{
			a_Sizer & Name;
		}
	};

public:

	Material()
		: m_Shader( &Shader::Default )
	{ }

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
	bool SetProperty( const Name& a_Key, const T& a_Value )
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
		TextureRef& NewTexture = m_Textures[ a_Key.HashCode() ];
		NewTexture.Name = a_Key;
		NewTexture.Handle = a_Texture;
	}

	ResourceHandle< Texture2D > GetTexture( Hash a_Key )
	{
		auto Iter = m_Textures.find( a_Key );

		if ( Iter != m_Textures.end() )
		{
			if ( !Iter->second.Handle )
			{
				Iter->second.Handle = Resource::Load< Texture2D >( Iter->first );
			}

			return Iter->second.Handle;
		}
		
		return ResourceHandle< Texture2D >{};
	}

	bool SetTexture( Hash a_Key, ResourceHandle< Texture2D > a_Texture )
	{
		auto Iter = m_Textures.find( a_Key );

		if ( Iter != m_Textures.end() )
		{
			Iter->second.Handle = a_Texture;
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

	void SetShader( Shader& a_Shader )
	{
		m_Shader = &a_Shader;
		FindLocations();		
	}

private:

	friend class Serialization;
	friend class ResourcePackager;
	friend class RenderPipeline;

	// All of the uniform functions really should be delegated to the Shader object.
	void Apply() const
	{
		if ( !m_Shader->GetProgramHandle() )
		{
			const_cast< Material* >( this )->m_Shader->Compile();
			const_cast< Material* >( this )->FindLocations();
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
					Rendering::Uniform1iv( Begin->second.m_Location, Begin->second.m_Size, Begin->second.Get< int32_t >() );
					break;
				}
				case MaterialProperty::Type::FLOAT:
				{
					Rendering::Uniform1fv( Begin->second.m_Location, Begin->second.m_Size, Begin->second.Get< float >() );
					break;
				}
			}
		}
	}

	void FindLocations()
	{
		for ( auto Begin = m_Attributes.begin(), End = m_Attributes.end(); Begin != End; ++Begin )
		{
			Begin->second.m_Location = Rendering::GetUniformLocation( m_Shader->GetProgramHandle(), Begin->second.GetName().Data() );
		}
	}

	template < typename T >
	void Serialize( T& a_Serializer ) const
	{
		a_Serializer << *static_cast< const Resource* >( this );
		a_Serializer << m_Attributes << m_Textures;
	}

	template < typename T >
	void Deserialize( T& a_Deserializer )
	{
		a_Deserializer >> *static_cast< Resource* >( this );
		a_Deserializer >> m_Attributes >> m_Textures;
	}

	template < typename T >
	void SizeOf( T& a_Sizer ) const
	{
		a_Sizer & *static_cast< const Resource* >( this );
		a_Sizer & m_Attributes & m_Textures;
	}

	Shader*                            m_Shader;
	std::map< Hash, MaterialProperty > m_Attributes;
	std::map< Hash, TextureRef       > m_Textures;
};

struct MaterialOld
{
	MaterialOld()
	{
		name;
		Ns = 0.0f;
		Ni = 0.0f;
		d = 0.0f;
		illum = 0;
	}

	// Material Name
	std::string name;
	// Ambient Color
	Vector3 Ka;
	// Diffuse Color
	Vector3 Kd;
	// Specular Color
	Vector3 Ks;
	// Specular Exponent
	float Ns;
	// Optical Density
	float Ni;
	// Dissolve
	float d;
	// Illumination
	int illum;
	// Ambient Texture Map
	std::string map_Ka;
	// Diffuse Texture Map
	std::string map_Kd;
	// Specular Texture Map
	std::string map_Ks;
	// Specular Hightlight Map
	std::string map_Ns;
	// Alpha Texture Map
	std::string map_d;
	// Bump Map
	std::string map_bump;

private:

	friend class ResourcePackager;
	friend class Serialization;

	template < typename T >
	void Deserializer( T& a_Deserializer )
	{

	}

	template < typename T >
	void SizeOf( T& a_Sizer ) const
	{

	}
};