#include "main.h"

void display_time(const chrono::steady_clock::time_point& t01,
	const chrono::steady_clock::time_point& t02)
{
	/* Getting number of milliseconds as an integer. */
	auto ms_int = duration_cast<milliseconds>(t02 - t01);
	duration<double, milli> ms_double = t02 - t01;
	cout << "Time: " << ms_double.count() << "ms" << endl;
	cout << endl;
}

int main(int argc, char* argv[])
{
	string filename = "D:/Vaishnavi/C++Trial/Images/001.bmp";
	if (argc > 1)
		filename = argv[1];
	
	// Read the image file in grayscale
	Mat image = imread(filename, IMREAD_GRAYSCALE);

	if (image.cols == 0)
	{
		std::cout << "Image broken" << std::endl;
		exit(1);
	}

	raw_edges edge0{};
	find_edges edge{};

	auto t01 = high_resolution_clock::now();

	edge0.ExecuteR(image, freq_range);

	display_time(t01, high_resolution_clock::now());

	auto t02 = high_resolution_clock::now();

	struct stage23 s23 = edge.Execute(edge0.getNext());

	display_time(t02, high_resolution_clock::now());

	/*grid_pos01 grid(s23);
	t02 = high_resolution_clock::now();
	struct stage34 s34 = grid.Execute();
	display_time(t02, high_resolution_clock::now());
	*/

	std::cout << "Complete runtime:";
	display_time(t01, high_resolution_clock::now());

	return 0;

}
 
