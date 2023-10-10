#include "main.h"
#include "find_edges01.h"
#include "raw_edges.h"

//#define Debug	

typedef nc::NdArray<double> ArrD;

int main()
{
	using std::chrono::high_resolution_clock;
	using std::chrono::duration_cast;
	using std::chrono::duration;
	using std::chrono::milliseconds;
	// Read the image file in grayscale
	Mat image = imread("D:/Vaishnavi/C++Trial/Images/001.bmp",IMREAD_GRAYSCALE);

	auto t01 = high_resolution_clock::now();

	raw_edges i1(image);
	struct stage12 s12 = i1.ExecuteR(image, freq_range);
	cout << s12.main_d_0;

	auto t02 = high_resolution_clock::now();

	/* Getting number of milliseconds as an integer. */
	auto ms_int = duration_cast<milliseconds>(t02 - t01);
	duration<double, std::milli> ms_double = t02 - t01;
	std::cout << "T1 : " << ms_double.count() << "ms\n";

	return 0;
}
 
