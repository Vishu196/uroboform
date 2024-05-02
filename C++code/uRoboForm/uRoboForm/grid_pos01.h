#pragma once
#include "find_edges.h"
#include "grid.h"
#include "cqueue.h"
#include <vector>
#include <opencv2/opencv.hpp>


struct stage34
{
	cv::Mat img;
	int gridRows;
	int gridCols;
	Grid** grids; 

	stage34() :gridRows(0), gridCols(0), grids(0)
	{}
	friend std::ostream& operator<<(std::ostream& ostr, const stage34& s34);

};


class grid_pos01
{
private:
	
	std::vector<double> gradient(const std::vector<double> &x);
	bool get_mean_grad(stage23& s23, const int row, const int col);
	cv::Mat get_gridrot(stage23& s23, const int row, const int col, bool &is_hor);
	cqueue<stage34> fifo;

public:

	void Execute(struct stage23 s23);
	stage34 getNext()
	{
		stage34 s34;
		fifo.pop(s34);
		return s34;
	}
};

