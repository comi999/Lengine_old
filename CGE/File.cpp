#include "File.hpp"

#include <filesystem>
#undef DestroyFile

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

Path Path::GetAbsolute() const
{
	return std::filesystem::absolute( *m_Path );
}

Path Path::GetRelative() const
{
	return std::filesystem::relative( *m_Path );
}

Path Path::GetParent() const
{
	return m_Path->parent_path();
}

Path Path::GetRoot() const
{
	return m_Path->root_directory();
}

bool Path::IsDirectory() const
{
	return std::filesystem::is_directory( *m_Path );
}

bool Path::IsFile() const
{
	return std::filesystem::is_regular_file( *m_Path );
}

Path::operator std::string () const
{
	return m_Path->string();
}

Path::operator const std::filesystem::path& () const
{
	return *m_Path;
}

Path& Path::operator =( const std::filesystem::path& a_Path )
{
	_ASSERT_EXPR( std::filesystem::exists( a_Path ), "Path does not exist." );
	*m_Path = a_Path;
	m_Path->make_preferred();
	return *this;
}

Path& Path::operator =( const Path& a_Path )
{
	*m_Path = *a_Path.m_Path;
	return *this;
}

bool Path::operator ==( const std::filesystem::path& a_Path ) const
{
	return *m_Path == a_Path;
}

bool Path::operator ==( const Path& a_Path ) const
{
	return *m_Path == *a_Path.m_Path;
}

bool Path::operator !=( const std::filesystem::path& a_Path ) const
{
	return *m_Path != a_Path;
}

bool Path::operator !=( const Path& a_Path ) const
{
	return *m_Path != *a_Path.m_Path;
}

Path& Path::operator /=( const char* a_Append )
{
	*m_Path /= a_Append;
	m_Path->make_preferred();
	return *this;
}

Path Path::operator /( const char* a_Append ) const
{
	return *m_Path / a_Append;
}

Path& Path::operator--()
{
	*m_Path = m_Path->parent_path();
	return *this;
}

Path Path::operator--( int )
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

Directory Directory::GetAbsolute() const
{
	return Path::GetAbsolute();
}

Directory Directory::GetRelative() const
{
	return Path::GetRelative();
}

Directory Directory::GetParent() const
{
	return Path::GetParent();
}

Directory Directory::GetRoot() const
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

File Directory::NewFile( const char* a_File, size_t a_Size ) const
{
	auto NewFilePath = *m_Path / a_File;

	_ASSERT_EXPR( !std::filesystem::exists( NewFilePath ), "File already exists." );

	FILE* NewFile;
	fopen_s( &NewFile, NewFilePath.string().c_str(), "w+" );
	fseek( NewFile, a_Size - 1, SEEK_SET );
	char Empty = 0;
	fwrite( &Empty, 1, 1, NewFile );
	fclose( NewFile );
	return NewFilePath;
}

