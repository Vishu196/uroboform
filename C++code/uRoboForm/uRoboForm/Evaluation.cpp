#include "Evaluation.h"
#include <algorithm>

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
	vector<double> sorted_arr((int)s_dic.size());
	sorted_arr = s_dic;

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
