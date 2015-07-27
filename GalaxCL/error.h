#ifndef GalaxCL_error_h
#define GalaxCL_error_h

#include <assert.h>
#include <stdio.h>
#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

const char* getErrorString(cl_int code);

#define checkError(code) \
	if(code != CL_SUCCESS) {\
	std::cerr << getErrorString(code) << std::endl; \
	assert(code == CL_SUCCESS); \
}

#endif