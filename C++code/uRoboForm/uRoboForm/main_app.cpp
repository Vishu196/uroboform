#include "main.h"
#include "find_edges01.h"
#include <chrono>

//#define Debug	

typedef nc::NdArray<double> ArrD;

ArrD BlackmanWindow(int n, bool sflag)
{
	const double a0 = 0.42;
	const double a1 = 0.5;
	const double a2 = 0.08;
	int wLen = n-1;
	ArrD wFun = nc::NdArray<double>(n,1);

	for (int i = 0; i < n; ++i)
	{
		double wi = 0.0;
		wi = a0 - (a1 * cos((2 * M_PI * i) / wLen)) + (a2 * cos((4 * M_PI * i) / wLen));
		wFun[i] = wi;
	}
	return wFun;
}

double spek_interpol(ArrD A) {

	uint32_t A_size = nc::size(A);
	uint32_t A2_size = A_size / 2;

	ArrD A2 = nc::NdArray<double>(A2_size,1);
	A2 = A[nc::Slice(A2_size)];
	auto n_0 = nc::argmax(A2);

	double y_ln1 = nc::log(A[n_0[0]+1]);
	double y_ln0 = nc::log(A[n_0[0]]);
	double y_ln_1 = nc::log(A[n_0[0]-1]);
	double tmp = (y_ln_1 - y_ln1) / (y_ln_1 - (2 * y_ln0) + y_ln1);
	double n_g = (n_0[0] + tmp/2);

	return n_g;
}

ArrD FFT(ArrD image_window,int size)
{
	const int N = 256;
	fftw_complex  y[N];
	double in[N];
	fftw_plan p;

	for (int i = 0; i < N; i++) {
		if (i < size) {
			in[i] = image_window[i];
		}
		else {
			in[i] = 0;
		}
	}
	p = fftw_plan_dft_r2c_1d(N, in, y, FFTW_ESTIMATE);//fftw_plan_dft_1d(N, in, y, FFTW_FORWARD, FFTW_ESTIMATE);

	fftw_execute(p);
	std::complex<double>* yy;
	yy = reinterpret_cast<std::complex<double> *>(y);
	auto y1 = nc::NdArray<double>(N, 1);

	for (int i = 0; i < N; i++)
	{
		y1[i] = nc::abs(yy[i]);
	}

	fftw_destroy_plan(p);
	return y1;
}

double Main_Freq(ArrD B0, int start, int stop)
{
	const int size = stop - start;
	auto B = nc::NdArray<double>(size,1);
	ArrD image_window = nc::NdArray<double>(150,1);

	for (int k = 0; k < size; k++) {
		B[k] = B0[k + start];
	}
	//auto Mean = nc::NdArray<double>(1);
	auto Mean = nc::mean(B);

	auto B1 = nc::NdArray<double>(150,1);
	for (int i = 0; i < 150; i++)
	{
		B1[i] = B[i] - Mean[0];
	}
	
	ArrD wFun = BlackmanWindow(150, true);
	for (int i = 0; i< 150; i++)
	{
		image_window[i] = B1[i] * wFun[i];
	}

	ArrD y1 = FFT(image_window, size);

	double n_g = spek_interpol(y1);
	double f_g = n_g / nc::size(B);

	return f_g;
}

int main()
{
	using std::chrono::high_resolution_clock;
	using std::chrono::duration_cast;
	using std::chrono::duration;
	using std::chrono::milliseconds;

	auto t01 = high_resolution_clock::now();
	// Read the image file in grayscale
	Mat image = imread("D:/Vaishnavi/C++Trial/Images/001.bmp",IMREAD_GRAYSCALE);
	const auto nRows = image.rows;
	const auto nCols = image.cols;
	const auto step = image.step;
	uint8_t* p = image.data;

	auto ncArray = nc::NdArray<nc::uint8>(nRows, nCols);
	for (int i = 0; i < nRows; i++)
	{
		for (int j = 0; j < nCols; j++)
		{
			ncArray(i,j) = p[i * image.step + j];
		}

	}

	auto p2 = ncArray(nc::Slice(0, nRows, 2), nc::Slice(0, nCols, 2));
	auto mean0 = nc::mean(p2, nc::Axis::ROW);
	auto mean1 = nc::mean(p2, nc::Axis::COL);
	auto main_d_0 = nc::NdArray<double>(1,1);
	auto t1 = nc::NdArray<double>(12,1);
	int j = 0; 
	for (int i = 0; i < (nc::size(mean0) - 150); i += 50) 
	{
		double tmp = Main_Freq(mean0, i, i + 150);
		t1[j] = 1 / tmp;
		j++;
			
	}
	main_d_0 = nc::median(t1);

	cout << "main_d_0: " << main_d_0;

	auto t02 = high_resolution_clock::now();

	/* Getting number of milliseconds as an integer. */
	auto ms_int = duration_cast<milliseconds>(t02 - t01);
	duration<double, std::milli> ms_double = t02 - t01;
	std::cout << ms_double.count() << "ms\n";

	return 0;
}
 
