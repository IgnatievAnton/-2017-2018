
#include <stdio.h>
extern "C"
{
#include "jpeglib.h"
}
#include <setjmp.h>
#include <fstream>

#define GETJSAMPLE(value)  ((int) (value))
#define CENTERJSAMPLE	128

#define DCTSIZE		    8
#define DCTSIZE2	    64



static const double aanscalefactor[DCTSIZE] =
{
	1.0, 1.3870398453221475, 1.3065629648763766, 1.1758756024193588,
	1.0, 0.7856949583871022, 0.5411961001461971, 0.2758993792829431,
};

static const double angel_coef[DCTSIZE2] = 
{
  1.000000, 0.500000, 0.333333, 0.250000, 0.200000, 0.166667, 0.142857, 0.125000,
  0.500000, 0.250000, 0.166667, 0.125000, 0.100000, 0.083333, 0.071429, 0.062500,
  0.333333, 0.166667, 0.111111, 0.083333, 0.066667, 0.055556, 0.047619, 0.041667,
  0.250000, 0.125000, 0.083333, 0.062500, 0.050000, 0.041667, 0.035714, 0.031250,
  0.200000, 0.100000, 0.066667, 0.050000, 0.040000, 0.033333, 0.028571, 0.025000,
  0.166667, 0.083333, 0.055556, 0.041667, 0.033333, 0.027778, 0.023810, 0.020833,
  0.142857, 0.071429, 0.047619, 0.035714, 0.028571, 0.023810, 0.020408, 0.017857,
  0.125000, 0.062500, 0.041667, 0.031250, 0.025000, 0.020833, 0.017857, 0.015625
};

