/* main.cpp */


#include "src/types.h"    //Typedefs & Classes
#include "src/drawing.h"  //Drawing to images.


int main(void) {

	draw::clearBuffer();
	draw::debug();
	draw::saveBuffer("buffer.debug.png");

	return 1;
}