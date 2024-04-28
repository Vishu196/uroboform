#pragma once
#include "find_edges.h"
#include "grid.h"

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
	cv::Mat cutGrid(const cv::Mat &grid_rot);
	void subpx_max_pos(const cv::Mat &cutGrid, string mode, vector<double>& max_pos);
	void subpx_gauss(const std::vector<double> &B_cut, struct peaks B_max, struct peaks B_min, double d_m, vector<double>& max_pos);
	void subpx_parabel(const std::vector<double> &B_cut, struct peaks B_max, struct peaks B_min, double d_m, vector<double>& max_pos);
	void subpx_phase(const cv::Mat &cutGrid, vector<double>& max_pos);
	bool get_mean_grad(stage23& s23, const int row, const int col);
	cv::Mat get_gridrot(stage23& s23, const int row, const int col, std::string& orientation);
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

