#include "signal_evaluation.h"
#include "Evaluation.h"
#include <fftw3.h>
#include <complex>


using namespace std;

std::mutex global_fftw_mutex;

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
	std::vector<complex<double>> y(N);
	std::vector<double> in(N, 0);
	fftw_plan p;

	for (int i = 0; i < image_windowR.size(); ++i)
	{
		in[i] = image_windowR[i];
	}
	{
		std::scoped_lock lock(global_fftw_mutex);
		fftw_plan p = fftw_plan_dft_r2c_1d(N, in.data(), reinterpret_cast<fftw_complex*>(y.data()), FFTW_ESTIMATE);//fftw_plan_dft_1d(N, in, y, FFTW_FORWARD, FFTW_ESTIMATE);
		fftw_execute(p);
		fftw_destroy_plan(p);
	}
	for (int i = 0; i < N; i++)
	{
		in[i] = abs(y[i]);
	}
	return in;
}

std::vector<complex<double>> signal_evaluation::RFFT(const vector<double>& x)
{
	auto N = x.size();
	std::vector<complex<double>> y(N);
	{
		std::scoped_lock lock(global_fftw_mutex);
		fftw_plan p = fftw_plan_dft_r2c_1d((int)N, const_cast<double*>(x.data()), reinterpret_cast<fftw_complex*>(y.data()),FFTW_ESTIMATE );//fftw_plan_dft_1d(N, in, y, FFTW_FORWARD, FFTW_ESTIMATE);

		fftw_execute(p);
		fftw_destroy_plan(p);
	}

	return y;
}

vector<double> signal_evaluation::IRFFT( std::vector<complex<double>>& x)
{
	auto N = x.size();
	std::vector<double> xx(N);

	std::vector<double> y(N);
	{
		std::scoped_lock lock(global_fftw_mutex);
		fftw_plan p = fftw_plan_dft_c2r_1d(N, reinterpret_cast<fftw_complex*>(x.data()), y.data(), FFTW_ESTIMATE);//fftw_plan_dft_1d(N, in, y, FFTW_FORWARD, FFTW_ESTIMATE);
		fftw_execute(p);
		fftw_destroy_plan(p);
	}

	for (int i = 0; i < N; i++)
	{
		y[i] /= N;
	}

	return y;
}

vector<double>  signal_evaluation::Bandfilter(const vector<double>& x, int x0, size_t x1)
{
	int x2 = (x1 / 2);
	std::vector<std::complex<double>> f_x = RFFT(x);
	fill(f_x.begin(), f_x.begin() + x0, 0);
	fill(f_x.begin() + x2 +1, f_x.end(), 0);
	if ((x1 & 1) == 0)
	{
		(f_x.at(x2)) = real(f_x.at(x2));
	}
	
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
