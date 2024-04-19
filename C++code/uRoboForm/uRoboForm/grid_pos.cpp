#include "grid_pos.h"
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

// A class for grid_pos function of python, wherein all 3 stages are called which is surrounded
// by the if condition of size of edges

void display_time01(const chrono::steady_clock::time_point& t01,
	const chrono::steady_clock::time_point& t02)
{
	/* Getting number of milliseconds as an integer. */
	auto ms_int = duration_cast<milliseconds>(t02 - t01);
	duration<double, milli> ms_double = t02 - t01;
	cout << "Time: " << ms_double.count() << "ms" << endl;
	cout << endl;
}

stage56 grid_pos::Execute(stage23 s23)
{
	stage56 s56;

	if ((s23.cut_ver.size() >= 2) && (s23.cut_hor.size() >= 2))
	{
		grid_pos01 grid1{};
		grid_pos02 grid2{};
		grid_pos03 grid3{};
		
		auto t11 = high_resolution_clock::now();
		
		grid1.Execute(s23);
		display_time01(t11, high_resolution_clock::now());
		
		auto t12 = high_resolution_clock::now();
		
		grid2.Execute(grid1.getNext());
		display_time01(t12, high_resolution_clock::now());

		auto t13 = high_resolution_clock::now();

		grid3.Execute(grid2.getNext(), s56);
		display_time01(t13, high_resolution_clock::now());
	}
	else
	{
		s56.index = nan("");
		s56.k = nan("");
		s56.ind_ori = nan("");
		s56.grids = {};
		s56.xi = nan("");
		s56.zi = nan("");
	}

	return s56;
}