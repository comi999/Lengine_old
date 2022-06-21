#include <iostream>
#include "ResourcePackager.hpp"
#include "File.hpp"
#include "Texture.hpp" 
#include "ResourcePackage.hpp"

int main( int argc, char** argv )
{
    ResourcePackager Packager;
    if ( argc == 1 )      Packager.BuildPackage( Directory().GetAbsolute() );
    else if ( argc == 2 ) Packager.BuildPackage( Directory( argv[ 1 ] ).GetAbsolute() );
}