#include "grid_pos.h"
#include "utility.h"

using std::chrono::high_resolution_clock;

// A class for grid_pos function of python, wherein all 3 stages are called which is surrounded
// by the if condition of size of edges

stage56 grid_pos::Execute(stage23 s23)
{
	if ((s23.cut_ver.size() >= 2) && (s23.cut_hor.size() >= 2))
	{
		grid_pos01 grid1;
		grid_pos02 grid2;
		grid_pos03 grid3;
		
		auto t11 = high_resolution_clock::now();

		grid1.Execute(s23);

		utility::display_time(t11, high_resolution_clock::now());	
		auto t12 = high_resolution_clock::now();

		grid2.Execute(grid1.getNext());

		utility::display_time(t12, high_resolution_clock::now());
		auto t13 = high_resolution_clock::now();

		stage56 s56 = grid3.Execute(grid2.getNext());

		utility::display_time(t13, high_resolution_clock::now());

		return s56;
	}
	else
	{
		stage56 s56;
		s56.index = (int)nan("");
		s56.k = nan("");
		s56.ind_ori = ("");
		s56.grids = {};
		s56.xi = nan("");
		s56.zi = nan("");

		return s56;
	}

	
}