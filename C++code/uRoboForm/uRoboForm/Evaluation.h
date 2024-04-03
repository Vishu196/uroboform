#pragma once

#include <algorithm>
#include <stdexcept>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class Evaluation
{
public:

	vector<double> static Mean0R(const Mat &image2);
	vector<double> static Mean1R(const Mat &image2);
	double static MeanR(const vector<double> &array);
	double static Median(vector<double> array);
	vector<int> static ArgSort(const vector<double>& s_dic);
	double static std_dev(const vector<double>& arr, int start, int stop);
	vector<int> static decumulateInt(const vector<int>& x);
	vector<double> static decumulateDouble(const vector<double>& x);
	vector<int> static deleteXint(vector<int> &arr, int pos);
};

