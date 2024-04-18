#include "Evaluation.h"
#include <algorithm>

using namespace std;
using namespace cv;

vector<double> Evaluation::Mean0R(const Mat &image2)
{
	vector<double> Mean0Arr(image2.cols);

	double avg = 0.0;
	int sum = 0;
	int x = 0;
	for (int w = 0; w < image2.cols; w++)
	{
		sum = 0;
		for (int h = 0; h < image2.rows; h++)
		{
			x = image2.data[h * image2.step + w];
			sum += x;
		}
		avg = (double)sum / (double)image2.rows;
		Mean0Arr[w] = avg;
	}

	return Mean0Arr;
}

vector<double> Evaluation::Mean1R(const Mat& image2)
{
	vector<double> Mean1Arr(image2.rows);

	double avg = 0.0;
	double sum = 0.0;
	int x = 0;
	for (int h = 0; h < image2.rows; h++)
	{
		sum = 0;
		for (int w = 0; w < image2.cols; w++)
		{
			x = image2.data[h * image2.step + w];
			sum += (double)x;
		}
		avg = sum / (double)image2.cols;
		Mean1Arr[h] = avg;
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
	for (int i = 0; i < size; i++)
	{
		for (int j = i; j < (size - 1); j++)
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
	double sum = 0.0, standardDeviation = 0.0;
	int i;
	int size = stop - start;

	vector<double> B(size);

	for (auto i = 0; i < size; ++i)
	{
		const int w = i + start;
		sum += arr[w];
	}

	const double mean = sum / size;

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

	for (int i = 0; i < s_dic.size(); i++)
	{
		for (int j = 0; j < (s_dic.size()); j++)
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