bool Directory::DestroyFile( const char* a_File ) const
{
	if ( !ContainsFile( a_File ) )
	{
		return false;
	}

	return std::filesystem::remove( GetFile( a_File ) );
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

Directory Directory::NewDirectory( const char* a_Directory ) const
{
	auto NewDirectoryPath = *m_Path / a_Directory;

	_ASSERT_EXPR( !std::filesystem::exists( NewDirectoryPath ), "Directory already exists." );

	std::filesystem::create_directory( NewDirectoryPath );
	return NewDirectoryPath;
}

bool Directory::DestroyDirectory( const char* a_Directory ) const
{
	if ( !ContainsDirectory( a_Directory ) )
	{
		return false;
	}

	return std::filesystem::remove_all( GetDirectory( a_Directory ) );
}

Directory Directory::GetDirectory( const char* a_Directory ) const
{
	_ASSERT_EXPR( ContainsDirectory( a_Directory ), "Directory does not exist." );
	return *m_Path / a_Directory;
}

Directory::operator const Path& () const
{
	return *this;
}

Directory::operator Path& ()
{
	return *this;
}

Directory& Directory::operator =( const std::filesystem::path& a_Path )
{
	return *this = a_Path;
}

Directory& Directory::operator =( const Path& a_Path )
{
	return *this = a_Path;
}

Directory& Directory::operator =( const Directory& a_Path )
{
	*this->m_Path = *a_Path.m_Path;
	return *this;
}

Directory& Directory::operator /=( const char* a_Append )
{
	_ASSERT_EXPR( ContainsDirectory( a_Append ), "Directory does not exist." );
	return *this /= a_Append;
}

Directory  Directory::operator /( const char* a_Append ) const
{
	_ASSERT_EXPR( ContainsDirectory( a_Append ), "Directory does not exist." );
	return *this / a_Append;
}

Directory& Directory::operator --()
{
	*m_Path = m_Path->parent_path();
	return *this;
}

Directory  Directory::operator --( int )
{
	auto Temp = *Path::m_Path;
	--*this;
	return Temp;
}

void Directory::SetWorkingDirectory( const Directory& a_Directory )
{
	std::filesystem::current_path( a_Directory );
}

Directory Directory::Create( const char* a_Path )
{
	auto NewDirectory = std::filesystem::path( a_Path );
	_ASSERT_EXPR( std::filesystem::create_directories( NewDirectory ), "Cannot create directory." );
	return NewDirectory;
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

	std::string ResourceFilePath = *this;
	return !fopen_s( &m_File, ResourceFilePath.c_str(), "rb+" );
}

bool File::Close()
{
	if ( !m_File )
	{
		return true;
	}

	auto Error = fclose( m_File );
	m_File = nullptr;
	return !Error;
}

bool File::IsOpen() const
{
	return m_File;
}

void File::Write( const void* a_From, size_t a_Size )
{
	size_t siz = fwrite( a_From, 1, a_Size, m_File );
}

void File::Read( void* a_To, size_t a_Size )
{
	fread( a_To, 1, a_Size, m_File );
}

size_t File::Tell() const
{
	return ftell( m_File );
}

void File::Seek( size_t a_Position )
{
	fseek( m_File, a_Position, SEEK_SET );
}

bool File::AtEnd() const
{
	return feof( m_File );
}

size_t File::Size() const
{
	if ( !m_File )
	{
		return 0;
	}

	return std::filesystem::file_size( *m_Path );
}

File& File::operator =( const File& a_File )
{
	m_Path = a_File.m_Path;
	m_File = a_File.m_File;
	return *this;
}

File::operator Directory () const
{
	return *m_Path;
}

File::operator std::ifstream()
{
	return std::ifstream( m_File );
}

File::operator std::ofstream()
{
	return std::ofstream( m_File );
}

File::operator std::fstream()
{
	return std::fstream( m_File );
}

File::operator FILE* ( )
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

DirectoryIterator& DirectoryIterator::operator++()
{
	++*m_Underlying;
	return *this;
}

DirectoryIterator DirectoryIterator::operator++( int )
{
	DirectoryIterator Temp( *this );
	++*this;
	return Temp;
}

DirectoryIterator::reference DirectoryIterator::operator*()
{
	return **m_Underlying;
}

DirectoryIterator::pointer DirectoryIterator::operator->()
{
	return &**m_Underlying;
}

bool DirectoryIterator::operator==( const DirectoryIterator& a_Iterator ) const
{
	return *m_Underlying == *a_Iterator.m_Underlying;
}

bool DirectoryIterator::operator!=( const DirectoryIterator& a_Iterator ) const
{
	return *m_Underlying != *a_Iterator.m_Underlying;
}

DirectoryIterator::operator bool () const
{
	return *m_Underlying != *m_End;
}

DirectoryIterator::operator Path () const
{
	return ( *m_Underlying )->path();
}

DirectoryIterator::operator Directory () const
{
	return ( *m_Underlying )->path();
}

DirectoryIterator::operator File () const
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

RecursiveDirectoryIterator& RecursiveDirectoryIterator::operator++()
{
	++*m_Underlying;
	return *this;
}

RecursiveDirectoryIterator RecursiveDirectoryIterator::operator++( int )
{
	RecursiveDirectoryIterator Temp( *this );
	++*this;
	return Temp;
}

RecursiveDirectoryIterator::reference RecursiveDirectoryIterator::operator*()
{
	return **m_Underlying;
}

RecursiveDirectoryIterator::pointer RecursiveDirectoryIterator::operator->()
{
	return &**m_Underlying;
}

bool RecursiveDirectoryIterator::operator==( const RecursiveDirectoryIterator& a_Iterator ) const
{
	return *m_Underlying == *a_Iterator.m_Underlying;
}

bool RecursiveDirectoryIterator::operator!=( const RecursiveDirectoryIterator& a_Iterator ) const
{
	return *m_Underlying != *a_Iterator.m_Underlying;
}

RecursiveDirectoryIterator::operator bool () const
{
	return *m_Underlying == *m_End;
}

RecursiveDirectoryIterator::operator Path () const
{
	return ( *m_Underlying )->path();
}

RecursiveDirectoryIterator::operator Directory () const
{
	return ( *m_Underlying )->path();
}

RecursiveDirectoryIterator::operator File () const
{
	return ( *m_Underlying )->path();
}