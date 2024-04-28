#include "Evaluation.h"
#include <algorithm>
#include <stdexcept>

using namespace std;
using namespace cv;

vector<double> Evaluation::Mean0R(const Mat &image2)
{
	vector<double> Mean0Arr;
	int axis = 0; 
	cv::reduce(image2, Mean0Arr, axis, cv::REDUCE_AVG);

	return Mean0Arr;
}

vector<double> Evaluation::Mean1R(const Mat &image2)
{
	vector<double> Mean1Arr(image2.rows,0.0);
	double sum = 0.0;
	for (int h = 0; h < image2.rows; ++h)
	{
		sum = 0;
		for (int w = 0; w < image2.cols; ++w)
		{
			sum += image2.data[h * image2.step + w];
		}
		Mean1Arr[h] = sum / (double)image2.cols;;
	}
	return Mean1Arr;
}

double Evaluation::Mean(vector<double>::const_iterator start, vector<double>::const_iterator end)
{
	const double sum = std::accumulate(start, end, 0.0);
	return sum / (end - start);
}

double Evaluation::Median(vector<double> array)
{
	size_t size = array.size();
	for (int i = 0; i < size; ++i)
	{
		for (int j = i; j < (size - 1); ++j)
		{
			const int w = j + 1;
			if (array[i] > array[w])
			{
				swap(array[i], array[w]);
			}
		}
	}

	size_t a = size - 1;
	if (size % 2 != 0)
		return (double)array[size / 2];
	else
		return (double)(array[a / 2] + array[size / 2]) / 2.0;

}

double Evaluation::std_dev(const vector<double>& arr, int start, int stop)
{
	double standardDeviation = 0.0;
	int size = stop - start;

	const double mean = Mean(arr.begin() + start, arr.begin() + stop);

	for (auto i = 0; i < size; ++i)
	{
		const double val = arr[i + start] - mean;
		standardDeviation += val * val;
	}

	return sqrt(standardDeviation / size);
}

vector<int> Evaluation::ArgSort(const vector<double>& s_dic)
{
	vector<int> indice_arr((int)s_dic.size());
	vector<double> sorted_arr = s_dic;

	sort(sorted_arr.begin(), sorted_arr.end());

	for (int i = 0; i < s_dic.size(); ++i)
	{
		for (int j = 0; j < (s_dic.size()); ++j)
		{
			if (sorted_arr[i] == s_dic[j])
			{
				indice_arr[i] = j;
				break;
			}
		}

	}

	return indice_arr;
}

peaks Evaluation::Find_Peaks(const vector<double>& arr, double height, double dist, double prom)
{
	peaks peaks;
	peaks.stripes.reserve(15);
	peaks.s_dic.reserve(15);

	if (height != 0.0)
	{
		for (int i = 1; i < arr.size() - 1; ++i)
		{
			if ((arr[i] > arr[i - 1] && arr[i] > arr[i + 1]) && (arr[i] > height))
			{
				peaks.stripes.push_back(i);
				peaks.s_dic.push_back(arr[i]);

			}
		}
	}
	else
	{
		for (int i = 1; i < arr.size() - 1; ++i) 
		{
			if (arr[i] > arr[i - 1] && arr[i] > arr[i + 1]) 
			{
				peaks.stripes.push_back(i);
				peaks.s_dic.push_back(arr[i]);
			}
		}
	}

	bool changed = true;
	while (changed) 
	{
		changed = false;
		for (int i = 0; i < peaks.stripes.size() - 1; ++i) 
		{
			if (peaks.stripes[i + 1] - peaks.stripes[i] < dist) 
			{
				if (arr[peaks.stripes[i]] > arr[peaks.stripes[i + 1]]) 
				{
					peaks.stripes.erase(peaks.stripes.begin() + i + 1);
					peaks.s_dic.erase(peaks.s_dic.begin() + i + 1);
					changed = true;
					break;
				}
				else 
				{
					peaks.stripes.erase(peaks.stripes.begin() + i);
					peaks.s_dic.erase(peaks.s_dic.begin() + i);
					changed = true;
					break;
				}
			}
		}
	}

	if (prom != -1.0)
	{
		vector<double> peaksProminence(50);
		for (int i = 0; i < peaks.stripes.size(); ++i)
		{
			int peakIndex = peaks.stripes[i];

			int leftBaseIndex = peakIndex;
			while (leftBaseIndex > 0 && arr[leftBaseIndex - 1] < arr[leftBaseIndex])
			{
				--leftBaseIndex;
			}

			int rightBaseIndex = peakIndex;
			while (rightBaseIndex < arr.size() - 1 && arr[rightBaseIndex + 1] < arr[rightBaseIndex])
			{
				++rightBaseIndex;
			}

			double leftBaseValue = arr[leftBaseIndex];
			double rightBaseValue = arr[rightBaseIndex];

			// Calculate prominence as the difference between peak value and the maximum of left and right bases
			double peakValue = arr[peakIndex];
			peaksProminence[i] = peakValue - std::max(leftBaseValue, rightBaseValue);
			if (peaksProminence[i] < prom)
			{
				peaks.s_dic.erase(peaks.s_dic.begin() + i);
				peaks.stripes.erase(peaks.stripes.begin() + i);
			}
		}
	}

	return peaks;
}