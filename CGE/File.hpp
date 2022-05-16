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

class File;
class FileIterator;

class Path
{
public:

	Path();
	Path( const char* a_Path );
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
	Directory( const char* a_Path );
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
	inline bool IsOpen() const;
	inline void Write( const void* a_From, size_t a_Size );
	inline void Read( void* a_To, size_t a_Size );
	inline void Seek( size_t a_Position );
	inline bool AtEnd() const;
	inline size_t Size() const;
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
	inline DirectoryIterator& operator++();
	inline DirectoryIterator operator++( int );
	inline reference operator*();
	inline pointer operator->();
	inline bool operator==( const DirectoryIterator& a_Iterator ) const;
	inline bool operator!=( const DirectoryIterator& a_Iterator ) const;
	inline operator bool () const;
	inline operator Path () const;
	inline operator Directory () const;
	inline operator File () const;

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
	inline RecursiveDirectoryIterator& operator++();
	inline RecursiveDirectoryIterator operator++( int );
	inline reference operator*();
	inline pointer operator->();
	inline bool operator==( const RecursiveDirectoryIterator& a_Iterator ) const;
	inline bool operator!=( const RecursiveDirectoryIterator& a_Iterator ) const;
	inline operator bool () const;
	inline operator Path () const;
	inline operator Directory () const;
	inline operator File () const;

private:

	std::filesystem::recursive_directory_iterator* m_Underlying;
	std::filesystem::recursive_directory_iterator* m_End;
};

typedef StreamSerializer   < File > FileSerializer;
typedef StreamDeserializer < File > FileDeserializer;