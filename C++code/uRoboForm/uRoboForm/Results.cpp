#include "Results.h"
#include <string>


void Results::write_to_csv(const string& filename,const stage56& s56, const string& orientation)
{
	ofstream myFile(filename, ios::app); 

	myFile << s56.xi << "  ; " << s56.zi << " ; " << s56.k << " ;  " << s56.index << "  ;  " << orientation  << "\n";
		//if (j != data1.size() - 1) myFile << ","; // No comma at end of line	
	myFile << "\n";

	myFile.close();
}

void Results::Execute(const stage56 &s56, std::chrono::high_resolution_clock::time_point t01)
{
	string orientation;

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