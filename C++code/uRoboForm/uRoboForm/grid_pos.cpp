#include "grid_pos.h"

//int grid_pos::Execute(stage23 s23)
//{
//	stage56 s56;
//	string mode = "gauss";
//	string orientation;
//
//	if ((s23.cut_ver.size() >= 2) && (s23.cut_hor.size() >= 2))
//	{
//		grid_pos01 grid1{};
//		grid_pos02 grid2{};
//		grid_pos03 grid3{};
//		
//		int s34 = grid1.Execute(s23);
//		grid2.Execute(grid1.getNext());
//		s56  = grid3.Execute(grid2.getNext());
//	}
//	else
//	{
//		s56.index = nan("");
//		s56.k = nan("");
//		s56.ind_ori = nan("");
//		s56.grids = {};
//		s56.xi = nan("");
//		s56.zi = nan("");
//	}
//
//	return s56;
//}