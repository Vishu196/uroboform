#include "main.h"

using namespace cv;
using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;


void display_time(const chrono::steady_clock::time_point& t01,
	const chrono::steady_clock::time_point& t02)
{
	/* Getting number of milliseconds as an integer. */
	auto ms_int = duration_cast<milliseconds>(t02 - t01);
	duration<double, milli> ms_double = t02 - t01;
	cout << "Time: " << ms_double.count() << "ms" << endl;
	cout << endl;
}

void write_to_csv(string filename, vector<string>colname, vector<vector<double>> data1, vector<int> index, vector<string>ori)
{	
	// Create an output filestream object
	ofstream myFile(filename);

	// Send column names to the stream
	for (int j = 0; j < colname.size(); ++j)
	{
		myFile << colname.at(j);
		if (j != colname.size() - 1) myFile << ","; // No comma at end of line
	}
	myFile << "\n";

	// Send data to the stream
	for (int j = 0; j < data1.size(); ++j) 
	{
			myFile << data1[j][0] << " ; " << data1[j][1] << " ; " << data1[j][2] << " ;   " << index[j] << "   ;    " << ori[j] << "\n";
			if (j != data1.size() - 1) myFile << ","; // No comma at end of line	
	}
		myFile << "\n";
	

	// Close the file
	myFile.close();
}

int main(int argc, char* argv[])
{
	string csvname = "Result";
	string foldername = "D:/Vaishnavi/C++Trial/Images";
	
	string filename = "D:/Vaishnavi/C++Trial/Images/001.bmp";
	if (argc > 1)
		filename = argv[1];

	// storing all o/p values to respective vector lists and then passing list as i/p to csv fn so that we can get
	//a table of all o/p values.
	
	vector<double> xi_i;
	vector<double> zi_i;
	vector<double> k_i;
	vector<int> index_i;
	vector<string> ori_i;

	//start the for loop here for more than 1 image

	// Read the image file in grayscale
	Mat image = imread(filename, IMREAD_GRAYSCALE);

	if (image.cols == 0)
	{
		std::cout << "Image broken" << std::endl;
		exit(1);
	}

	raw_edges edge0{};
	find_edges edge{};
	grid_pos grid_final{};
	
	auto t01 = high_resolution_clock::now();

	edge0.ExecuteR(image);

	display_time(t01, high_resolution_clock::now());

	auto t02 = high_resolution_clock::now();

	edge.Execute(edge0.getNext());

	display_time(t02, high_resolution_clock::now());

	auto t03 = high_resolution_clock::now();

	stage56 s56 = grid_final.Execute(edge.getNext());
	
	display_time(t03, high_resolution_clock::now());

	std::cout << "Complete runtime:";
	display_time(t01, high_resolution_clock::now());

	xi_i.push_back(s56.xi);
	zi_i.push_back(s56.zi);
	k_i.push_back(s56.k);
	index_i.push_back(s56.index);
	ori_i.push_back(s56.ind_ori);
	
	////for loop ends

	vector<vector<double>> data1(1, vector<double>(3));
	for (size_t i = 0; i < xi_i.size(); i++)
	{
		data1[i][0] = xi_i[i];
		data1[i][1] = zi_i[i];
		data1[i][2] = k_i[i];
	}
	 
	vector<string> colname = { "  xi    ","   zi    ","    k    ","  index  ","  orientation "};
	
	write_to_csv(csvname, colname, data1, index_i, ori_i);

	return 0;

}
 
