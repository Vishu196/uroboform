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
	peaks.index.reserve(15);
	peaks.value.reserve(15);

	if (height != 0.0)
	{
		for (int i = 1; i < arr.size() - 1; ++i)
		{
			if ((arr[i] > arr[i - 1] && arr[i] > arr[i + 1]) && (arr[i] > height))
			{
				peaks.index.push_back(i);
				peaks.value.push_back(arr[i]);

			}
		}
	}
	else
	{
		for (int i = 1; i < arr.size() - 1; ++i)
		{
			if (arr[i] > arr[i - 1] && arr[i] > arr[i + 1])
			{
				peaks.index.push_back(i);
				peaks.value.push_back(arr[i]);
			}
		}
	}

	bool changed = true;
	while (changed)
	{
		changed = false;
		for (int i = 0; i < peaks.index.size() - 1; ++i)
		{
			if (peaks.index[i + 1] - peaks.index[i] < dist)
			{
				if (peaks.value[i] > peaks.value[i + 1])
				{
					peaks.index.erase(peaks.index.begin() + i + 1);
					peaks.value.erase(peaks.value.begin() + i + 1);
					changed = true;
					break;
				}
				else
				{
					peaks.index.erase(peaks.index.begin() + i);
					peaks.value.erase(peaks.value.begin() + i);
					changed = true;
					break;
				}
			}
		}
	}

	if (prom != -1.0)
	{
		vector<double> peaksProminence(50);
		for (int i = 0; i < peaks.index.size(); ++i)
		{
			int peakIndex = peaks.index[i];

			int leftBaseIndex = peakIndex;
			int greater_left_index = peakIndex;
			if(i>0)
			{
				for (int k = 1; k < peaks.index.size(); k++)
				{
					if ((i - k) > 0)
					{
						if (peaks.value[i] < peaks.value[i - k])
						{
							greater_left_index = peaks.index[i - k];
							leftBaseIndex = peaks.index[i - k + 1];
							break;
						}
					}
					else
						greater_left_index = 0;
				}				
			}

			if (i==0 || greater_left_index == 0)
			{
				auto it = std::min_element(std::begin(arr), std::begin(arr)+ leftBaseIndex);
				leftBaseIndex = std::distance(std::begin(arr), it);
			}
			else
			{
				while (leftBaseIndex > greater_left_index && arr[leftBaseIndex - 1] < arr[leftBaseIndex])
				{
					--leftBaseIndex;
				}
			}

			int rightBaseIndex = peakIndex;
			int greater_right_index = peakIndex;

			if (i < (peaks.index.size() - 1))
			{
				for (int k = 1; k < peaks.index.size(); k++)
				{
					if ((i + k) < peaks.index.size())
					{
						if (peaks.value[i+k] > peaks.value[i])
						{
							greater_right_index = peaks.index[i + k];
							//rightBaseIndex = peaks.index[i + k - 1];
							break;
						}
					}
					else
						greater_right_index = 0;
				}
			}

			if (i == (peaks.index.size() - 1) || greater_right_index == 0)
			{
				auto it = std::min_element(std::begin(arr) + rightBaseIndex, std::end(arr));
				rightBaseIndex = std::distance(std::begin(arr), it);
			}
			else
			{
				while (rightBaseIndex < greater_right_index && arr[rightBaseIndex + 1] < arr[rightBaseIndex])
				{
					++rightBaseIndex;
				}
			}
			

			double leftBaseValue = arr[leftBaseIndex];
			double rightBaseValue = arr[rightBaseIndex];

			// Calculate prominence as the difference between peak value and the maximum of left and right bases
			double peakValue = arr[peakIndex];
			peaksProminence[i] = peakValue - std::max(leftBaseValue, rightBaseValue);
			if (peaksProminence[i] < prom)
			{
				peaks.value.erase(peaks.value.begin() + i);
				peaks.index.erase(peaks.index.begin() + i);
			}
		}
	}

	return peaks;
}