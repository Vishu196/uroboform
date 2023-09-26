#pragma once
#include "NumCpp.hpp"
class find_edges02
{
private:
	
	static const int search_range = 150;
	int mid, i0, rank, s_max, s_min;
	nc::NdArray<double>th_edge;
	nc::NdArray<double>mean_range0, mean_range1;
	nc::NdArray<double>im_col, std_col ,im_row_low, std_row_low, im_row, std_row;
	float cond_col, cond_row_low, cond_row;
	nc::NdArray<int>through_loc, d_through, cut_through;
	int ideal_d;

	int line_index(nc::NdArray<double>mean_range0, nc::NdArray<double>th_edge,int i0,int rank);
	auto detect_through(nc::NdArray<double>im_col, nc::NdArray<double>th_edge);
	auto delete_edges(nc::NdArray<int>cut_hor,int ideal_d);

public:

	nc::NdArray<int>cut_hor,cut_ver;

	//auto mean_range0();
	

};

