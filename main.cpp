#include "sciter-x.h"
#include "sciter-x-window.hpp"
#include <string>
#include <codecvt>
#include <locale>
#include <utility>


extern std::string runPipeline(int va11, std::string val2);
extern void run_pipeline_real(int, std::string);
extern std::string getProg();
extern std::string get_total_pipeline_info();
extern std::string show_cache_values();
extern std::string show_register_bank_values();
extern std::string show_file(std::string a);
extern std::string getTotalCycles();
using namespace sciter;

class frame: public sciter::window, sciter::event_handler {
public:
  frame() : window(SW_TITLEBAR | SW_RESIZEABLE | SW_CONTROLS | SW_MAIN | SW_ENABLE_DEBUG) {}

   
  // map of native functions exposed to script:
  BEGIN_FUNCTION_MAP
    FUNCTION_0("nativeMessage", nativeMessage);
    FUNCTION_0("showCache",showCache);
    FUNCTION_2("run_pipeline",run_pipeline);
    FUNCTION_2("runPipelineUI", runPipelineUI);
    FUNCTION_0("getPc", getPc);
    FUNCTION_0("getPipelineInfo", getPipelineInfo);
    FUNCTION_0("getRegs", getRegs);
    FUNCTION_1("getProgram", getProgram);
    FUNCTION_0("total_cycles", total_cycles);
  END_FUNCTION_MAP
  // function expsed to script:
  sciter::string  nativeMessage() { return WSTR("Hello C++ World"); }

  sciter::string total_cycles() {
    using convert_typeX = std::codecvt_utf8_utf16<char16_t>;
    std::wstring_convert<convert_typeX, char16_t> converterX;
    std::string ans = getTotalCycles();
    return converterX.from_bytes(ans);
  }
  
  sciter::string getProgram(sciter::value val) {
    using convert_typeX = std::codecvt_utf8_utf16<char16_t>;
    std::wstring_convert<convert_typeX, char16_t> converterX;
    sciter::string val_string = val.get((WCHAR *)val_string.c_str());
    std::string a = converterX.to_bytes(val_string);
    std::string ans = show_file(a);
    return converterX.from_bytes(ans);
  }
  
  sciter::string showCache() {
    std::string ans = show_cache_values();
    using convert_typeX = std::codecvt_utf8_utf16<char16_t>;
		std::wstring_convert<convert_typeX , char16_t> converterX;
		return converterX.from_bytes(ans);
  }
  sciter::string runPipelineUI(sciter::value val1, sciter::value val2) {
    using convert_typeX = std::codecvt_utf8_utf16<char16_t>;
		std::wstring_convert<convert_typeX , char16_t> converterX;
    int a;
    sciter::string val_string = val2.get((WCHAR *)val_string.c_str());
    std::string file = converterX.to_bytes(val_string);
    std::string ans = runPipeline(val1.get(a), file);
    
		return converterX.from_bytes(ans);
  }
  sciter::string run_pipeline(sciter::value val, sciter::value val2) {
    using convert_typeX = std::codecvt_utf8_utf16<char16_t>;
    std::wstring_convert<convert_typeX, char16_t> converterX;
      int a = val.get(a);
    sciter::string val_string = val2.get((WCHAR *)val_string.c_str());
    std::string config = converterX.to_bytes(val_string);
      run_pipeline_real(a, config);
      return WSTR("RAN!");
  } 
  sciter::string getPc() {
     
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