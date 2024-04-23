#include "signal_evaluation.h"

using namespace std;

double signal_evaluation::Spek_InterpolR(const vector<double>& A) 
{
	const auto A_size = 256;
	const auto A2_size = A_size / 2;

	const auto m = std::max_element(A.begin(), A.begin() + A2_size);
	auto n_0 = std::distance(A.begin(), m);

	const auto a = n_0 + 1;
	const auto b = n_0 - 1;
	const auto y_ln1 = log(A[a]);
	const auto y_ln0 = log(A[n_0]);
	const auto y_ln_1 = log(A[b]);
	const auto tmp = (y_ln_1 - y_ln1) / (y_ln_1 - (2 * y_ln0) + y_ln1);
	
	return (n_0 + tmp / 2);
}

double signal_evaluation::BlackmanWindowR(int n, int pos)
{
	const double a0 = 0.42;
	const double a1 = 0.5;
	const double a2 = 0.08;
	const int wLen = n - 1;
	vector<double> wFun(n);

	return a0 - (a1 * cos((2 * M_PI * pos) / wLen)) + (a2 * cos((4 * M_PI * pos) / wLen));
}

vector<double> signal_evaluation::FFTR(const vector<double>& image_windowR)
{
	size_t size = image_windowR.size();
	const int N = 256;
	fftw_complex* y = new fftw_complex[N];
	double in[N]{};
	fftw_plan p;

	for (int i = 0; i < N; ++i) {
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

	for (int i = 0; i < N; ++i)
	{
		y1[i] = abs(*reinterpret_cast<std::complex<double>*>(y + i));
	}

	fftw_destroy_plan(p);

	return y1;
}

vector<double> signal_evaluation::RFFT(const vector<double>& x)
{
	auto N = x.size();
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
	auto N = x.size();
	double* y = new double[N]();

	double* xx = new double[N]();
	xx[0] = x[0];
	for (int i = 1; i < N / 2; ++i)
	{
		int a = (2 * i) - 1;
		xx[i] = x[a];
	}

	for (size_t i = N; i > ((N / 2) + 1); --i)
	{
		size_t a = (2 * (N - i)) + 2;
		xx[i - 1] = x[a];
	}
	size_t b = N - 1;
	xx[N / 2] = x[b];

	fftw_plan p;

	p = fftw_plan_r2r_1d(N, xx, y, FFTW_HC2R, FFTW_ESTIMATE);//fftw_plan_dft_1d(N, in, y, FFTW_FORWARD, FFTW_ESTIMATE);

	fftw_execute(p);

	fftw_destroy_plan(p);

	vector<double> yy(N);
	for (int i = 0; i < N; ++i)
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
	
	fill(f_x.begin(), f_x.begin() + x0, 0);
	fill(f_x.begin() + x1, f_x.end(), 0);
	/*for (int i = 0; i < x0; ++i)
		f_x[i] = 0;

	for (size_t i = x1; i < x.size(); ++i)
		f_x[i] = 0;*/

	return IRFFT(f_x);
}

struct MFreq signal_evaluation::Main_FreqR(const vector<double>& B0, int start, int size)
{
	struct MFreq mf;
	auto istart = B0.begin() + start;

	vector<double> B1;
	B1.reserve(size);

	mf.Image_window.reserve(size);

	const double Mean = Evaluation::Mean(istart,istart+size);

	for (int i = 0; i < size; ++i)
		mf.Image_window.emplace_back((*(istart + i) - Mean) * BlackmanWindowR(size, i));

	vector<double> y1 = FFTR(mf.Image_window);

	mf.n_g = Spek_InterpolR(y1);
	mf.f_g = mf.n_g / size;

	return mf;
}
