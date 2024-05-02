#pragma once
#include "grid_pos01.h"
#include "grid.h"
#include "cqueue.h"
#include <vector>
#include <opencv2/opencv.hpp>



struct stage45
{
	cv::Mat img;
	int gridRows;
	int gridCols;
	Grid** grids;
	
	stage45() :gridRows(0), gridCols(0), grids(0)
	{};
	friend std::ostream& operator<<(std::ostream& ostr, const stage45& s45);
};


class grid_pos02
{
	friend class grid_pos03;
private:
	cv::Mat cutGrid(const cv::Mat& grid_rot);
	void subpx_max_pos(const cv::Mat& cutGrid, string mode, vector<double>& max_pos);
	void subpx_gauss(const std::vector<double>& B_cut, struct peaks B_max, struct peaks B_min, double d_m, vector<double>& max_pos);
	void subpx_parabel(const std::vector<double>& B_cut, struct peaks B_max, struct peaks B_min, double d_m, vector<double>& max_pos);
	void subpx_phase(const cv::Mat& cutGrid, vector<double>& max_pos);
	
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

