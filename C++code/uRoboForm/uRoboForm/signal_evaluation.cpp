#include "signal_evaluation.h"

using namespace std;

double signal_evaluation::Spek_InterpolR(const vector<double>& A) 
{

	uint32_t A_size = 256;
	uint32_t A2_size = A_size / 2;

	vector<double> A2(A2_size);
	for (uint32_t i = 0; i < A2_size; i++)
	{
		A2[i] = A[i];
	}

	int n_0 = (int)std::distance(A2.begin(), std::max_element(A2.begin(), A2.begin() + A2_size));

	int a = n_0 + 1;
	int b = n_0 - 1;
	double y_ln1 = log(A[a]);
	double y_ln0 = log(A[n_0]);
	double y_ln_1 = log(A[b]);
	double tmp = (y_ln_1 - y_ln1) / (y_ln_1 - (2 * y_ln0) + y_ln1);
	double n_g = (n_0 + tmp / 2);

	return n_g;
}

vector<double> signal_evaluation::BlackmanWindowR(int n)
{
	const double a0 = 0.42;
	const double a1 = 0.5;
	const double a2 = 0.08;
	int wLen = n - 1;
	vector<double> wFun(n);

	for (int i = 0; i < n; ++i)
	{
		double wi = 0.0;
		wi = a0 - (a1 * cos((2 * M_PI * i) / wLen)) + (a2 * cos((4 * M_PI * i) / wLen));
		wFun[i] = wi;
	}
	
	return wFun;
}

vector<double> signal_evaluation::FFTR(const vector<double>& image_windowR)
{
	size_t size = image_windowR.size();
	const int N = 256;
	fftw_complex* y = 0;
	y = new fftw_complex[N];
	double in[N]{};
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
	vector<double> y1(N);

	for (int i = 0; i < N; i++)
	{
		y1[i] = abs(yy[i]);
	}

	fftw_destroy_plan(p);

	return y1;
}

vector<double> signal_evaluation::RFFT(const vector<double>& x)
{
	size_t N = x.size();
	double* x_arr = new double[N]();
	copy(x.begin(), x.end(), x_arr);
	double* y = new double[N]();
	fftw_plan p;

	p = fftw_plan_r2r_1d((int)N, x_arr, y, FFTW_R2HC, FFTW_ESTIMATE);//fftw_plan_dft_1d(N, in, y, FFTW_FORWARD, FFTW_ESTIMATE);

	fftw_execute(p);

	fftw_destroy_plan(p);

	vector<double> yy(N);
	yy[0] = y[0];
	int j = 0;
	for (int i = 1; i < N; i += 2)
	{
		int a = i - j;
		yy[i] = y[a];
		j++;
	}

	int k = 1;
	for (int i = 2; i < N; i += 2)
	{
		size_t a = N - k;
		yy[i] = (y[a]);
		k++;
	}

	delete[] y;
	return yy;
}

vector<double> signal_evaluation::IRFFT(const vector<double>& x)
{
	size_t N = x.size();
	double* y = new double[N]();

	double* xx = new double[N]();
	xx[0] = x[0];
	for (int i = 1; i < N / 2; i++)
	{
		int a = (2 * i) - 1;
		xx[i] = x[a];
	}

	for (size_t i = N; i > ((N / 2) + 1); i--)
	{
		size_t a = (2 * (N - i)) + 2;
		xx[i - 1] = x[a];
	}
	size_t b = N - 1;
	xx[N / 2] = x[b];

	fftw_plan p;

	p = fftw_plan_r2r_1d((int)N, xx, y, FFTW_HC2R, FFTW_ESTIMATE);//fftw_plan_dft_1d(N, in, y, FFTW_FORWARD, FFTW_ESTIMATE);

	fftw_execute(p);

	fftw_destroy_plan(p);

	vector<double> yy(N);
	for (int i = 0; i < N; i++)
	{
		y[i] /= N;
		yy[i] = y[i];
	}

	delete[] xx;
	return yy;
}

vector<double>  signal_evaluation::Bandfilter(const vector<double>& x, int x0, size_t x1)
{
	vector<double> f_x = RFFT(x);

	vector<double> f_x_cut;
	f_x_cut = f_x;

	for (int i = 0; i < x0; i++)
	{
		f_x_cut[i] = 0;
	}

	for (size_t i = x1; i < x.size(); i++)
	{
		f_x_cut[i] = 0;
	}

	vector<double> x_cut = IRFFT(f_x_cut);

	return x_cut;
}

struct MFreq signal_evaluation::Main_FreqR(const vector<double>& B0, int start, int stop)
{
	struct MFreq mf;
	double f_g = 0.0;
	const int size = stop - start;

	vector<double> B(size);

	mf.Image_window.reserve(size);


	for (int k = 0; k < size; k++)
	{
		int w = k + start;
		B[k] = B0[w];
	}

	double Mean = Evaluation::MeanR(B);

	vector<double> B1(size);

	for (int i = 0; i < size; i++)
	{
		double x = B[i] - Mean;
		B1[i] = x;
	}

	vector<double> wFun = BlackmanWindowR(size);
	for (int i = 0; i < size; i++)
	{
		mf.Image_window.push_back(B1[i] * wFun[i]);
	}

	vector<double> y1 = FFTR(mf.Image_window);

	mf.n_g = Spek_InterpolR(y1);
	uint32_t size_B = size;
	mf.f_g = mf.n_g / size_B;

	return mf;
}
