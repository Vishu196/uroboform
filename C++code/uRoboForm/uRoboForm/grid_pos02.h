#pragma once
#include "grid_pos01.h"



struct stage45
{
	int gridRows;
	int gridCols;
	Grid** grids;
	int index;
	std::string ind_ori;

	stage45() :gridRows(0), gridCols(0), grids(0), index(0), ind_ori()
	{};
	friend std::ostream& operator<<(std::ostream& ostr, const stage45& s45);
};

struct RdBinary
{
	int index;
	std::string ind_ori;

	RdBinary() : index(0), ind_ori()
	{};
};

class grid_pos02
{
	friend class grid_pos03;
private:

	void checkGrid(Grid** &grids01, int gRows, int gCols);
	std::vector<int> linspace(double start, double end, int num);
	RdBinary ReadBinary(const stage34 &s34);
	int static get_mask_pos(Grid field, int row, int col, size_t i_max);
	cqueue<stage45> fifo;

public:

	void Execute(stage34 s34);
	stage45 getNext()
	{
		stage45 s45;
		fifo.pop(s45);
		return s45;
	}
};

