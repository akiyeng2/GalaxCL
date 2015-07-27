#include <iostream>
#include "GalaxSee.h" 

int main(int argc, const char * argv[]) {
	// insert code here...
	GalaxSee galaxy(512, initialConfigurations::GRID);
	galaxy.printJSON(1800);
	return 0;
}
