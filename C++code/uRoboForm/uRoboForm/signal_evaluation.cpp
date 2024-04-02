#include "signal_evaluation.h"

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
	/*wFun.shrink_to_fit();*/
	return wFun;
}

vector<double> signal_evaluation::FFTR(const vector<double>& image_windowR)
{
	int size = (int)image_windowR.size();
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
	//delete[] y;

	return y1;
}

vector<double> signal_evaluation::RFFT(const vector<double>& x)
{
	int N = (int)x.size();
	double* x_arr = new double[N]();
	copy(x.begin(), x.end(), x_arr);
	double* y = new double[N]();
	fftw_plan p;

	p = fftw_plan_r2r_1d(N, x_arr, y, FFTW_R2HC, FFTW_ESTIMATE);//fftw_plan_dft_1d(N, in, y, FFTW_FORWARD, FFTW_ESTIMATE);

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
		int a = N - k;
		yy[i] = (y[a]);
		k++;
	}

	//yy.shrink_to_fit();
	delete[] y;
	return yy;
}

vector<double> signal_evaluation::IRFFT(const vector<double>& x)
{
	int N = (int)x.size();
	double* y = new double[N]();

	double* xx = new double[N]();
	xx[0] = x[0];
	for (int i = 1; i < N / 2; i++)
	{
		int a = (2 * i) - 1;
		xx[i] = x[a];
	}

	for (int i = N; i > ((N / 2) + 1); i--)
	{
		int a = (2 * (N - i)) + 2;
		xx[i - 1] = x[a];
	}
	int b = N - 1;
	xx[N / 2] = x[b];

	fftw_plan p;

	p = fftw_plan_r2r_1d(N, xx, y, FFTW_HC2R, FFTW_ESTIMATE);//fftw_plan_dft_1d(N, in, y, FFTW_FORWARD, FFTW_ESTIMATE);

	fftw_execute(p);

	fftw_destroy_plan(p);

	vector<double> yy(N);
	for (int i = 0; i < N; i++)
	{
		y[i] /= N;
		yy[i] = y[i];
	}

	delete[] xx;
	/*yy.shrink_to_fit();*/
	return yy;
}

vector<double>  signal_evaluation::Bandfilter(const vector<double>& x, int x0, int x1)
{
	vector<double> f_x = RFFT(x);

	vector<double> f_x_cut;
	f_x_cut = f_x;

	for (int i = 0; i < x0; i++)
	{
		f_x_cut[i] = 0;
	}

	for (int i = x1; i < x.size(); i++)
	{
		f_x_cut[i] = 0;
	}

	vector<double> x_cut = IRFFT(f_x_cut);

	return x_cut;
}
