#include "raw_edges.h"

static struct stage12 s12;

raw_edges::raw_edges(Mat ImageR)
{
	Mat image = ImageR;
}

static void buffer_s12_init() {
	s12.mean0 = 0;
	s12.mean1 = 0;
	s12.mean0 = new double[720];
	s12.mean1 = new double[540];


	s12.img = new int* [1080];;// new int[1080][1440];
	for (int h = 0; h < 1080; h++)
	{
		s12.img[h] = new int[1440];
	}

	s12.img2 = new int* [540];;// new int[1080][1440];
	for (int h = 0; h < 540; h++)
	{
		s12.img2[h] = new int[720];
	}
}

static void buffers_init(void) {
	buffer_s12_init();
}

Mat raw_edges::ImageSliceR(Mat image, int n)
{
	Mat imgCopy = Mat(image.rows / n, image.cols / n, CV_8U, (int)image.step / n);

	uint8_t* myData = image.data;
	uint8_t* myCopyData = imgCopy.data;
	int width = image.cols;
	int height = image.rows;
	int _stride = image.step / n;//in case cols != strides
	for (int i = 0; i < height; i += n)
	{
		for (int j = 0; j < width; j += n)
		{
			int val = myData[i * image.step + j];
			int idx = (i / n * _stride + j / n);
			*(myCopyData + idx) = val;
		}
	}
	return imgCopy;
}

int** raw_edges::Image2ArrayR(Mat imageR)
{
	uint8_t* myData = imageR.data;
	int** array2D = 0;
	array2D = new int* [imageR.rows];
	for (int h = 0; h < imageR.rows; h++)
	{
		array2D[h] = new int[imageR.cols];
		for (int w = 0; w < imageR.cols; w++)
		{
			array2D[h][w] = myData[h * imageR.step + w];
		}
	}
	return array2D;
}

double* raw_edges::Mean0R(int rows, int cols, int** array)
{
	double* Mean0Arr = 0;
	Mean0Arr = new double[cols]();

	double avg = 0.0;
	double sum = 0.0;
	int x = 0;
	for (int w = 0; w < cols; w++)
	{
		sum = 0;
		for (int h = 0; h < rows; h++)
		{
			x = *(*(array + h) + w);
			sum += (double)x;
		}
		avg = sum / (double)rows;
		Mean0Arr[w] = avg;
	}
	return Mean0Arr;
}

double* raw_edges::Mean1R(int rows, int cols, int** array)
{
	double* Mean1Arr = 0;
	Mean1Arr = new double[rows]();

	double avg = 0.0;
	double sum = 0.0;
	int x = 0;
	for (int h = 0; h < rows; h++)
	{
		sum = 0;
		for (int w = 0; w < cols; w++)
		{
			x = *(*(array + h) + w);
			sum += (double)x;
		}
		avg = sum / (double)cols;
		Mean1Arr[h] = avg;
	}
	return Mean1Arr;
}

double raw_edges::MeanR(int rows, double* mean0)
{
	double sum = 0.0;
	double meanR = 0.0;
	for (int i = 0; i < rows; i++) 
	{
		sum += mean0[i];
	}

	meanR = sum / rows;
	return meanR;
}

double raw_edges::Median(int size, double* array)
{
	for (int i = 0; i < size; i++) 
	{
		for (int j = i; j < (size-1); j++) 
		{
			if (array[i] > array[j + 1])
			{
				double temp = array[i];
				array[i] = array[j + 1];
				array[j + 1] = temp;
			}
		}
	}

	if (size % 2 != 0)
		return (double)array[size / 2];
	else
		return (double)(array[(size - 1) / 2] + array[size / 2]) / 2.0;

}

double* raw_edges::BlackmanWindowR(int n)
{
	const double a0 = 0.42;
	const double a1 = 0.5;
	const double a2 = 0.08;
	int wLen = n - 1;
	double* wFun = 0;
	wFun = new double[n]();

	for (int i = 0; i < n; ++i)
	{
		float wi = 0.0;
		wi = a0 - (a1 * cos((2 * M_PI * i) / wLen)) + (a2 * cos((4 * M_PI * i) / wLen));
		wFun[i] = wi;
	}
	return wFun;
}

