#include "stdafx.hpp"
#include "injection.hpp"

namespace injection
{
	c_injection::c_injection( const std::string *target, const std::string *path )
	{
		this->pid = utils::process::get_process_id( *target ).value( );
		this->dll_path = *path;

		if ( GetFileAttributesA( this->dll_path.c_str( ) ) == INVALID_FILE_ATTRIBUTES )
		{
			std::cout << "dll not found";
			this->~c_injection( );
		}

		if ( this->pid != NULL )
		{
			this->handle = OpenProcess( PROCESS_ALL_ACCESS, false, this->pid );
		}
	}

	c_injection::~c_injection( )
	{
		if ( this->handle != nullptr && this->handle != INVALID_HANDLE_VALUE )
			CloseHandle( this->handle );
	}

	void c_injection::create_thread( )
	{
		const auto address = VirtualAllocEx( this->handle, 0, this->dll_path.length( ), MEM_COMMIT, PAGE_READWRITE );

		if ( !WriteProcessMemory( this->handle, address, ( LPVOID ) this->dll_path.c_str( ), this->dll_path.length( ), 0 ) )
			MessageBoxA( nullptr, "failed while writting dll path to the memory", "error", MB_OK | MB_ICONERROR );

		const auto thread_handle = CreateRemoteThread( this->handle, NULL, NULL, ( LPTHREAD_START_ROUTINE ) LoadLibraryA, address, 0, 0 );

		WaitForSingleObject( thread_handle, INFINITE );

		VirtualFreeEx( this->handle, address, 0, MEM_RELEASE );
		CloseHandle( thread_handle );
	}

	void c_injection::thread_hijacking( )
	{
		const int thread_id = utils::process::get_thread_id( this->pid ).value( );
		const safe_handle thread_handle( OpenThread( THREAD_ALL_ACCESS, false, thread_id ) );

		if ( thread_handle.get( ) == INVALID_HANDLE_VALUE )
		{
			printf( "failed to open thread handle" );
			return;
		}

		char shell_code[ ] =
		{
			0x60,								 // pushad
			0xE8, 0x00, 0x00, 0x00, 0x00,		 // call $+5 (to be filled in later)
			0x5B,								 // pop ebx
			0x81, 0xEB, 0x06, 0x00, 0x00, 0x00,  // sub ebx, 6
			0xB8, 0xCC, 0xCC, 0xCC, 0xCC,		 // mov eax, LoadLibraryA (placeholder)
			0x8D, 0x93, 0x22, 0x00, 0x00, 0x00,  // lea edx, [ebx+22]
			0x52,								 // push edx
			0xFF, 0xD0,							 // call eax
			0x61,								 // popad
			0x68, 0xCC, 0xCC, 0xCC, 0xCC,		 // push returnAddress (placeholder)
			0xC3								 // ret
		};


		auto address = VirtualAllocEx( this->handle, NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE );
		auto buffer = std::make_unique<char[ ]>( 256 );
		auto ptr = reinterpret_cast< LPBYTE >( buffer.get( ) );

		CONTEXT ctx;
		ctx.ContextFlags = CONTEXT_ALL;

		SuspendThread( thread_handle.get( ) );
		GetThreadContext( thread_handle.get( ), &ctx );

		memcpy( buffer.get( ), shell_code, sizeof( shell_code ) );

		*reinterpret_cast< DWORD * >( ptr + 14 ) = ( DWORD ) LoadLibraryA;
		*reinterpret_cast< DWORD * >( ptr + 29 ) = ctx.Eip;
		reinterpret_cast< LPBYTE > ( ptr ) += 34;

		strcpy( reinterpret_cast< char * > ( ptr ), this->dll_path.c_str( ) );

		if ( !WriteProcessMemory( this->handle, address, buffer.get( ), sizeof( shell_code ) + strlen( reinterpret_cast< char * > ( ptr ) ), nullptr ) )
			VirtualFreeEx( this->handle, address, NULL, MEM_RELEASE );

		ctx.Eip = reinterpret_cast< DWORD >( address );

		if ( !SetThreadContext( thread_handle.get( ), &ctx ) )
			VirtualFreeEx( this->handle, address, NULL, MEM_RELEASE );

		ResumeThread( thread_handle.get( ) );
	}
}