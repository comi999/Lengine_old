#include "File.hpp"
#include <filesystem>

// Path...
Path::Path()
	: m_Path( new std::filesystem::path( "." ) )
{}

Path::Path( const char* a_Path )
	: m_Path( new std::filesystem::path( a_Path ) )
{
	m_Path->make_preferred();
}

Path::Path( const std::filesystem::path& a_Path )
	: m_Path( new std::filesystem::path( a_Path ) )
{
	_ASSERT_EXPR( std::filesystem::exists( a_Path ), "Path does not exist." );
	m_Path->make_preferred();
}

Path::Path( const Path& a_Path )
	: m_Path( new std::filesystem::path( *a_Path.m_Path ) )
{ }

Path::Path( Path&& a_Path )
	: m_Path( a_Path.m_Path )
{ }

Path::~Path()
{
	delete m_Path;
}

inline Path Path::GetAbsolute() const
{
	return std::filesystem::absolute( *m_Path );
}

inline Path Path::GetRelative() const
{
	return std::filesystem::relative( *m_Path );
}

inline Path Path::GetParent() const
{
	return m_Path->parent_path();
}

inline Path Path::GetRoot() const
{
	return m_Path->root_directory();
}

inline bool Path::IsDirectory() const
{
	return std::filesystem::is_directory( *m_Path );
}

inline bool Path::IsFile() const
{
	return std::filesystem::is_regular_file( *m_Path );
}

inline Path::operator const char* () const
{
	return m_Path->string().c_str();
}

inline Path::operator std::string () const
{
	return m_Path->string();
}

inline Path::operator const std::filesystem::path& () const
{
	return *m_Path;
}

inline Path& Path::operator =( const std::filesystem::path& a_Path )
{
	_ASSERT_EXPR( std::filesystem::exists( a_Path ), "Path does not exist." );
	*m_Path = a_Path;
	m_Path->make_preferred();
	return *this;
}

inline Path& Path::operator =( const Path& a_Path )
{
	*m_Path = *a_Path.m_Path;
}

inline bool Path::operator ==( const std::filesystem::path& a_Path ) const
{
	return *m_Path == a_Path;
}

inline bool Path::operator ==( const Path& a_Path ) const
{
	return *m_Path == *a_Path.m_Path;
}

inline bool Path::operator !=( const std::filesystem::path& a_Path ) const
{
	return *m_Path != a_Path;
}

inline bool Path::operator !=( const Path& a_Path ) const
{
	return *m_Path != *a_Path.m_Path;
}

inline Path& Path::operator /=( const char* a_Append )
{
	*m_Path /= a_Append;
	m_Path->make_preferred();
	return *this;
}

inline Path Path::operator /( const char* a_Append ) const
{
	return *m_Path / a_Append;
}

inline Path& Path::operator--()
{
	*m_Path = m_Path->parent_path();
	return *this;
}

inline Path Path::operator--( int )
{
	Path Temp( *m_Path );
	--*this;
	return Temp;
}

// Directory...
Directory::Directory()
	: Path()
{ }

Directory::Directory( const char* a_Path )
	: Path( a_Path )
{
	_ASSERT_EXPR( IsDirectory(), "Path is not a directory." );
}

Directory::Directory( const std::filesystem::path& a_Path )
	: Path( a_Path )
{
	if ( IsFile() )
	{
		--*this;
	}
}

Directory::Directory( const Path& a_Path )
	: Path( a_Path )
{
	if ( IsFile() )
	{
		--*this;
	}
}

Directory::Directory( Path&& a_Path )
	: Path( a_Path )
{
	if ( IsFile() )
	{
		--*this;
	}
}

Directory::Directory( const Directory& a_Directory )
	: Path( a_Directory )
{ }

Directory::Directory( Directory&& a_Directory )
	: Path( a_Directory )
{ }

inline Directory Directory::GetAbsolute() const
{
	return Path::GetAbsolute();
}

inline Directory Directory::GetRelative() const
{
	return Path::GetRelative();
}

inline Directory Directory::GetParent() const
{
	return Path::GetParent();
}

inline Directory Directory::GetRoot() const
{
	return Path::GetRoot();
}

bool Directory::Contains( const char* a_FileOrFolder ) const
{
	auto FileOrFolder = *m_Path / a_FileOrFolder;
	return  std::filesystem::exists( FileOrFolder ) && ( 
			std::filesystem::is_regular_file( FileOrFolder ) || 
			std::filesystem::is_directory( FileOrFolder ) );
}

bool Directory::ContainsFile( const char* a_File ) const
{
	auto File = *m_Path / a_File;
	return std::filesystem::exists( File ) && std::filesystem::is_regular_file( File );
}

