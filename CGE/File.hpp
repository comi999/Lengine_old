#pragma once
#include <fstream>
#include "Serialization.hpp"

namespace std {
namespace filesystem{
class path; } }

class File;
class FileIterator;

class Path
{
public:

	Path();
	Path( const std::filesystem::path& a_Path );
	Path( const Path& a_Path );
	Path( Path&& a_Path );
	~Path();
	inline Path GetAbsolute() const;
	inline Path GetRelative() const;
	inline Path GetParent() const;
	inline Path GetRoot() const;
	inline bool IsDirectory() const;
	inline bool IsFile() const;
	inline operator const char* () const;
	inline operator std::string () const;
	inline operator const std::filesystem::path& () const;
	inline Path& operator =( const std::filesystem::path& a_Path );
	inline Path& operator =( const Path& a_Path );
	inline bool operator ==( const std::filesystem::path& a_Path ) const;
	inline bool operator ==( const Path& a_Path ) const;
	inline bool operator !=( const std::filesystem::path& a_Path ) const;
	inline bool operator !=( const Path& a_Path ) const;
	inline Path& operator /=( const char* a_Append );
	inline Path  operator /( const char* a_Append ) const;
	inline Path& operator --();
	inline Path  operator --( int );

private:

	friend class Directory;
	friend class DirectoryIterator;
	friend class File;
	friend class FileIterator;
	friend class FileRecursiveIterator;

	std::filesystem::path* m_Path;
};

class Directory : public Path
{
public:

	Directory();
	Directory( const std::filesystem::path& a_Path );
	Directory( const Path& a_Path );
	Directory( Path&& a_Path );
	Directory( const Directory& a_Directory );
	Directory( Directory&& a_Directory );
	inline Directory GetAbsolute() const;
	inline Directory GetRelative() const;
	inline Directory GetParent() const;
	inline Directory GetRoot() const;
	bool Contains( const char* a_FileOrFolder ) const;
	bool ContainsFile( const char* a_File ) const;
	File CreateFile( const char* a_File, size_t a_Size ) const;
	File GetFile( const char* a_File ) const;
	bool ContainsDirectory( const char* a_Directory ) const;
	Directory CreateDirectory( const char* a_Directory ) const;
	Directory GetDirectory( const char* a_Directory );
	static void SetWorkingDirectory( const Directory& a_Directory );
	inline operator const Path& () const;
	inline operator Path& ();
	inline Directory& operator =( const std::filesystem::path& a_Path );
	inline Directory& operator =( const Path& a_Path );
	inline Directory& operator =( const Directory& a_Path );
	inline Directory& operator /=( const char* a_Append );
	inline Directory  operator /( const char* a_Append ) const;
	inline Directory& operator --();
	inline Directory  operator --( int );
};

class File : public Path
{
public:

	File( const std::filesystem::path& a_Path );
	File( const Path& a_Path );
	File( Path&& a_Path );
	File( const File& ) = delete;
	File( File&& a_File );
	std::string GetName() const;
	std::string GetExtension() const;
	std::string GetStem() const;
	bool Open();
	bool Close();
	inline bool IsOpen() const;
	inline void Write( const void* a_From, size_t a_Size );
	inline void Read( void* a_To, size_t a_Size );
	inline void Seek( size_t a_Position );
	inline bool AtEnd() const;
	inline size_t Size() const;
	inline operator const Path& () const;
	inline operator Path& ();
	inline operator Directory () const;
	inline operator std::ifstream ();
	inline operator std::ofstream ();
	inline operator std::fstream (); 
	inline operator FILE* ();

private:

	friend class FileIterator;
	template < typename > friend class Serializer;
	template < typename > friend class Deserializer;

	FILE* m_File;
};

class FileIterator
{
private:

	FileIterator( std::filesystem::directory_iterator a_DirectoryIterator )
		: m_Underlying( a_DirectoryIterator )
	{ }

public:

	FileIterator( const Directory& a_Directory )
		: m_Underlying( a_Directory.GetPath().m_Path )
		, m_End( std::filesystem::end( std::filesystem::directory_iterator( a_Directory.GetPath().m_Path ) ) )
	{
		while ( m_Underlying != m_End && !std::filesystem::is_regular_file( *m_Underlying ) )
		{
			++m_Underlying;
		}
	}

	inline FileIterator& operator++()
	{
		_STL_ASSERT( m_Underlying != m_End, "Cannot iterate past end." );
		while ( ( ++m_Underlying ) != m_End && !( m_Underlying )->is_regular_file() );
		return *this;
	}

	inline FileIterator operator++( int )
	{
		FileIterator Iterator( m_Underlying );
		++*this;
		return Iterator;
	}

	inline File operator*()
	{
		return File( m_Underlying->path() );
	}

	inline bool operator==( const FileIterator& a_Iterator ) const
	{
		return m_Underlying == a_Iterator.m_Underlying;
	}

	inline bool operator!=( const FileIterator& a_Iterator ) const
	{
		return m_Underlying != a_Iterator.m_Underlying;
	}

	inline operator bool () const
	{
		return m_Underlying != m_End;
	}

private:

	friend class Directory;

	std::filesystem::directory_iterator m_Underlying;
	std::filesystem::directory_iterator m_End;
};

class FileRecursiveIterator
{
private:

	FileRecursiveIterator( std::filesystem::recursive_directory_iterator a_DirectoryIterator )
		: m_Underlying( a_DirectoryIterator )
	{ }

public:

	FileRecursiveIterator( const Directory& a_Directory )
		: m_Underlying( a_Directory.GetPath().m_Path )
		, m_End( std::filesystem::end( std::filesystem::recursive_directory_iterator( a_Directory.GetPath().m_Path ) ) )
	{
		while ( m_Underlying != m_End && !std::filesystem::is_regular_file( *m_Underlying ) )
		{
			++m_Underlying;
		}
	}

	inline FileRecursiveIterator& operator++()
	{
		_STL_ASSERT( m_Underlying != m_End, "Cannot iterate past end." );
		while ( ( ++m_Underlying ) != m_End && !( m_Underlying )->is_regular_file() );
		return *this;
	}

	inline FileRecursiveIterator operator++( int )
	{
		FileRecursiveIterator Iterator( m_Underlying );
		++* this;
		return Iterator;
	}

	inline File operator*()
	{
		return File( m_Underlying->path() );
	}

	inline bool operator==( const FileRecursiveIterator& a_Iterator ) const
	{
		return m_Underlying == a_Iterator.m_Underlying;
	}

	inline bool operator!=( const FileRecursiveIterator& a_Iterator ) const
	{
		return m_Underlying != a_Iterator.m_Underlying;
	}

	inline operator bool() const
	{
		return m_Underlying != m_End;
	}

private:

	friend class Directory;

	std::filesystem::recursive_directory_iterator m_Underlying;
	std::filesystem::recursive_directory_iterator m_End;
};

typedef StreamSerializer   < File > FileSerializer;
typedef StreamDeserializer < File > FileDeserializer;