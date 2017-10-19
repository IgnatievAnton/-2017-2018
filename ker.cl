#define GETJSAMPLE(value)  ((int) (value))
#define CENTERJSAMPLE	128
#define DCTSIZE		    8
#define DCTSIZE2	    64


void divide(short unsigned *src, int width, int height, unsigned char *dst)
{
	/* if(x>=a && x<b) ==
	if((unsigned int) (x-a) < (b-a))
	*/

	for (int j = 0; j < height; j++)
	{
		int ky = j < DCTSIZE ? j + 1 : j > height - DCTSIZE ? height - j : DCTSIZE;
		for (int i = 0; i < width; i++)
		{
			int kx = i < DCTSIZE ? i + 1 : i > width - DCTSIZE ? width - i : DCTSIZE;
			dst[i + j*width] = src[i + j*width] / (kx*ky);
		}
	}
}


void sum(short unsigned *result, unsigned char *summ, unsigned char *input, int stride)
{
	for (int x = 0; x < DCTSIZE; x++)
	{
		for (int y = 0; y < DCTSIZE; y++)
		{
			result[x + y*stride] += input[x + y*DCTSIZE];
			summ[x + y*stride]++;
		}
	}
}

void fdct(const unsigned char *input_buf, int input_stride, float *outptr) //const unsigned char * p, int stride, float *d)
{
	float tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
	float tmp10, tmp11, tmp12, tmp13;
	float z1, z2, z3, z4, z5, z11, z13;
	float *dataptr;
	int ctr;

	/* Pass 1: process rows. */

	dataptr = outptr;
	for (ctr = 0; ctr < DCTSIZE; ctr++) {
		tmp0 = (float)(GETJSAMPLE(input_buf[0]) + GETJSAMPLE(input_buf[7]));
		tmp7 = (float)(GETJSAMPLE(input_buf[0]) - GETJSAMPLE(input_buf[7]));
		tmp1 = (float)(GETJSAMPLE(input_buf[1]) + GETJSAMPLE(input_buf[6]));
		tmp6 = (float)(GETJSAMPLE(input_buf[1]) - GETJSAMPLE(input_buf[6]));
		tmp2 = (float)(GETJSAMPLE(input_buf[2]) + GETJSAMPLE(input_buf[5]));
		tmp5 = (float)(GETJSAMPLE(input_buf[2]) - GETJSAMPLE(input_buf[5]));
		tmp3 = (float)(GETJSAMPLE(input_buf[3]) + GETJSAMPLE(input_buf[4]));
		tmp4 = (float)(GETJSAMPLE(input_buf[3]) - GETJSAMPLE(input_buf[4]));

		tmp10 = tmp0 + tmp3;
		tmp13 = tmp0 - tmp3;
		tmp11 = tmp1 + tmp2;
		tmp12 = tmp1 - tmp2;

		/* Apply unsigned->signed conversion. */
		dataptr[0] = tmp10 + tmp11 - 8 * CENTERJSAMPLE;
		dataptr[4] = tmp10 - tmp11;

		z1 = (tmp12 + tmp13) * 0.707106781f;
		dataptr[2] = tmp13 + z1;
		dataptr[6] = tmp13 - z1;

		tmp10 = tmp4 + tmp5;
		tmp11 = tmp5 + tmp6;
		tmp12 = tmp6 + tmp7;

		z5 = (tmp10 - tmp12) * 0.382683433f;
		z2 = 0.541196100f * tmp10 + z5;
		z4 = 1.306562965f * tmp12 + z5;
		z3 = tmp11 * 0.707106781f;

		z11 = tmp7 + z3;
		z13 = tmp7 - z3;

		dataptr[5] = z13 + z2;
		dataptr[3] = z13 - z2;
		dataptr[1] = z11 + z4;
		dataptr[7] = z11 - z4;

		dataptr += DCTSIZE;
		input_buf += input_stride;
	}

	/* Pass 2: process columns. */

	dataptr = outptr;
	for (ctr = DCTSIZE - 1; ctr >= 0; ctr--) {
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

		z1 = (tmp12 + tmp13) * 0.707106781f;
		dataptr[DCTSIZE * 2] = tmp13 + z1;
		dataptr[DCTSIZE * 6] = tmp13 - z1;

		tmp10 = tmp4 + tmp5;
		tmp11 = tmp5 + tmp6;
		tmp12 = tmp6 + tmp7;

		z5 = (tmp10 - tmp12) * 0.382683433f;
		z2 = 0.541196100f * tmp10 + z5;
		z4 = 1.306562965f * tmp12 + z5;
		z3 = tmp11 * 0.707106781f;

		z11 = tmp7 + z3;
		z13 = tmp7 - z3;

		dataptr[DCTSIZE * 5] = z13 + z2;
		dataptr[DCTSIZE * 3] = z13 - z2;
		dataptr[DCTSIZE * 1] = z11 + z4;
		dataptr[DCTSIZE * 7] = z11 - z4;

		dataptr++;
	}
}

