#pragma once
#include "grid_pos01.h"
#include "grid.h"
#include "cqueue.h"
#include "ImgSource.h"
#include "utility.h"
#include <vector>
#include <opencv2/opencv.hpp>
#include <thread>

struct stage45
{
	cv::Mat img;
	int gridRows;
	int gridCols;
	Grid** grids;
	bool edges_sufficient;
	int last_image;
	
	stage45() :gridRows(0), gridCols(0), grids(0), edges_sufficient(false), last_image(0)
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
	void Execute(stage34 s34);


public:

	grid_pos02(grid_pos01& grid1)
	{
#ifdef WITH_THREADING
		std::thread t1([&]
			{
#endif
				while (1)
				{
					auto t04 = std::chrono::high_resolution_clock::now();
					const stage34 &s34 = grid1.getNext();
					if (s34.img.data == nullptr && s34.last_image != (-1))
					{
						fifo.push({});
						return;
					}
					Execute(s34);
#ifdef WITH_DEBUGGING
					utility::display_time(t04, std::chrono::high_resolution_clock::now());
#endif	
				}
#ifdef WITH_THREADING
			});
		t1.detach();
#endif
	}

	stage45 getNext()
	{
		stage45 s45;
		fifo.pop(s45);
		return s45;
	}
};

