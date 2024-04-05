#pragma once
#include <fftw3.h>
#include <complex>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

using namespace std;

class signal_evaluation
{
public:

	double static Spek_InterpolR(const vector<double>& A);
	vector<double> static BlackmanWindowR(int n);
	vector<double> static FFTR(const vector<double>& image_window);
	vector<double> static Bandfilter(const vector<double>& x, int start, size_t end);
	vector<double> static RFFT(const vector<double>& x);
	vector<double> static IRFFT(const vector<double>& x);
};

