# 💾dll-injector
- simple x86 dll injector

## 💉 injection methods
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
## 🎥 showcase
https://github.com/trxgedy/dll-injector/assets/79763373/4516a6b4-c3fa-4d9b-b866-0383d2904025
