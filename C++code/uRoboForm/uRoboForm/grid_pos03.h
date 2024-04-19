#pragma once
#include "grid_pos02.h"
#include "grid.h"
#include "constants.h"
#include"Evaluation.h"
#include "cqueue.h"

struct stage56
{
	int gridRows;
	int gridCols;
	Grid** grids;
	double k;
	int index;
	std::string ind_ori;
	double xi;
	double zi;

	stage56() :gridRows(0), gridCols(0), grids(0), k(0.0),xi(0.0),zi(0.0)
	{};
};

class grid_pos03
{
private:

	double calc_d_k(std::vector<vector<double>> lines);
	double get_d_k(Grid**& cgrids, int gRows, int gCols);
	std::vector<vector<list<int>>> grid_params(void);
	double weighted_avg(const std::vector<vector<double>> &center);
	cqueue<stage56> fifo;

public:
	stage56 Execute(stage45 s45);
	stage56 getNext()
	{
		stage56 s56;
		fifo.pop(s56);
		return s56;
	}
};