File Directory::CreateFile( const char* a_File, size_t a_Size ) const
{
	auto NewFilePath = *m_Path / a_File;

	_ASSERT_EXPR( !std::filesystem::exists( NewFilePath ), "File already exists." );

	FILE* NewFile;
	fopen_s( &NewFile, NewFilePath.string().c_str(), "w+" );
	fseek( NewFile, a_Size, SEEK_SET );
	char Empty = 0;
	fwrite( &Empty, 1, 1, NewFile );
	fclose( NewFile );
	return NewFilePath;
}

File Directory::GetFile( const char* a_File ) const
{
	_ASSERT_EXPR( ContainsFile( a_File ), "File does not exist." );
	return *m_Path / a_File;
}

bool Directory::ContainsDirectory( const char* a_Directory ) const
{
	auto File = *m_Path / a_Directory;
	return std::filesystem::exists( File ) && std::filesystem::is_directory( File );
}

Directory Directory::CreateDirectory( const char* a_Directory ) const
{
	auto NewDirectoryPath = *m_Path / a_Directory;

	_ASSERT_EXPR( !std::filesystem::exists( NewDirectoryPath ), "Directory already exists." );

	std::filesystem::create_directory( NewDirectoryPath );
	return NewDirectoryPath;
}

Directory Directory::GetDirectory( const char* a_Directory )
{
	_ASSERT_EXPR( ContainsDirectory( a_Directory ), "Directory does not exist." );
	return *m_Path / a_Directory;
}

void Directory::SetWorkingDirectory( const Directory& a_Directory )
{
	std::filesystem::current_path( a_Directory );
}

inline Directory::operator const Path& () const
{
	return *this;
}

inline Directory::operator Path& ()
{
	return *this;
}

inline Directory& Directory::operator =( const std::filesystem::path& a_Path )
{
	return *this = a_Path;
}

inline Directory& Directory::operator =( const Path& a_Path )
{
	return *this = a_Path;
}

inline Directory& Directory::operator =( const Directory& a_Path )
{
	return *this = a_Path;
}

inline Directory& Directory::operator /=( const char* a_Append )
{
	_ASSERT_EXPR( ContainsDirectory( a_Append ), "Directory does not exist." );
	return *this /= a_Append;
}

inline Directory  Directory::operator /( const char* a_Append ) const
{
	_ASSERT_EXPR( ContainsDirectory( a_Append ), "Directory does not exist." );
	return *this / a_Append;
}

inline Directory& Directory::operator --()
{
	*m_Path = m_Path->parent_path();
	return *this;
}

inline Directory  Directory::operator --( int )
{
	auto Temp = *Path::m_Path;
	--*this;
	return Temp;
}

// File...
File::File( const char* a_Path )
	: Path( a_Path )
	, m_File( nullptr )
{
	_ASSERT_EXPR( IsFile(), "Path is not a file." );
}

File::File( const std::filesystem::path& a_Path )
	: Path( a_Path )
	, m_File( nullptr )
{
	_ASSERT_EXPR( IsFile(), "Path is not a file." );
}

File::File( const Path& a_Path )
	: Path( a_Path )
	, m_File( nullptr )
{
	_ASSERT_EXPR( IsFile(), "Path is not a file." );
}

File::File( const File& a_File )
	: Path( a_File )
	, m_File( a_File.m_File )
{ }

File::File( Path&& a_Path )
	: Path( a_Path )
	, m_File( nullptr )
{
	_ASSERT_EXPR( IsFile(), "Path is not a file." );
}

File::File( File&& a_File )
	: Path( a_File )
	, m_File( a_File.m_File )
{ }

std::string File::GetName() const
{
	return m_Path->filename().string();
}

std::string File::GetExtension() const
{
	return m_Path->extension().string();
}

std::string File::GetStem() const
{
	return m_Path->stem().string();
}

bool File::Open()
{
	if ( m_File )
	{
		return true;
	}

	return !fopen_s( &m_File, *this, "rb+" );
}

bool File::Close()
{
	if ( !m_File )
	{
		return true;
	}

	m_File = nullptr;
	return !fclose( m_File );
}

inline bool File::IsOpen() const
{
	return m_File;
}

inline void File::Write( const void* a_From, size_t a_Size )
{
	fwrite( a_From, 1, a_Size, m_File );
}

inline void File::Read( void* a_To, size_t a_Size )
{
	fread( a_To, 1, a_Size, m_File );
}

inline void File::Seek( size_t a_Position )
{
	fseek( m_File, a_Position, SEEK_SET );
}

inline bool File::AtEnd() const
{
	return feof( m_File );
}

inline size_t File::Size() const
{
	if ( !m_File )
	{
		return 0;
	}

	return std::filesystem::file_size( *m_Path );
}

inline File::operator Directory () const
{
	return *m_Path;
}

inline File::operator std::ifstream()
{
	return std::ifstream( m_File );
}

