#pragma once
#include <fstream>
#include "Serialization.hpp"

namespace std
{
	namespace filesystem
	{
		class path;
		class directory_entry;
		class directory_iterator;
		class recursive_directory_iterator;
	}
}

class Path;
class Directory;
class File;
class DirectoryIterator;
class RecursiveDirectoryIterator;

class Path
{
public:

	Path();
	Path( const char* a_Path );
	Path( const std::filesystem::path& a_Path );
	Path( const Path& a_Path );
	Path( Path&& a_Path );
	~Path();
	Path GetAbsolute() const;
	Path GetRelative() const;
	Path GetParent() const;
	Path GetRoot() const;
	bool IsDirectory() const;
	bool IsFile() const;
	operator std::string () const;
	operator const std::filesystem::path& () const;
	Path& operator =( const std::filesystem::path& a_Path );
	Path& operator =( const Path& a_Path );
	bool operator ==( const std::filesystem::path& a_Path ) const;
	bool operator ==( const Path& a_Path ) const;
	bool operator !=( const std::filesystem::path& a_Path ) const;
	bool operator !=( const Path& a_Path ) const;
	Path& operator /=( const char* a_Append );
	Path  operator /( const char* a_Append ) const;
	Path& operator --();
	Path  operator --( int );

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
	Directory( const char* a_Path );
	Directory( const std::filesystem::path& a_Path );
	Directory( const Path& a_Path );
	Directory( Path&& a_Path );
	Directory( const Directory& a_Directory );
	Directory( Directory&& a_Directory );
	Directory GetAbsolute() const;
	Directory GetRelative() const;
	Directory GetParent() const;
	Directory GetRoot() const;
	bool Contains( const char* a_FileOrFolder ) const;
	bool ContainsFile( const char* a_File ) const;
	File NewFile( const char* a_File, size_t a_Size ) const;
	bool DestroyFile( const char* a_File ) const;
	File GetFile( const char* a_File ) const;
	bool ContainsDirectory( const char* a_Directory ) const;
	Directory NewDirectory( const char* a_Directory ) const;
	bool DestroyDirectory( const char* a_Directory ) const;
	Directory GetDirectory( const char* a_Directory ) const;
	operator const Path& () const;
	operator Path& ();
	Directory& operator =( const std::filesystem::path& a_Path );
	Directory& operator =( const Path& a_Path );
	Directory& operator =( const Directory& a_Path );
	Directory& operator /=( const char* a_Append );
	Directory  operator /( const char* a_Append ) const;
	Directory& operator --();
	Directory  operator --( int );
	static void SetWorkingDirectory( const Directory& a_Directory );
	static Directory Create( const char* a_Path );
};

class File : public Path
{
public:

	File( const char* a_Path );
	File( const std::filesystem::path& a_Path );
	File( const Path& a_Path );
	File( Path&& a_Path );
	File( const File& a_File );
	File( File&& a_File );
	std::string GetName() const;
	std::string GetExtension() const;
	std::string GetStem() const;
	bool Open();
	bool Close();
	bool IsOpen() const;
	void Write( const void* a_From, size_t a_Size );
	void Read( void* a_To, size_t a_Size );
	size_t Tell() const;
	void Seek( size_t a_Position );
	bool AtEnd() const;
	size_t Size() const;
	File& operator =( const File& a_File );
	operator Directory () const;
	operator std::ifstream ();
	operator std::ofstream ();
	operator std::fstream (); 
	operator FILE* ();

private:

	friend class FileIterator;
	template < typename > friend class Serializer;
	template < typename > friend class Deserializer;

	FILE* m_File;
};

class DirectoryIterator
{
public:

	using iterator_category = std::input_iterator_tag;
	using value_type        = const std::filesystem::directory_entry;
    using difference_type   = std::ptrdiff_t;
    using pointer           = const std::filesystem::directory_entry*;
    using reference         = const std::filesystem::directory_entry&;

	DirectoryIterator( const Directory& a_Directory );
	DirectoryIterator( const DirectoryIterator& a_Iterator );
	DirectoryIterator( DirectoryIterator&& a_Iterator );
	~DirectoryIterator();
	DirectoryIterator& operator++();
	DirectoryIterator operator++( int );
	reference operator*();
	pointer operator->();
	bool operator==( const DirectoryIterator& a_Iterator ) const;
	bool operator!=( const DirectoryIterator& a_Iterator ) const;
	operator bool () const;
	operator Path () const;
	operator Directory () const;
	operator File () const;

private:

	std::filesystem::directory_iterator* m_Underlying;
	std::filesystem::directory_iterator* m_End;
};

class RecursiveDirectoryIterator
{
public:

	using iterator_category = std::input_iterator_tag;
	using value_type        = const std::filesystem::directory_entry;
    using difference_type   = std::ptrdiff_t;
    using pointer           = const std::filesystem::directory_entry*;
    using reference         = const std::filesystem::directory_entry&;

	RecursiveDirectoryIterator( const Directory& a_Directory );
	RecursiveDirectoryIterator( const RecursiveDirectoryIterator& a_Iterator );
	RecursiveDirectoryIterator( RecursiveDirectoryIterator&& a_Iterator );
	~RecursiveDirectoryIterator();
	RecursiveDirectoryIterator& operator++();
	RecursiveDirectoryIterator operator++( int );
	reference operator*();
	pointer operator->();
	bool operator==( const RecursiveDirectoryIterator& a_Iterator ) const;
	bool operator!=( const RecursiveDirectoryIterator& a_Iterator ) const;
	operator bool () const;
	operator Path () const;
	operator Directory () const;
	operator File () const;

private:

	std::filesystem::recursive_directory_iterator* m_Underlying;
	std::filesystem::recursive_directory_iterator* m_End;
};

typedef StreamSerializer   < File > FileSerializer;
typedef StreamDeserializer < File > FileDeserializer;