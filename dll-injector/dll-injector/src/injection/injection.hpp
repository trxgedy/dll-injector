#ifndef injection_hpp
#define injection_hpp

namespace injection
{
	class c_injection
	{
	private:
		int pid = {};
		HANDLE handle = {};
		std::string dll_path;

	public:
		c_injection( const std::string *target, const std::string *path );
		~c_injection( );

		void create_thread( );
		void thread_hijacking( );
	};
}

#endif // !injection_hpp