idct(const float *inptr, unsigned char *output_buf, int output_stride) //const float *d, unsigned char *res, int stride)
{

	float tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
	float tmp10, tmp11, tmp12, tmp13;
	float z5, z10, z11, z12, z13;
	float * wsptr;
	int ctr;
	float workspace[DCTSIZE2];

	wsptr = workspace;
	for (ctr = DCTSIZE; ctr > 0; ctr--) {
		if (inptr[DCTSIZE * 1] == 0 && inptr[DCTSIZE * 2] == 0 &&
			inptr[DCTSIZE * 3] == 0 && inptr[DCTSIZE * 4] == 0 &&
			inptr[DCTSIZE * 5] == 0 && inptr[DCTSIZE * 6] == 0 &&
			inptr[DCTSIZE * 7] == 0)
		{
			float dcval = inptr[DCTSIZE * 0];

			wsptr[DCTSIZE * 0] = dcval;
			wsptr[DCTSIZE * 1] = dcval;
			wsptr[DCTSIZE * 2] = dcval;
			wsptr[DCTSIZE * 3] = dcval;
			wsptr[DCTSIZE * 4] = dcval;
			wsptr[DCTSIZE * 5] = dcval;
			wsptr[DCTSIZE * 6] = dcval;
			wsptr[DCTSIZE * 7] = dcval;

			inptr++;
			wsptr++;
			continue;
		}

		tmp0 = inptr[DCTSIZE * 0];
		tmp1 = inptr[DCTSIZE * 2];
		tmp2 = inptr[DCTSIZE * 4];
		tmp3 = inptr[DCTSIZE * 6];

		tmp10 = tmp0 + tmp2;
		tmp11 = tmp0 - tmp2;

		tmp13 = tmp1 + tmp3;
		tmp12 = (tmp1 - tmp3) * 1.414213562f - tmp13;

		tmp0 = tmp10 + tmp13;
		tmp3 = tmp10 - tmp13;
		tmp1 = tmp11 + tmp12;
		tmp2 = tmp11 - tmp12;

		tmp4 = inptr[DCTSIZE * 1];
		tmp5 = inptr[DCTSIZE * 3];
		tmp6 = inptr[DCTSIZE * 5];
		tmp7 = inptr[DCTSIZE * 7];

		z13 = tmp6 + tmp5;
		z10 = tmp6 - tmp5;
		z11 = tmp4 + tmp7;
		z12 = tmp4 - tmp7;

		tmp7 = z11 + z13;
		tmp11 = (z11 - z13) * 1.414213562f;

		z5 = (z10 + z12) * 1.847759065f;
		tmp10 = z5 - z12 * 1.082392200f;
		tmp12 = z5 - z10 * 2.613125930f;

		tmp6 = tmp12 - tmp7;
		tmp5 = tmp11 - tmp6;
		tmp4 = tmp10 - tmp5;

		wsptr[DCTSIZE * 0] = tmp0 + tmp7;
		wsptr[DCTSIZE * 7] = tmp0 - tmp7;
		wsptr[DCTSIZE * 1] = tmp1 + tmp6;
		wsptr[DCTSIZE * 6] = tmp1 - tmp6;
		wsptr[DCTSIZE * 2] = tmp2 + tmp5;
		wsptr[DCTSIZE * 5] = tmp2 - tmp5;
		wsptr[DCTSIZE * 3] = tmp3 + tmp4;
		wsptr[DCTSIZE * 4] = tmp3 - tmp4;

		inptr++;
		wsptr++;
	}

	/* Pass 2: process rows. */

	wsptr = workspace;
	for (ctr = 0; ctr < DCTSIZE; ctr++) {
		/* Prepare range-limit and float->int conversion */
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
		tmp10 = z5 - z12 * 1.082392200f;
		tmp12 = z5 - z10 * 2.613125930f;

		tmp6 = tmp12 - tmp7;
		tmp5 = tmp11 - tmp6;
		tmp4 = tmp10 - tmp5;

		/* Final output stage: float->int conversion and range-limit */
		output_buf[0] = range_limit((int)(tmp0 + tmp7));
		output_buf[7] = range_limit((int)(tmp0 - tmp7));
		output_buf[1] = range_limit((int)(tmp1 + tmp6));
		output_buf[6] = range_limit((int)(tmp1 - tmp6));
		output_buf[2] = range_limit((int)(tmp2 + tmp5));
		output_buf[5] = range_limit((int)(tmp2 - tmp5));
		output_buf[3] = range_limit((int)(tmp3 + tmp4));
		output_buf[4] = range_limit((int)(tmp3 - tmp4));

		wsptr += DCTSIZE;
		output_buf += output_stride;
	}
}

__kernel void tipomain(const uchar __global*data,const float __global *ftable,const float __global *itable, const float __global *ftable,const uchar __global*summ)
{
	uint gr_i = get_group_id(0);
	uint gr_j = get_group_id(1);
	uint loc_i = get_local_id(0); 
	uint loc_j = get_local_id(1);
	uint gl_i = get_global_id(0);
	uint gl_j = get_global_id(1);

	__local uchar loc_source_image [DCTSIZE*2][DCTSIZE*2];
	for(int i=0;i<DCTSIZE*2;i++)
	{
		for (int j=0;j<DCTSIZE*2;j++)
		{
			loc_source_image[i][j] = data[gr_i]
		}
	}
	__local uchar loc_out_image [DCTSIZE*2][DCTSIZE*2]
	short *result = (short*)malloc(width * height);
	memset(result, 0, width * height*sizeof(short));
	for (int i = 0; i <= width - DCTSIZE; i++)
		{
			for (int j = 0; j <= height - DCTSIZE; j++)
			{
				float d[DCTSIZE2];
				fdct(&data[i + j*width], width, d);
				quant(d, ftable, itable);
				idct(d, output, DCTSIZE);
				sum(&result[i + j*width], &summ[i + j*width], output, width);
			}
			divide(result, width, height, summ);
		}
	delete result;
}
