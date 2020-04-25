#include "sciter-x.h"
#include "sciter-x-window.hpp"
#include <string>
#include <codecvt>
#include <locale>
#include <utility>


extern std::string runPipeline(int va11);
extern void run_pipeline_real(int);
extern std::string getProg();
extern std::string get_total_pipeline_info();
extern std::string show_cache_values();
extern std::string show_register_bank_values();
extern std::string show_file();
using namespace sciter;

class frame: public sciter::window, sciter::event_handler {
public:
  frame() : window(SW_TITLEBAR | SW_RESIZEABLE | SW_CONTROLS | SW_MAIN | SW_ENABLE_DEBUG) {}

   
  // map of native functions exposed to script:
  BEGIN_FUNCTION_MAP
    FUNCTION_0("nativeMessage", nativeMessage);
    FUNCTION_0("showCache",showCache);
    FUNCTION_1("run_pipeline",run_pipeline);
    FUNCTION_1("runPipelineUI", runPipelineUI);
    FUNCTION_0("getPc", getPc);
    FUNCTION_0("getPipelineInfo", getPipelineInfo);
    FUNCTION_0("getRegs", getRegs);
    FUNCTION_0("getProgram", getProgram);
  END_FUNCTION_MAP
  // function expsed to script:
  sciter::string  nativeMessage() { return WSTR("Hello C++ World"); }

  sciter::string getProgram() {
    
    std::string ans = show_file();
        std::cout << "Answer is: " << ans << std::endl;
    
		using convert_typeX = std::codecvt_utf8_utf16<char16_t>;
		std::wstring_convert<convert_typeX , char16_t> converterX;
		return converterX.from_bytes(ans);
  }
  
  sciter::string showCache() {
      std::string ans = show_cache_values();
        std::cout << "Answer is: " << ans << std::endl;
    
		using convert_typeX = std::codecvt_utf8_utf16<char16_t>;
		std::wstring_convert<convert_typeX , char16_t> converterX;
		return converterX.from_bytes(ans);
  }
  sciter::string runPipelineUI(sciter::value val1) {
      int a;
        std::string ans = runPipeline(val1.get(a));
    
    
		using convert_typeX = std::codecvt_utf8_utf16<char16_t>;
		std::wstring_convert<convert_typeX , char16_t> converterX;
		return converterX.from_bytes(ans);
  }
  sciter::string run_pipeline(sciter::value val) {
      int a = val.get(a);
      
      std::cout << "Printing from the function " << a << std::endl;
      run_pipeline_real(a);
      return WSTR("RAN!");
  } 
  sciter::string getPc() {
      std::cout << "Searching for Prog counter" << std::endl;
      std::string ans = getProg();
      using convert_typeX = std::codecvt_utf8_utf16<char16_t>;
		std::wstring_convert<convert_typeX , char16_t> converterX;
		return converterX.from_bytes(ans);
  }

  sciter::string getRegs() {

    std::string ans = show_register_bank_values();
    using convert_typeX = std::codecvt_utf8_utf16<char16_t>;
    std::wstring_convert<convert_typeX, char16_t> converterX;
    return converterX.from_bytes(ans);
  }

  sciter::string getPipelineInfo() {
       std::string ans = get_total_pipeline_info();
      using convert_typeX = std::codecvt_utf8_utf16<char16_t>;
		std::wstring_convert<convert_typeX , char16_t> converterX;
		return converterX.from_bytes(ans);
  }
};

#include "resources.cpp" // resources packaged into binary blob.

int uimain(std::function<int()> run ) {

  // enable features you may need in your scripts:
  SciterSetOption(NULL, SCITER_SET_SCRIPT_RUNTIME_FEATURES,
    ALLOW_FILE_IO |
    ALLOW_SOCKET_IO | // NOTE: the must for communication with Inspector
    ALLOW_EVAL |
    ALLOW_SYSINFO);

  sciter::archive::instance().open(aux::elements_of(resources)); // bind resources[] (defined in "resources.cpp") with the archive

  aux::asset_ptr<frame> pwin = new frame();

  // note: this:://app URL is dedicated to the sciter::archive content associated with the application
  pwin->load( WSTR("this://app/main.htm") );
  //or use this to load UI from  
  //  pwin->load( WSTR("file:///home/andrew/Desktop/Project/res/main.htm") );
  
  pwin->expand();

  return run();
}