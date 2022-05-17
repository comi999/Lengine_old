#include "Resource.hpp"
#include "File.hpp"

ResourcePackage Resource::s_ResourcePackage( File( "./Resources/resource.package" ) );
ResourceRepository Resource::s_ResourceRepository;