#include "Results.h"
#include <string>

/*This is the last class in algorithm implementation which has functions to write the output result of multiple images in a common csv
file which is created in main function and passed as argument*/

void Results::write_to_csv(const string& filename,const stage56& s56, const string& orientation)
{
	ofstream myFile(filename, ios::app); 

	myFile << fixed << setprecision(10);
	myFile << s56.xi << "  ; " << s56.zi << " ; " << s56.k << " ;  " << s56.index << "  ;  " << orientation ;
	myFile << "\n";

	myFile.close();
}

void Results::Execute(const stage56 &s56, std::chrono::high_resolution_clock::time_point t01)
{
	string orientation;

	// is_hor is a boolen parameter used to store the orientation, so here conversion is done to
	// interprate orientation as hor, ver or non in csv file
	if (s56.is_hor)
	{
		orientation = "hor";
	}
	else if (s56.index == 400)
	{
		orientation = "non";
	}
	else
	{
		orientation = "ver";
	}

	string csvname = "Result";
	write_to_csv(csvname, s56, orientation);
	
}