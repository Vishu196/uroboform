#pragma once

class bandfilter
{
public:

	bandfilter(double* x, int* limits);
	

	double* RFFT(double* x);
	double* IRFFT();
	double* Bandfilter(double* x, int* limits);


};

