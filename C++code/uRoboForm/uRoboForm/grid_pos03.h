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
	int index;
	double k;
	std::string ind_ori;
	double xi;
	double zi;

	stage56() :gridRows(0), gridCols(0), grids(0),index(0), k(0.0),ind_ori(),xi(0.0),zi(0.0)
	{};
};

class grid_pos03
{
private:

	std::vector<vector<list<int>>> grid_params(void);
	int get_mask_pos(Grid field, int row, int col, size_t i_max);
	double weighted_avg(const std::vector<vector<double>> &center);
	void DisplayResult(const stage56& s56);
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