static const double line_coef[DCTSIZE] = 
{
  0.125000, 0.062500, 0.041667, 0.031250, 0.025000, 0.020833, 0.017857, 0.015625
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

GLOBAL(void)
fdct(const unsigned char * p, int stride, float *d) //float *outptr, const unsigned char *input_buf, int input_stride)
{
	float tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
	float tmp10, tmp11, tmp12, tmp13;
	float z1, z2, z3, z4, z5, z11, z13;
	float *dataptr;
	int ctr;

	dataptr = d;
	for (ctr = 0; ctr < DCTSIZE; ctr++)
	{
		tmp0 = (float)(GETJSAMPLE(p[0]) + GETJSAMPLE(p[7]));
		tmp7 = (float)(GETJSAMPLE(p[0]) - GETJSAMPLE(p[7]));
		tmp1 = (float)(GETJSAMPLE(p[1]) + GETJSAMPLE(p[6]));
		tmp6 = (float)(GETJSAMPLE(p[1]) - GETJSAMPLE(p[6]));
		tmp2 = (float)(GETJSAMPLE(p[2]) + GETJSAMPLE(p[5]));
		tmp5 = (float)(GETJSAMPLE(p[2]) - GETJSAMPLE(p[5]));
		tmp3 = (float)(GETJSAMPLE(p[3]) + GETJSAMPLE(p[4]));
		tmp4 = (float)(GETJSAMPLE(p[3]) - GETJSAMPLE(p[4]));

		tmp10 = tmp0 + tmp3;
		tmp13 = tmp0 - tmp3;
		tmp11 = tmp1 + tmp2;
		tmp12 = tmp1 - tmp2;

		dataptr[0] = tmp10 + tmp11 - 8 * CENTERJSAMPLE;
		dataptr[4] = tmp10 - tmp11;

		z1 = (tmp12 + tmp13) * ((float) 0.707106781); 
		dataptr[2] = tmp13 + z1;
		dataptr[6] = tmp13 - z1;

		tmp10 = tmp4 + tmp5;
		tmp11 = tmp5 + tmp6;
		tmp12 = tmp6 + tmp7;

		z5 = (tmp10 - tmp12) * ((float) 0.382683433);
		z2 = ((float) 0.541196100) * tmp10 + z5; 
		z4 = ((float) 1.306562965) * tmp12 + z5; 
		z3 = tmp11 * ((float) 0.707106781); 

		z11 = tmp7 + z3;		
		z13 = tmp7 - z3;

		dataptr[5] = z13 + z2;	
		dataptr[3] = z13 - z2;
		dataptr[1] = z11 + z4;
		dataptr[7] = z11 - z4;

		dataptr += DCTSIZE;
		p += stride;
	}


	dataptr = d;
	for (ctr = DCTSIZE - 1; ctr >= 0; ctr--) 
	{
		tmp0 = dataptr[DCTSIZE * 0] + dataptr[DCTSIZE * 7];
		tmp7 = dataptr[DCTSIZE * 0] - dataptr[DCTSIZE * 7];
		tmp1 = dataptr[DCTSIZE * 1] + dataptr[DCTSIZE * 6];
		tmp6 = dataptr[DCTSIZE * 1] - dataptr[DCTSIZE * 6];
		tmp2 = dataptr[DCTSIZE * 2] + dataptr[DCTSIZE * 5];
		tmp5 = dataptr[DCTSIZE * 2] - dataptr[DCTSIZE * 5];
		tmp3 = dataptr[DCTSIZE * 3] + dataptr[DCTSIZE * 4];
		tmp4 = dataptr[DCTSIZE * 3] - dataptr[DCTSIZE * 4];


		tmp10 = tmp0 + tmp3;
		tmp13 = tmp0 - tmp3;
		tmp11 = tmp1 + tmp2;
		tmp12 = tmp1 - tmp2;

		dataptr[DCTSIZE * 0] = tmp10 + tmp11;
		dataptr[DCTSIZE * 4] = tmp10 - tmp11;

		z1 = (tmp12 + tmp13) *  0.707106781; 
		dataptr[DCTSIZE * 2] = tmp13 + z1; 
		dataptr[DCTSIZE * 6] = tmp13 - z1;



		tmp10 = tmp4 + tmp5;	
		tmp11 = tmp5 + tmp6;
		tmp12 = tmp6 + tmp7;

	
		z5 = (tmp10 - tmp12) * 0.382683433; 
		z2 = 0.541196100 * tmp10 + z5; 
		z4 = 1.306562965 * tmp12 + z5; 
		z3 = tmp11 * 0.707106781;

		z11 = tmp7 + z3;		
		z13 = tmp7 - z3;

		dataptr[DCTSIZE * 5] = z13 + z2;
		dataptr[DCTSIZE * 3] = z13 - z2;
		dataptr[DCTSIZE * 1] = z11 + z4;
		dataptr[DCTSIZE * 7] = z11 - z4;

		dataptr++;	
	}
}

GLOBAL(void)
idct(const float *d, unsigned char *res, int stride)
{
	float tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
	float tmp10, tmp11, tmp12, tmp13;
	float z5, z10, z11, z12, z13;
	float * wsptr;
	int ctr;
	float workspace[DCTSIZE2];

	wsptr = workspace;
	for (ctr = DCTSIZE; ctr > 0; ctr--) {
		if (d[DCTSIZE * 1] == 0 && d[DCTSIZE * 2] == 0 &&
			d[DCTSIZE * 3] == 0 && d[DCTSIZE * 4] == 0 &&
			d[DCTSIZE * 5] == 0 && d[DCTSIZE * 6] == 0 &&
			d[DCTSIZE * 7] == 0)
		{
			float dcval = d[DCTSIZE * 0];

			wsptr[DCTSIZE * 0] = dcval;
			wsptr[DCTSIZE * 1] = dcval;
			wsptr[DCTSIZE * 2] = dcval;
			wsptr[DCTSIZE * 3] = dcval;
			wsptr[DCTSIZE * 4] = dcval;
			wsptr[DCTSIZE * 5] = dcval;
			wsptr[DCTSIZE * 6] = dcval;
			wsptr[DCTSIZE * 7] = dcval;

			d++;
			wsptr++;
			continue;
		}

		tmp0 = d[DCTSIZE * 0];
		tmp1 = d[DCTSIZE * 2];
		tmp2 = d[DCTSIZE * 4];
		tmp3 = d[DCTSIZE * 6];

		tmp10 = tmp0 + tmp2;
		tmp11 = tmp0 - tmp2;

		tmp13 = tmp1 + tmp3;
		tmp12 = (tmp1 - tmp3) *  1.414213562f - tmp13;

		tmp0 = tmp10 + tmp13;
		tmp3 = tmp10 - tmp13;
		tmp1 = tmp11 + tmp12;
		tmp2 = tmp11 - tmp12;

		tmp4 = d[DCTSIZE * 1];
		tmp5 = d[DCTSIZE * 3]; 
		tmp6 = d[DCTSIZE * 5];
		tmp7 = d[DCTSIZE * 7]; 

		z13 = tmp6 + tmp5;	
		z10 = tmp6 - tmp5;
		z11 = tmp4 + tmp7;
		z12 = tmp4 - tmp7;

		tmp7 = z11 + z13;	
		tmp11 = (z11 - z13) * 1.414213562f; 

		z5 = (z10 + z12) * 1.847759065f; 
		tmp10 = 1.082392200f * z12 - z5; 
		tmp12 = 2.613125930f * z10 + z5; 

		tmp6 = tmp12 - tmp7;
		tmp5 = tmp11 - tmp6;
		tmp4 = tmp10 + tmp5;

		wsptr[DCTSIZE * 0] = tmp0 + tmp7;
		wsptr[DCTSIZE * 7] = tmp0 - tmp7;
		wsptr[DCTSIZE * 1] = tmp1 + tmp6;
		wsptr[DCTSIZE * 6] = tmp1 - tmp6;
		wsptr[DCTSIZE * 2] = tmp2 + tmp5;
		wsptr[DCTSIZE * 5] = tmp2 - tmp5;
		wsptr[DCTSIZE * 4] = tmp3 + tmp4;
		wsptr[DCTSIZE * 3] = tmp3 - tmp4;

		d++;
		wsptr++;
	}

	wsptr = workspace;
	for (ctr = 0; ctr < DCTSIZE; ctr++) 
	{
		z5 = wsptr[0] + (CENTERJSAMPLE + 0.5f);
		tmp10 = z5 + wsptr[4];
		tmp11 = z5 - wsptr[4];

		tmp13 = wsptr[2] + wsptr[6];
		tmp12 = (wsptr[2] - wsptr[6]) * 1.414213562f - tmp13;

		tmp0 = tmp10 + tmp13;
		tmp3 = tmp10 - tmp13;
		tmp1 = tmp11 + tmp12;
		tmp2 = tmp11 - tmp12;


		z13 = wsptr[5] + wsptr[3];
		z10 = wsptr[5] - wsptr[3];
		z11 = wsptr[1] + wsptr[7];
		z12 = wsptr[1] - wsptr[7];

		tmp7 = z11 + z13;
		tmp11 = (z11 - z13) * 1.414213562f;

		z5 = (z10 + z12) * 1.847759065f; 
		tmp10 = z5 - 1.082392200f * z12; 
		tmp12 = z5 - 2.613125930f * z10; 

		tmp6 = tmp12 - tmp7;
		tmp5 = tmp11 - tmp6;
		tmp4 = tmp10 + tmp5;


		res[0] = range_limit((int)(tmp0 + tmp7));
		res[7] = range_limit( (int)(tmp0 - tmp7));
		res[1] = range_limit((int)(tmp1 + tmp6));
		res[6] = range_limit((int)(tmp1 - tmp6));
		res[2] = range_limit((int)(tmp2 + tmp5));
		res[5] = range_limit((int)(tmp2 - tmp5));
		res[4] = range_limit((int)(tmp3 + tmp4));
		res[3] = range_limit((int)(tmp3 - tmp4));

		wsptr += DCTSIZE;
		res += stride;
	}
}

GLOBAL(void)
quant(float *d, float *ftable, float *itable)
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

GLOBAL(unsigned char*)
read_JPEG_file(char * filename, int &width, int &height)
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
		printf("���� ���������� ������� ��� �������\n");
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

	(void)jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	fclose(infile);

	return data;
}

