#include <iostream>
#include "ResourcePackager.hpp"
#include "File.hpp"
#include "Texture.hpp" 
#include "ResourcePackage.hpp"

int main( int argc, char** argv )
{
    if ( argc != 2 )
    {
        return 1;
    }

    ResourcePackager Packager;
    Packager.BuildPackage( Directory( argv[ 1 ] ).GetAbsolute() );
}