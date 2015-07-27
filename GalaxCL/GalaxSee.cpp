#include "GalaxSee.h"

#include <iostream>
#include <string.h>
#include "error.h"
#include "readFile.h"

//Length, width, and height of the galaxy box
#define GALAXY_SIZE (100.0)

void GalaxSee::initOpenCL() {
	cl_uint numPlatforms;
	errorCode = clGetPlatformIDs(0, NULL, &numPlatforms);
	cl_platform_id platforms[numPlatforms];
	errorCode = clGetPlatformIDs(numPlatforms, platforms, NULL);
	checkError(errorCode);
	
	//The double cast is for some needed in C++14, but not in C99
	cl_context_properties properties[] = {CL_CONTEXT_PLATFORM, (int)(size_t)(platforms[0]), 0};
	context = clCreateContextFromType(properties, CL_DEVICE_TYPE_GPU, 0, NULL, &errorCode);
	checkError(errorCode);
	
	size_t dataBytes;
	errorCode = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &dataBytes);
	cl_device_id* devices = (cl_device_id*) malloc(dataBytes);
	checkError(errorCode);
	errorCode |= clGetContextInfo(context, CL_CONTEXT_DEVICES, dataBytes, device, NULL);
	
	device = devices[0];
	
	commandQueue = clCreateCommandQueue(context, device, 0, &errorCode);
	checkError(errorCode);
	
	starBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, starBufferSize, stars, &errorCode);
	checkError(errorCode);
	
	const char* fileName = "kernel.cl";
	const char* programBuffer = readFile(fileName);
	size_t kernelLength = strlen(programBuffer);
	program = clCreateProgramWithSource(context, 1, (const char **)&programBuffer, &kernelLength, &errorCode);
	errorCode = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if (errorCode == CL_BUILD_PROGRAM_FAILURE) {
		// Determine the size of the log
		size_t log_size;
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		
		// Allocate memory for the log
		char *log = (char *) malloc(log_size);
		
		// Get the log
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
		
		// Print the log
		
		std::cerr << log << std::endl;
		
		free(log);

	}
	checkError(errorCode);
	
	kernel = clCreateKernel(program, "calculateGravity", &errorCode);
	checkError(errorCode);
	
	free(devices);
}

void GalaxSee::execute() {
	clEnqueueWriteBuffer(commandQueue, starBuffer, CL_TRUE, 0, starBufferSize, stars, 0, NULL, NULL);
	errorCode |= clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&starBuffer);
	errorCode |= clSetKernelArg(kernel, 1, sizeof(int), (void *)&numStars);
	errorCode |= clSetKernelArg(kernel, 2, sizeof(int), (void *)&numStars);
	checkError(errorCode);

	errorCode = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, globalWorkSize, globalWorkSize, 0, NULL, NULL);
	checkError(errorCode);
}

void GalaxSee::printJSON(int iterations) {
	
	int i, j;
	printf("var nstars=%i,points=[",numStars);
	for(i = 0; i < iterations; i++) {
		errorCode = clEnqueueReadBuffer(commandQueue, starBuffer, CL_TRUE, 0, starBufferSize, stars, 0, NULL, NULL);
		checkError(errorCode);

		printf("[");
		for(j = 0; j < numStars; j++) {
			star st = stars[j];
			printf("{");
			printf("x: %.4f, y: %.4f, z: %.4f", st.position.x, st.position.y, st.position.z);
			printf("},");
		}
		printf("],");
		execute();
	}
	printf("]");

	
	
}

GalaxSee::GalaxSee(int nStars, initialConfigurations config) {
	numStars = nStars;
	
	
	if(config == initialConfigurations::GRID) {
		int width = (int) sqrt(nStars);
		numStars = width * width;
		double spacing = GALAXY_SIZE / ((double) width);
		
		starBufferSize = sizeof(star) * numStars;
		stars = (star *) malloc(starBufferSize);
		
		double i, j;
		int index = 0;
		for(i = 0; i < width; i++) {
			for(j = 0; j < width; j++) {		
				stars[index] = star {
					.mass = static_cast<long>(1e9),
					.position = vec {
						.x = static_cast<float>(i * spacing - (spacing * width * 0.5)),
						.y = static_cast<float>(j * spacing - (spacing * width * 0.5)),
						.z = 0
					},
					.velocity = vec {
						.x = 0,
						.y = 0,
						.z = 0
					},
					.acceleration = vec {
						.x = 0,
						.y = 0,
						.z = 0
					}
					
				};
				
				index++;
			}
		}
	}
	
	
	
	
	globalWorkSize[0] = static_cast<size_t>(numStars);
	localWorkSize[0] = static_cast<size_t>(numStars);
	
	
	initOpenCL();	
}

