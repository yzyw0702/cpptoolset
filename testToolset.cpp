// testToolset.cpp
#include "MStringTool.hpp"
#include "MPathTool.hpp"
#include "MSystemTool.hpp"


int main() {
	// test string toolset
	using namespace debug_toolstring;
	//debug_split();
	
	// test path toolset
	using namespace debug_toolpath;
	//debug_path();
	
	using namespace debug_toolsystem;
	debug_system();
	return 0;
}