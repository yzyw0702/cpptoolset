// testToolset.cpp
#include "MStringTool.hpp"
#include "MPathTool.hpp"
#include "MSystemTool.hpp"
#include "MChipTool.hpp"
#include "MTimeTool.hpp"


int main() {
	// test string toolset
	using namespace debug_toolstring;
	//debug_split();
	
	// test path toolset
	using namespace debug_toolpath;
	//debug_path();
	
	using namespace debug_toolsystem;
	//debug_system();
	
	using namespace debug_toolchip;
	//debug_toolchip();
	
	using namespace debug_tooltime;
	debug_time();
	return 0;
}