inline File::operator std::ofstream()
{
	return std::ofstream( m_File );
}

inline File::operator std::fstream()
{
	return std::fstream( m_File );
}

inline File::operator FILE* ( )
{
	return m_File;
}

// Directory Iterator...
DirectoryIterator::DirectoryIterator( const Directory& a_Directory )
	: m_Underlying( new std::filesystem::directory_iterator( a_Directory ) )
	, m_End( new std::filesystem::directory_iterator( std::filesystem::end( *m_Underlying ) ) )
{ }

DirectoryIterator::DirectoryIterator( const DirectoryIterator& a_Iterator )
	: m_Underlying( new std::filesystem::directory_iterator( *a_Iterator.m_Underlying ) )
	, m_End( new std::filesystem::directory_iterator( *a_Iterator.m_End ) )
{ }

DirectoryIterator::DirectoryIterator( DirectoryIterator&& a_Iterator )
	: m_Underlying( a_Iterator.m_Underlying )
	, m_End( a_Iterator.m_End )
{ }

DirectoryIterator::~DirectoryIterator()
{
	delete m_Underlying;
	delete m_End;
}

inline DirectoryIterator& DirectoryIterator::operator++()
{
	++*m_Underlying;
	return *this;
}

inline DirectoryIterator DirectoryIterator::operator++( int )
{
	DirectoryIterator Temp( *this );
	++*this;
	return Temp;
}

inline DirectoryIterator::reference DirectoryIterator::operator*()
{
	return **m_Underlying;
}

inline DirectoryIterator::pointer DirectoryIterator::operator->()
{
	return &**m_Underlying;
}

inline bool DirectoryIterator::operator==( const DirectoryIterator& a_Iterator ) const
{
	return *m_Underlying == *a_Iterator.m_Underlying;
}

inline bool DirectoryIterator::operator!=( const DirectoryIterator& a_Iterator ) const
{
	return *m_Underlying != *a_Iterator.m_Underlying;
}

inline DirectoryIterator::operator bool () const
{
	return *m_Underlying == *m_End;
}

inline DirectoryIterator::operator Path () const
{
	return ( *m_Underlying )->path();
}

inline DirectoryIterator::operator Directory () const
{
	return ( *m_Underlying )->path();
}

inline DirectoryIterator::operator File () const
{
	return ( *m_Underlying )->path();
}

// Recursive Directory Iterator...
RecursiveDirectoryIterator::RecursiveDirectoryIterator( const Directory& a_Directory )
	: m_Underlying( new std::filesystem::recursive_directory_iterator( a_Directory ) )
	, m_End( new std::filesystem::recursive_directory_iterator( std::filesystem::end( *m_Underlying ) ) )
{ }

RecursiveDirectoryIterator::RecursiveDirectoryIterator( const RecursiveDirectoryIterator& a_Iterator )
	: m_Underlying( new std::filesystem::recursive_directory_iterator( *a_Iterator.m_Underlying ) )
	, m_End( new std::filesystem::recursive_directory_iterator( *a_Iterator.m_End ) )
{ }

RecursiveDirectoryIterator::RecursiveDirectoryIterator( RecursiveDirectoryIterator&& a_Iterator )
	: m_Underlying( a_Iterator.m_Underlying )
	, m_End( a_Iterator.m_End )
{ }

RecursiveDirectoryIterator::~RecursiveDirectoryIterator()
{
	delete m_Underlying;
	delete m_End;
}

inline RecursiveDirectoryIterator& RecursiveDirectoryIterator::operator++()
{
	++*m_Underlying;
	return *this;
}

inline RecursiveDirectoryIterator RecursiveDirectoryIterator::operator++( int )
{
	RecursiveDirectoryIterator Temp( *this );
	++*this;
	return Temp;
}

inline RecursiveDirectoryIterator::reference RecursiveDirectoryIterator::operator*()
{
	return **m_Underlying;
}

inline RecursiveDirectoryIterator::pointer RecursiveDirectoryIterator::operator->()
{
	return &**m_Underlying;
}

inline bool RecursiveDirectoryIterator::operator==( const RecursiveDirectoryIterator& a_Iterator ) const
{
	return *m_Underlying == *a_Iterator.m_Underlying;
}

inline bool RecursiveDirectoryIterator::operator!=( const RecursiveDirectoryIterator& a_Iterator ) const
{
	return *m_Underlying != *a_Iterator.m_Underlying;
}

inline RecursiveDirectoryIterator::operator bool () const
{
	return *m_Underlying == *m_End;
}

inline RecursiveDirectoryIterator::operator Path () const
{
	return ( *m_Underlying )->path();
}

inline RecursiveDirectoryIterator::operator Directory () const
{
	return ( *m_Underlying )->path();
}

inline RecursiveDirectoryIterator::operator File () const
{
	return ( *m_Underlying )->path();
}