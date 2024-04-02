#include "stdafx.hpp"
#include "injection\injection.hpp"

int main( )
{
    std::string target = "process.exe";
    std::string dll_path = "path";

    const auto inject_ = std::make_unique<injection::c_injection>( &target, &dll_path );
    inject_->thread_hijacking( );

    std::cin.get( );
    return EXIT_SUCCESS;
}