#pragma once
#include <fftw3.h>
#include <complex>
#include <vector>
#include "Evaluation.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

struct MFreq
{
	std::vector<double> Image_window;
	double f_g;
	double n_g;

	MFreq() : f_g(0.0), n_g(0.0)
	{}
};

class signal_evaluation
{
public:

	double static Spek_InterpolR(const std::vector<double>& A);
	double static BlackmanWindowR(int n,int pos);
	std::vector<double> static FFTR(const std::vector<double>& image_window);
	std::vector<double> static Bandfilter(const std::vector<double>& x, int start, size_t end);
	std::vector<double> static RFFT(const std::vector<double>& x);
	std::vector<double> static IRFFT(const std::vector<double>& x);
	struct MFreq static Main_FreqR(const std::vector<double>& B0, int start, int stop);
};

