#include "find_edges01.h"



find_edges01::find_edges01(nc::NdArray<int>Image)
{
	ArrI ImageArray = Image;
}

/*ArrI find_edges01::SliceImage(ArrI ImageArray)
{
	ArrI image2 = nc::NdArray<int>(ImageArray.numRows() / 2, ImageArray.numCols() / 2);
	image2 = ImageArray(nc::Slice(0, ImageArray.numRows(), 2), nc::Slice(0, ImageArray.numCols(), 2));
	return image2;
}*/

ArrD find_edges01::BlackmanWindow(int n, bool sflag)
{
	const double a0 = 0.42;
	const double a1 = 0.5;
	const double a2 = 0.08;
	int wLen = n-1;
	auto wFun = nc::NdArray<double>(n);

	for (int i = 0; i < n; ++i)
	{
		double wi = 0.0;
		wi = a0 - (a1 * cos((2 * M_PI * i) / wLen)) + (a2 * cos((4 * M_PI * i) / wLen));
		wFun[i] = wi;
	}
	return wFun;
}

ArrD find_edges01::FFT(ArrD image_window, int size)
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
	yy = reinterpret_cast<std::complex<double>*>(y);
	auto y1 = nc::NdArray<double>(N, 1);

	for (int i = 0; i < N; i++)
	{
		y1[i] = nc::abs(yy[i]);
	}

	fftw_destroy_plan(p);
	return y1;
}

double find_edges01::Spek_Interpol(ArrD A) {

	uint32_t A_size = nc::size(A);
	uint32_t A2_size = A_size / 2;

	ArrD A2 = nc::NdArray<double>(A2_size, 1);
	A2 = A[nc::Slice(A2_size)];
	auto n_0 = nc::argmax(A2);

	double y_ln1 = nc::log(A[n_0[0] + 1]);
	double y_ln0 = nc::log(A[n_0[0]]);
	double y_ln_1 = nc::log(A[n_0[0] - 1]);
	double tmp = (y_ln_1 - y_ln1) / (y_ln_1 - (2 * y_ln0) + y_ln1);
	double n_g = (n_0[0] + tmp / 2);

	return n_g;
}

double find_edges01::Main_Freq(ArrD B0, int start, int stop)
{
	const int size = stop - start;
	auto B = nc::NdArray<double>(size, 1);
	ArrD image_window = nc::NdArray<double>(150, 1);

	for (int k = 0; k < size; k++) {
		B[k] = B0[k + start];
	}
	//auto Mean = nc::NdArray<double>(1);
	auto Mean = nc::mean(B);

	auto B1 = nc::NdArray<double>(150, 1);
	for (int i = 0; i < 150; i++)
	{
		B1[i] = B[i] - Mean[0];
	}

	ArrD wFun = BlackmanWindow(150, true);
	for (int i = 0; i < 150; i++)
	{
		image_window[i] = B1[i] * wFun[i];
	}

	ArrD y1 = FFT(image_window, size);

	double n_g = Spek_Interpol(y1);
	double f_g = n_g / nc::size(B);

	return f_g;
}

ArrD find_edges01::Execute(ArrI ImageArray, int freq_range)
{
	ArrD main_d_0 = nc::NdArray<double>(1, 1);
	ArrD main_d_1 = nc::NdArray<double>(1, 1);
	int nRows = ImageArray.numRows();
	int nCols = ImageArray.numCols();
	auto image2 = ImageArray(nc::Slice(0, nRows, 2), nc::Slice(0, nCols, 2));
	auto Mean0 = nc::mean(image2, nc::Axis::ROW);
	auto t1 = nc::NdArray<double>(12, 1);
	int j = 0;
	int ran = nc::size(Mean0) - freq_range;
	for (int i = 0; i < ran; i += 50)
	{
		double tmp = Main_Freq(Mean0, i, i + freq_range);
		t1[j] = 1 / tmp;
		j++;

	}
	main_d_0 = nc::median(t1);
	return main_d_0;
}