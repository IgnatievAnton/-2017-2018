
#include <stdio.h>
extern "C"
{
#include "jpeglib.h"
}
#include <setjmp.h>
#include <fstream>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <CL/cl.h>
#include <stdbool.h>




#define DCTSIZE		    8
#define DCTSIZE2	    64


static const double aanscalefactor[DCTSIZE] =
{
	1.0, 1.3870398453221475, 1.3065629648763766, 1.1758756024193588,
	1.0, 0.7856949583871022, 0.5411961001461971, 0.2758993792829431,
};

void jpeg_init_fdct_table(const unsigned short *quantptr, float quant_table[DCTSIZE2])
{
	for (int row = 0, i = 0; row < DCTSIZE; row++)
		for (int col = 0; col < DCTSIZE; i++, col++)
			quant_table[i] = 0.125 / (aanscalefactor[row] * aanscalefactor[col] * quantptr[i]); // 1/64
}

void jpeg_init_idct_table(const unsigned short *quantptr, float quant_table[DCTSIZE2])
{
	for (int row = 0, i = 0; row < DCTSIZE; row++)
		for (int col = 0; col < DCTSIZE; i++, col++)
			quant_table[i] = quantptr[i] * aanscalefactor[row] * aanscalefactor[col] * 0.125;
}

unsigned char range_limit(int x)
{
	return x < 0 ? 0 : x > 0xFF ? 0xFF : x;
}

void quant(float *d, float *ftable, float *itable)
{
	for (int i = 0; i < DCTSIZE2; i++)
	{
		d[i] = round(d[i] * ftable[i]) * itable[i];
	}
}

struct my_error_mgr {
	struct jpeg_error_mgr pub;

	jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr * my_error_ptr;

METHODDEF(void)
my_error_exit(j_common_ptr cinfo)
{
	my_error_ptr myerr = (my_error_ptr)cinfo->err;

	(*cinfo->err->output_message) (cinfo);

	longjmp(myerr->setjmp_buffer, 1);
}

unsigned char* read_JPEG_file(char * filename, int &width, int &height, unsigned short *quant_table)
{
	struct jpeg_decompress_struct cinfo;

	struct my_error_mgr jerr;
	FILE * infile;
	JSAMPARRAY buffer;
	int row_stride;
	FILE *out_file;

	if ((infile = fopen(filename, "rb")) == NULL) {
		fprintf(stderr, "can't open %s\n", filename);
		return 0;
	}

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if (setjmp(jerr.setjmp_buffer)) {
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);
		return 0;
	}

	jpeg_create_decompress(&cinfo);


	jpeg_stdio_src(&cinfo, infile);


	(void)jpeg_read_header(&cinfo, TRUE);
	cinfo.out_color_space = JCS_GRAYSCALE;

	(void)jpeg_start_decompress(&cinfo);
	row_stride = cinfo.output_width * cinfo.output_components;

	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

	unsigned char *data = new unsigned char[cinfo.output_width*cinfo.output_height];
	int row_count = 0;
	if ((out_file = fopen("output.pgm", "wb")) == NULL)
		printf("Сайл невозможно открыть или создать\n");
	else {
		fprintf(out_file, "P5\n%i %i\n255\n", cinfo.output_width, cinfo.output_height);
		while (cinfo.output_scanline < cinfo.output_height) {
			(void)jpeg_read_scanlines(&cinfo, buffer, 1);
			fwrite(buffer[0], 1, cinfo.output_width, out_file);
			for (int i = 0; i < cinfo.output_width; i++)
			{
				data[row_count*cinfo.output_width + i] = buffer[0][i];
			}
			row_count++;
		}
	}
	fclose(out_file);
	width = cinfo.output_width;
	height = cinfo.output_height;

	JQUANT_TBL **table = cinfo.quant_tbl_ptrs;
	for (int i = 0; i < DCTSIZE2; i++)
	{
		quant_table[i] = table[0]->quantval[i];
	}

	(void)jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	fclose(infile);

	return data;
}

void write_file(const int width, const int height, unsigned char *data)
{
	FILE *out_file;
	if ((out_file = fopen("res.pgm", "wb")) == NULL)
		printf("Сайл невозможно открыть или создать\n");
	else {
		fprintf(out_file, "P5\n%i %i\n255\n", width, height);
		fwrite(data, 1, width*height, out_file);
	}

}