void write_file(const int width, const int height, unsigned char *data)
{
  FILE *out_file;
  if ((out_file = fopen("res.pgm", "wb")) == NULL)
    printf("���� ���������� ������� ��� �������\n");
  else {
    fprintf(out_file, "P5\n%i %i\n255\n", width, height);
    fwrite(data, 1, width*height, out_file);
  }

}

void sum(float *result, unsigned char *summ, unsigned char *input, int stride)
{
  for (int x = 0; x < DCTSIZE; x++)
  {
    for (int y = 0; y < DCTSIZE; y++)
    {
      result[x + y*stride] += (float)(input[x + y*DCTSIZE]);
      summ[x + y*stride]++;
    }
  }
}

void divide(unsigned char *output, float *res, int width, int height)
{
  int rows1 = 0;
  int rows3 = 7;
  int cols4 = 7;
  int rows5 = 7;
  int cols6 = 7;
  int rows7 = 63;


  for (int i = 0; i < width-1; i++)
  {
    if (i >= 0 && i < DCTSIZE)  // ��������� ������ ������ �������
    {
      for (int j = 0; j < height-1; j++)
      {
        if (rows3 == -1)
        {
          rows3 = 7;
        }
        if (j >= 0 && j < DCTSIZE) // ����� ������� ���� 8�8
        {
          output[i + j*width] = res[i + j*width] * angel_coef[i + j*DCTSIZE];//range_limit((int)(res[i + j*width] * angel_coef[i + j*DCTSIZE]));
          rows1++;
        }
        if (j >= DCTSIZE && j < height - 9) // �������������� �������������
        {
          output[i + j*width] = res[i + j*width] * line_coef[i];// range_limit((int)(res[i + j*width] * line_coef[i]));
        }

        if (j >= height - 9 && j < height - 1) // ������ ������� ����
        {
          output[i + j*width] = res[i + j*width] * angel_coef[i + rows3*DCTSIZE];//range_limit((int)(res[i + j*width] * angel_coef[i + rows3*DCTSIZE]));
          rows3--;
        }
      }
    }

    if (i >= DCTSIZE && i < width - 9) // ��������� ����������� ������ ��������
    {
      for (int j = 0; j < height - 1; j++)
      {
        if (cols4 == -1)
        {
          cols4 = 7;
        }
        if (j >= 0 && j < DCTSIZE)
        { 
          output[i + j*width] = range_limit((int)(res[i + j*width] * line_coef[j])); //������������ ����� �������
        }
        if (j >= height - 9 && j < height - 1) // ������������ ������ �������
        {
          output[i + j*width] = range_limit((int)(res[i + j*width] * line_coef[cols4]));
          cols4--;
        }
        else // ��������
        {
          output[i + j*width] = range_limit((int)(res[i + j*width] * 0.015625));
        }
      }
    }

    if (i >= width - 9 && i < width - 1)  // ��������� ��������� 8�� �������
    {
      for (int j = 0; j < height - 1; j++)
      {
        if (j >= 0 && j < DCTSIZE) // ����� ������ ����
        {
          output[i + j*width] = range_limit((int)(res[i + j*width] * angel_coef[rows5 + j * DCTSIZE]));
        }
        if (j >= DCTSIZE && j < height - 9) // ������ �������������� �������
        {
          output[i + j*width] = range_limit((int)(res[i + j*width] * line_coef[cols6]));
        }
        if (j >= height - 9 && j < height - 1) // ������ ������ ����
        {
          output[i + j*width] = range_limit((int)(res[i + j*width] * angel_coef[rows7]));
          rows7--;
        }
      }  
      cols6--;
      rows5--;
    }
  }
}

