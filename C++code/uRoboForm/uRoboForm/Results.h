#pragma once
#include "cqueue.h"
#include "ImgSource.h"
#include "utility.h"
#include "grid_pos03.h"
#include <vector>
#include <thread>

class Results
{
private:

	void write_to_csv(const std::string& filename, const stage56 &s56, const std::string& orientation);
	//cqueue<stage56> fifo;
	void Execute(const stage56& s56, std::chrono::high_resolution_clock::time_point t01);


public:
	Results(grid_pos03& grid3, chrono::high_resolution_clock::time_point t01)
	{
#ifdef WITH_THREADING
		std::thread t1([&]
			{
#endif
				while (1)
				{
					//auto t05 = std::chrono::high_resolution_clock::now();
					const stage56& s56 = grid3.getNext();
					if (s56.grids == nullptr && s56.last_image != (-1))
					{
						cout << "Complete runtime:";
						utility::display_time(t01, std::chrono::high_resolution_clock::now());
						break;

					}
					Execute(s56, t01);
					/*cout << "Data added to csv" << endl;
					utility::display_time(t05, std::chrono::high_resolution_clock::now());*/
				}
				//exit(1);
#ifdef WITH_THREADING
			});
		t1.detach();
#endif
		
	}
};

	

