#include "main.h"
#include "raw_edges.h"
#include "find_edges.h"
//#include "grid_pos01.h"


//#define Debug	

int main()
{
	using std::chrono::high_resolution_clock;
	using std::chrono::duration_cast;
	using std::chrono::duration;
	using std::chrono::milliseconds;

	// Read the image file in grayscale
	Mat image = imread("D:/Vaishnavi/C++Trial/Images/001.bmp",IMREAD_GRAYSCALE);

	auto t01 = high_resolution_clock::now();

	raw_edges edge0(image);
	struct stage12 s12 = edge0.ExecuteR(image, freq_range);
	cout << "Stage 1 complete." << endl;

	find_edges edge(s12);
	struct stage23 s23 = edge.Execute();
	
	cout << "cut_hor: ";
	for (auto v : s23.cut_hor)
		cout << v << ",";
	cout << endl << "cut_ver: ";
	for (auto v : s23.cut_ver)
		cout << v << ",";
	cout << endl;
	cout << "Stage 2 complete." << endl;

	/*grid_pos01 grid(s23);
	struct stage34 s34 = grid.Execute();*/

	auto t02 = high_resolution_clock::now();

	/* Getting number of milliseconds as an integer. */
	auto ms_int = duration_cast<milliseconds>(t02 - t01);
	duration<double, milli> ms_double = t02 - t01;
	cout << endl;
	cout << "T1 : " << ms_double.count() << "ms\n";

	return 0;
}
 
