# 💾dll-injector
- simple x86 dll injector

# 💉 injection methods
- create remote thread
- thread hijacking
  
TODO:
- manual map
- fake veh

## 📘 Usage
```cpp
int main( )
{
  std::string target = "process.exe";
  std::string dll_path = "path";

  const auto inject_ = std::make_unique<injection::c_injection>( &target, &dll_path );
  inject_->thread_hijacking( );

  std::cin.get( );
  return EXIT_SUCCESS;
}
```
