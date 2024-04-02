#include "Evaluation.h"


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

double Evaluation::MeanR(const vector<double>& mean0)
{
	double sum = 0.0;
	double meanR = 0.0;
	for (int i = 0; i < mean0.size(); i++)
	{
		sum += mean0[i];
	}

	meanR = sum / mean0.size();
	return meanR;
}

double Evaluation::Median(vector<double> array)
{
	int size = (int)array.size();
	for (int i = 0; i < size; i++)
	{
		for (int j = i; j < (size - 1); j++)
		{
			const int w = j + 1;
			if (array[i] > array[w])
			{
				swap(array[i], array[w]);
				/*double temp = array[i];
				array[i] = array[w];
				array[w] = temp;*/
			}
		}
	}

	int a = size - 1;
	if (size % 2 != 0)
		return (double)array[size / 2];
	else
		return (double)(array[a / 2] + array[size / 2]) / 2.0;

}

double Evaluation::std_dev(const vector<double>& arr, int start, int stop)
{
	double sum = 0.0, mean, standardDeviation = 0.0;
	int i;
	int size = stop - start;

	vector<double> B(size);

	for (int k = 0; k < size; k++)
	{
		const int w = k + start;
		B[k] = arr[w];
	}

	for (i = 0; i < size; ++i)
	{
		sum += B[i];
	}

	mean = sum / size;

	for (i = 0; i < size; ++i)
	{
		standardDeviation += pow(B[i] - mean, 2);
	}

	return sqrt(standardDeviation / size);
}

vector<int> Evaluation::decumulateInt(const vector<int>& x)
{
	const size_t n = x.size() - 1;
	vector<int> xi(n);
	vector<int> x1(n);
	vector<int> x2(n);

	for (size_t i = 0; i < n; i++)
	{
		x1[i] = x[i + 1];

	}
	for (int i = 0; i < n; i++)
	{
		x2[i] = x[i];
	}

	for (int i = 0; i < n; i++)
	{
		xi[i] = x1[i] - x2[i];
	}

	/*x1.clear();
	x2.clear();
	x1.shrink_to_fit();
	x2.shrink_to_fit();
	xi.shrink_to_fit();*/

	return xi;
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
	/*sorted_arr.clear();
	sorted_arr.shrink_to_fit();*/
	return indice_arr;
}
