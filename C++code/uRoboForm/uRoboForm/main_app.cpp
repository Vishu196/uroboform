#include "main.h"
#include <string>
#include <thread>
#include "utility.h"
#include "constants.h"
#include "cqueue.h"
#include "ImgSource.h"
#include "raw_edges.h"
#include "find_edges.h"
#include "grid_pos01.h"
#include "grid_pos02.h"
#include "grid_pos03.h"
#include "grid.h"
#include "Results.h"


void create_csv(const std::string& filename, const vector<std::string>& colname)
{
	// Create an output filestream object
	ofstream myFile(filename);

	for (int j = 0; j < colname.size(); ++j)
	{
		myFile << colname.at(j);
		if (j != colname.size() - 1) myFile << ","; // No comma at end of line
	}
	myFile << "\n";
}

using namespace cv;
using namespace std;

std::mutex output_mutex;

int main(int argc, char* argv[])
{
	string csvname = "Result";
	std::vector<std::string> colname = { "   xi    ","   zi    ","    k    "," index ","  orientation " };
	create_csv(csvname, colname);

	string path = "D:\\Vaishnavi\\C++Trial\\Images";
	//string path = "D:\\Vaishnavi\\C++Trial\\terlau2";


	auto t01 = std::chrono::high_resolution_clock::now();

	if (argc > 1)
		path = argv[1];

#ifdef WITH_THREADING
		Source imgsrc(path,2);
#else
		Source imgsrc(path);
#endif	
		raw_edges edge0(imgsrc, freq_range);
		find_edges edge(edge0);
		grid_pos01 grid1(edge);
		grid_pos02 grid2(grid1);
		grid_pos03 grid3(grid2);	
		Results output(grid3, t01);
		
#ifdef WITH_THREADING
	while (1);
#endif
	return 0;
}
 