int main()
{
	unsigned char *data;
	int width;
	int height;
	data = read_JPEG_file("testimg.jpg", width, height);

	unsigned short quant_table[DCTSIZE2];
	for (int i = 0; i < DCTSIZE2; i++)
	{
		quant_table[i] = 1;
	}

	float ftable[DCTSIZE2], itable[DCTSIZE2];
	jpeg_init_fdct_table(quant_table, ftable);
	jpeg_init_idct_table(quant_table, itable);
	float d[DCTSIZE2];
  float *result = new float[width * height];
	memset(result, 0.0, width * height);
  unsigned char *summ = new unsigned char[width * height];
  memset(summ, 0, width * height);
  unsigned char *output = new unsigned char[width * height];
  memset(output, 0, width * height);


	for (int i = 0; i < width - DCTSIZE; i++)
	{
		for (int j = 0; j < height - DCTSIZE; j++)
		{
			fdct((data + i + j*width), width, d);			
			quant(d, ftable, itable);
			idct(d, output, DCTSIZE);
      if (i == 0 && j== height - 9)
      {
        for (int i = 0; i < DCTSIZE; i++)
        {
          for (int j = 0; j < DCTSIZE; j++)
          {
            printf("%f ", d[i + j*DCTSIZE]);
          }
          printf("\n");
        }
      }
			sum((result + i + j*width), (summ+i+j*width), output, width);
		}
	}

  printf("result\n");
  for (int i = 0; i < 8; i++)
  {
    for (int j = height-9; j < height-1; j++)
    {
      printf("%f ", result[i + j*width]);
    }
    printf("\n");
  }

  printf("summ\n");
  for (int i = 0; i < 8; i++)
  {
    for (int j = height - 9; j < height - 1; j++)
    {
      printf("%i ", summ[i + j*width]);
    }
    printf("\n");
  }


	divide(output, result, width, height);

  printf("res after divide\n");
  for (int i = 0; i < 8; i++)
  {
    for (int j = height - 9; j < height - 1; j++)
    {
			printf("%d ", output[i + j*width]);
		}
		printf("\n");
	}

	printf("data\n");
  for (int i = 0; i < 8; i++)
  {
    for (int j = height - 9; j < height - 1; j++)
    {
			printf("%d ", data[i + j*width]);
		}
		printf("\n");
	}


  write_file(width, height, output);

  delete data;
  delete result;
  delete output;

	return 0;
}