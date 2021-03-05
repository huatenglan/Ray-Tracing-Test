#ifndef RTWEEKEND_STB_IMAGE_H
#define RTWEEKEND_STB_IMAGE_H

//Disable pedantic(À¬»ø) warnings for this external library
#ifdef _MSC_VER
	//Microsoft Visual C++ Compiler
	#pragma warning(push,0)
#endif // !_MSC_VER

#define STB_IMAGE_IMPLEMENTATION
#include "../../../lmy/raytracing.github.io-master/src/common/external/stb_image.h"

//Restore(»Ö¸´) warning levels
#ifdef _MSC_VER
	//Microsoft Visual C++ Compiler
	#pragma warning(pop)
#endif // _MSC_VER

#endif // !RTWEEKEND_STB_IMAGE_H