double* raw_edges::FFTR(double* image_windowR, int size)
{
	const int N = 256;
	fftw_complex*  y = 0;
	y = new fftw_complex[N];
	double in[N];
	fftw_plan p;

	for (int i = 0; i < N; i++) {
		if (i < size) {
			in[i] = image_windowR[i];
		}
		else {
			in[i] = 0;
		}
	}
	p = fftw_plan_dft_r2c_1d(N, in, y, FFTW_ESTIMATE);//fftw_plan_dft_1d(N, in, y, FFTW_FORWARD, FFTW_ESTIMATE);

	fftw_execute(p);
	std::complex<double>* yy;
	yy = reinterpret_cast<std::complex<double> *>(y);
	double* y1 = 0;
	y1 = new double[N];

	for (int i = 0; i < N; i++)
	{
		y1[i] = abs(yy[i]);
	}

	fftw_destroy_plan(p);
	delete[] y;
	return y1;
}

double raw_edges::Spek_InterpolR (double* A) {

	uint32_t A_size = 256;
	uint32_t A2_size = A_size / 2;

	double* A2 = 0;
	A2 = new double[A2_size]();
	for (int i = 0; i < A2_size; i++)
	{
		A2[i] = A[i];
	}

	int n_0 = std::distance(A2, std::max_element(A2, A2 + A2_size));

	double y_ln1 = log(A[n_0 + 1]);
	double y_ln0 = log(A[n_0]);
	double y_ln_1 = log(A[n_0 - 1]);
	double tmp = (y_ln_1 - y_ln1) / (y_ln_1 - (2 * y_ln0) + y_ln1);
	double n_g = (n_0 + tmp / 2);
	delete[] A2;
	return n_g;
}

double raw_edges::Main_FreqR(double* B0, int start, int stop)
{
	double f_g = 0.0;
	const int size = stop - start;

	double* B = new double[size]();

	double* image_window = 0;
	image_window = new double[size]();

	for (int k = 0; k < size; k++) 
	{
		B[k] = B0[k + start];
	}
	
	double Mean = MeanR(size,B);

	double* B1 = new double[size]();

	for (int i = 0; i < size; i++)
	{
		B1[i] = B[i] - Mean;
	}

	double* wFun = BlackmanWindowR(size);
	for (int i = 0; i < size; i++)
	{
		image_window[i] = B1[i] * wFun[i];
	}

	double* y1 = FFTR(image_window, size);

	double n_g = Spek_InterpolR(y1);
	uint32_t size_B = size;
	f_g = n_g / size_B;

	delete[] B1;
	delete[] B;
	delete[] wFun;
	delete[] y1;

	return f_g;
}

struct stage12 raw_edges::ExecuteR(Mat Image, int freq_range)
{
	buffers_init();
	int** ImgArr = Image2ArrayR(Image);
	Mat Image2 = ImageSliceR(Image, 2);
	int** ImgArr2 = Image2ArrayR(Image2);
	const int rows = Image2.rows;
	const int cols = Image2.cols;
	
	double* Mean0 = Mean0R(rows, cols, ImgArr2);
	
	double main_d_0 = 0;
	uint32_t size_Mean0 = cols;
	int n1 = ((size_Mean0 - freq_range) / 50) + 1;
	//int n1 = 12;
	double* t1 = 0;
	t1 = new double[n1]();

	int j = 0;
	for (int i = 0; i < (size_Mean0 - freq_range); i += 50)
	{
		double tmp = Main_FreqR(Mean0, i, i + freq_range);
		t1[j] = 1 / tmp;
		j++;
	}
 	main_d_0 = Median(n1,t1);

	double* Mean1 = Mean1R(rows, cols, ImgArr2);
	double main_d_1 = 0;
	uint32_t size_Mean1 = rows;
	int n2 = ((size_Mean1 - freq_range) / 50) + 1;
	//int n2 = 8;
	double* t2 = 0;
	t2 = new double[n2]();

	int k = 0;
	for (int i = 0; i < (size_Mean1 - freq_range); i += 50)
	{
		double tmp = Main_FreqR(Mean1, i, i + freq_range);
		t2[k] = 1 / tmp;
		k++;
	}
	main_d_1 = Median(n2, t2);

	//this value will be needed in 2nd bock which is calculated here
	double th_edge = MeanR(cols, Mean0);

	//To do - add mutex protection later

	for (int i = 0; i < Image.rows; i++)
	{
		memcpy(s12.img, ImgArr, (Image.cols * sizeof(int)));
	}
	
	for (int i = 0; i < Image2.rows; i++)
	{
		memcpy(s12.img2, ImgArr2, (Image2.cols* sizeof(int)));
	}
	
	memcpy(s12.mean0, Mean0, Image2.cols * sizeof(double));
	memcpy(s12.mean1, Mean1, Image2.rows * sizeof(double));
	s12.main_d_0 = main_d_0;
	s12.main_d_1 = main_d_1;
	s12.th_edge = th_edge;

	return s12;
}
