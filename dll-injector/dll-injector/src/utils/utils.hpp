#ifndef utils_hpp
#define utils_hpp

#include "stdafx.hpp"

namespace utils
{
	namespace implements
	{
		struct handle_deleter
		{
			void operator()( HANDLE handle ) const
			{
				if ( handle != nullptr && handle != INVALID_HANDLE_VALUE )
				{
					CloseHandle( handle );
				}
			}
		};
	}

	namespace process
	{
		__inline std::optional<int> get_process_id( const std::string &proc_name )
		{
			const auto handle = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

			if ( handle )
			{
				PROCESSENTRY32 entry;
				entry.dwSize = sizeof( entry );

				do
				{
					if ( proc_name.compare( entry.szExeFile ) == 0 )
					{
						CloseHandle( handle );
						return entry.th32ProcessID;
					}
				} while ( Process32Next( handle, &entry ) );
			}

			CloseHandle( handle );
			return 0;
		}

		__inline std::optional<int> get_thread_id( const int pid )
		{
			const auto handle = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 );

			if ( handle )
			{
				THREADENTRY32 entry {};
				entry.dwSize = sizeof( entry );

				do
				{
					if ( entry.th32OwnerProcessID == pid )
					{
						CloseHandle( handle );
						return entry.th32ThreadID;
					}
				} while ( Thread32Next( handle, &entry ) );
			}

			CloseHandle( handle );
			return 0;
		}
	}
}

using safe_handle = std::unique_ptr<std::remove_pointer<HANDLE>::type, utils::implements::handle_deleter>;

#endif // !utils_hpp