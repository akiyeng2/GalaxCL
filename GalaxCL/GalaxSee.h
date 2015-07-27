#ifndef __GalaxCL__GalaxSee__
#define __GalaxCL__GalaxSee__

#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#include <cmath>

typedef struct {
	float x;
	float y;
	float z;
} vec;

typedef struct {
	unsigned long mass;
	vec position;
	vec velocity;
	vec acceleration;
} star;

enum class initialConfigurations {SPHERE, CUBE, GRID, DISK};
enum class initialBehaviors {NONE, ROTATE};

class GalaxSee {
	
	cl_int errorCode;
	cl_context context;
	cl_command_queue commandQueue;
	cl_program program;
	cl_kernel kernel;
	
	star *stars;
	cl_mem starBuffer;
	
	cl_device_id device;
	
	int numStars;
	size_t starBufferSize;
	
	size_t localWorkSize[1], globalWorkSize[1];

	
public:
	GalaxSee(int, initialConfigurations);
	
	void initOpenCL(void);
	void execute(void);
	void printJSON(int);
};

#endif /* defined(__GalaxCL__GalaxSee__) */
