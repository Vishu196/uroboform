#pragma once
#include "cqueue.h"
#include <vector>
#include <opencv2/opencv.hpp>
#include "constants.h"
#include "ImgSource.h"
#include "utility.h"


struct stage12
{		
	cv::Mat img;
	cv::Mat img2;
	std::vector<double> mean0;
	std::vector<double> mean1;
	double main_d_0;
	double main_d_1;
	double th_edge;

	stage12() :main_d_0(0.0),main_d_1(0.0),th_edge(0)
	{}
	friend std::ostream& operator<<(std::ostream& ostr, const stage12& s12);
};

class raw_edges
{
private:

	cv::Mat ImageSliceR (const cv::Mat &ImageR, int n);
	double Calc_main_d(const std::vector<double> &mean0);
	cqueue<stage12> fifo;
	int Freq_Range;
	void ExecuteR(const cv::Mat& image);

public:

	raw_edges(Source& imgsrc, int _freq_range) : Freq_Range(_freq_range)
	{
#ifdef WITH_THREADING
		std::thread t1([&] {
#endif
			while (1)
			{
				cv::Mat image = imgsrc.getNext();
				if (image.cols == 0)
				{
					std::cout << "Image broken" << std::endl;
					fifo.push({});
					return;
				}
				auto t01 = std::chrono::high_resolution_clock::now();
				ExecuteR(image);
				utility::display_time(t01, std::chrono::high_resolution_clock::now());
			}
#ifdef WITH_THREADING
			});
		t1.detach();
#endif
	}
	
	stage12 getNext() 
	{
		stage12 s12;
		fifo.pop(s12);
		return s12;
	}
};

