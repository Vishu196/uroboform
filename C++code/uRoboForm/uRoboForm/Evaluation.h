#pragma once
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <opencv2/opencv.hpp>

class Evaluation
{
public:

	std::vector<double> static Mean0R(const cv::Mat &image2);
	std::vector<double> static Mean1R(const cv::Mat &image2);
	double static MeanR(const std::vector<double> &array);
	double static Median(std::vector<double> array);
	std::vector<int> static ArgSort(const std::vector<double>& s_dic);
	double static std_dev(const std::vector<double>& arr, int start, int stop);
	std::vector<int> static decumulateInt(const std::vector<int>& x);
	std::vector<double> static decumulateDouble(const std::vector<double>& x);
	std::vector<int> static deleteXint(std::vector<int> &arr, int pos);
	double static IntMeanR(const std::vector<int>& mean0);
};

