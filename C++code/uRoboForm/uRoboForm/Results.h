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
	void Execute(const stage56& s56);


public:
	Results(grid_pos03& grid3)
	{
#ifdef WITH_THREADING
		std::thread t1([&]
			{
#endif
				while (1)
				{
					auto t05 = std::chrono::high_resolution_clock::now();
					const stage56& s56 = grid3.getNext();
					if (s56.grids == nullptr)
					{
						break;
					}
					Execute(s56);
					utility::display_time(t05, std::chrono::high_resolution_clock::now());
					//exit(1);
				}
#ifdef WITH_THREADING
			});
		t1.detach();
#endif

	}
};

	

