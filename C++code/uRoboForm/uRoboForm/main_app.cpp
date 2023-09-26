#include "main.h"
#include "find_edges01.h"


//#define Debug


int main()
{
	// Read the image file in grayscale
	Mat image = imread("D:/Vaishnavi/C++Trial/Images/001.bmp",IMREAD_GRAYSCALE);


	return 0;
}
 
int main01()
{
	fftw_complex* in, * y;
	fftw_plan p;


	int N = 8;

	in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
	y = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);


	p = fftw_plan_dft_1d(N, in, y, FFTW_FORWARD, FFTW_ESTIMATE);

	fftw_execute(p); /* repeat as needed */

	std::cout << *y;
	fftw_destroy_plan(p);
	fftw_free(in); fftw_free(y);

	return 0;
}