int main()
{
	unsigned char *data;
	int width;
	int height;
	unsigned short quant_table[DCTSIZE2];
	data = read_JPEG_file("testimg.jpg", width, height, quant_table);
	float ftable[DCTSIZE2], itable[DCTSIZE2];
	jpeg_init_fdct_table(quant_table, ftable);
	jpeg_init_idct_table(quant_table, itable);
	//short unsigned *result = new short unsigned[width * height];
	//memset(result, 0, width * height*sizeof(short));
	unsigned char *summ = new unsigned char[width * height];
	memset(summ, 0, width * height);
	unsigned char output[DCTSIZE2];


	cl_uint num_platforms = 0;
	clGetPlatformIDs(NULL, NULL, &num_platforms);
	cl_platform_id *platforms = new cl_platform_id[num_platforms];
	clGetPlatformIDs(num_platforms, platforms, NULL);
	cl_device_type device_type = CL_DEVICE_TYPE_GPU;
	cl_device_id *device = NULL;
	for (int i = 0; i < num_platforms; i++) {
		cl_uint num_devices = 0;
		clGetDeviceIDs(platforms[i], device_type, 0, NULL, &num_devices);
		if (num_devices >0) {
			device = new cl_device_id[num_devices];
			clGetDeviceIDs(platforms[i], device_type, num_devices, device, NULL);
			break;
		}
	}
	if (device == NULL) {
		cl_device_type device_type = CL_DEVICE_TYPE_CPU;
		for (int i = 0; i < num_platforms; i++) {
			cl_uint num_devices = 0;
			clGetDeviceIDs(platforms[i], device_type, 0, NULL, &num_devices);
			if (num_devices >0) {
				device = new cl_device_id[num_devices];
				clGetDeviceIDs(platforms[i], device_type, num_devices, device, NULL);
				break;
			}
		}
	}

	cl_uint buff;
	clGetDeviceInfo(device[0], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &buff, NULL);
	clGetDeviceInfo(device[0], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(cl_uint), &buff, NULL);
	cl_uint buff2[3];
	clGetDeviceInfo(device[0], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(buff2), buff2, NULL);

	cl_context context;
	cl_int errContext;
	context = clCreateContext(NULL, 1, &device[0], NULL, NULL, &errContext);

	cl_command_queue com_queue;
	cl_int errcodeQueue;
	com_queue = clCreateCommandQueue(context, device[0], CL_QUEUE_PROFILING_ENABLE, &errcodeQueue);

	FILE *pFile;
	size_t  sFile;
	char *buffer;
	size_t result;
	pFile = fopen("ker.cl", "rb");

	const char *string = const_cast<char*>(buffer);

	cl_program program = clCreateProgramWithSource(context, 1, &string, &sFile, NULL);
	cl_int errProgramm;
	errProgramm = clBuildProgram(program, 1, &device[0], " ", NULL, NULL);

	char *buildLog = NULL;
	clGetProgramBuildInfo(program, device[0], CL_PROGRAM_BUILD_LOG, 10240, buildLog, NULL);

	const char *nameOfKernel = "ker";
	int kernel_err;
	cl_kernel kernel = clCreateKernel(program, nameOfKernel, &kernel_err);

	int size1 = width*height;
	int size2 = DCTSIZE2;

	cl_mem sourceJPG = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(unsigned char)*size1, NULL, NULL);
	cl_mem resultJPG = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(unsigned char)*size1, NULL, NULL);
	cl_mem bufFtable = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float)*size2, NULL, NULL);
    cl_mem bufItable = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float)*size2, NULL, NULL);


	int err_setParam;
	err_setParam = clSetKernelArg(kernel, 0, sizeof(sourceJPG), &sourceJPG);
	err_setParam = clSetKernelArg(kernel, 1, sizeof(resultJPG), &resultJPG);
	err_setParam = clSetKernelArg(kernel, 2, sizeof(bufFtable), &bufFtable);
	err_setParam = clSetKernelArg(kernel, 3, sizeof(bufItable), &bufItable);
	err_setParam = clSetKernelArg(kernel, 4, sizeof(width), &width);
	err_setParam = clSetKernelArg(kernel, 5, sizeof(height), &height);

	int errWr;
	int errWr1;
	errWr = clEnqueueWriteBuffer(com_queue, sourceJPG, CL_FALSE, 0, sizeof(unsigned char)*size1, data, NULL, NULL, NULL);
	errWr = clEnqueueWriteBuffer(com_queue, bufFtable, CL_FALSE, 0, sizeof(float)*size2, ftable, NULL, NULL, NULL);
	errWr = clEnqueueWriteBuffer(com_queue, bufItable, CL_FALSE, 0, sizeof(float)*size2, itable, NULL, NULL, NULL);

 	const size_t global_work_size[] = {(width/ (DCTSIZE *2) )* DCTSIZE,(height / (DCTSIZE*2))*DCTSIZE};
	const size_t local_work_size[] = {DCTSIZE, DCTSIZE};
	cl_event event = 0;

	errWr = clEnqueueNDRangeKernel(com_queue, kernel, 2, 0, global_work_size, local_work_size, NULL, NULL, &event);
	errWr1 = clEnqueueReadBuffer(com_queue, resultJPG, CL_TRUE, 0, sizeof(unsigned char)*size1, summ, NULL, NULL, NULL);
	//errWr = clEnqueueWriteBuffer(com_queue, resultJPG, CL_FALSE, 0, sizeof(int)*size1, b, NULL, NULL, NULL);

	//d_C = clCreateBuffer(context, CL_MEM_READ_WRITE, mem_size_C, NULL, &err);
	//d_A = clCreateBuffer(context, CL_MEM_READ_WRITE, mem_size_A, NULL, &err);
	//d_B = clCreateBuffer(context, CL_MEM_READ_WRITE, mem_size_B, NULL, &err);

	


	//for (int i = 0; i <= width - DCTSIZE; i++)
	//{
	//	for (int j = 0; j <= height - DCTSIZE; j++)
	//	{
	//		float d[DCTSIZE2];
	//		fdct(&data[i + j*width], width, d);
	//		quant(d, ftable, itable);
	//		idct(d, output, DCTSIZE);
	//		sum(&result[i + j*width], &summ[i + j*width], output, width);
	//	}
	//}
	//divide(result, width, height, summ);

	write_file(width, height, summ);

	delete data;

	return 0;
}
