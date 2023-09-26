#pragma once
#include "NumCpp.hpp"

using namespace std;

class grid_pos01
{
private:

	nc::NdArray<double>mean2_0; //1D array
	nc::NdArray<double>mean2_1; //1D array
	nc::NdArray<double>mean_grad0; //1D array
	nc::NdArray<double>mean_grad1; //1D array
	nc::NdArray<int>grid0, grid_rot, grid_cut; //2D array
	double five_percent;
	int row, col;
	nc::NdArray<double>max_pos, pres; //1D array

	auto flip_grid(nc::NdArray<int>grid0);
	auto cut_grid(nc::NdArray<int>grid_rot);
	auto subpx_max_pos(nc::NdArray<int>grid_cut, int stripe_width, float px_size, char mode);


public: 
	string orientation;

	void find_grids();

};

