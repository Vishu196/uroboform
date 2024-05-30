#pragma once
#include "find_edges.h"
#include "grid.h"
#include "cqueue.h"
#include "ImgSource.h"
#include "utility.h"
#include <vector>
#include <opencv2/opencv.hpp>

struct stage34
{
	cv::Mat img;
	int gridRows;
	int gridCols;
	Grid** grids; 
	bool edges_sufficient;
	int last_image;

	stage34() :gridRows(0), gridCols(0), grids(0), edges_sufficient(false), last_image(0)
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
	void Execute(stage23 s23);

public:

	grid_pos01(find_edges& edge)
	{
#ifdef WITH_THREADING
		std::thread t1([&]
		{
#endif
				while (1)
				{
					auto t03 = std::chrono::high_resolution_clock::now();
					const stage23& s23 = edge.getNext();
					
					if (s23.img.data == nullptr)
					{
						fifo.push({});
						return;
					}
					Execute(s23);
#ifdef WITH_DEBUGGING
					utility::display_time(t03, std::chrono::high_resolution_clock::now());
#endif	
				}				
#ifdef WITH_THREADING
		});
		t1.detach();
#endif
	}
	
	stage34 getNext()
	{
		stage34 s34;
		fifo.pop(s34);
		return s34;
	}
};

