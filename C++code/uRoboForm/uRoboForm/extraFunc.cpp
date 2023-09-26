#include "main.h"

float* Mean0(int rows, int cols, int** array)
{
	float* Mean0Arr = 0;
	Mean0Arr = new float[cols];

	float avg = 0.0;
	float sum = 0.0;
	int x = 0;
	for (int w = 0; w < cols; w++)
	{
		sum = 0;
		for (int h = 0; h < rows; h++)
		{
			x = *(*(array + h) + w);
			sum += (float)x;
		}
		avg = sum / (float)rows;
		Mean0Arr[w] = avg;
	}
	return Mean0Arr;
}

float* Mean1(int rows, int cols, int** array)
{
	float* Mean1Arr = 0;
	Mean1Arr = new float[rows];

	float avg = 0.0;
	float sum = 0.0;
	int x = 0;
	for (int h = 0; h < rows; h++)
	{
		sum = 0;
		for (int w = 0; w < cols; w++)
		{
			x = *(*(array + h) + w);
			sum += (float)x;
		}
		avg = sum / (float)cols;
		Mean1Arr[h] = avg;
	}
	return Mean1Arr;
}

int** Image2Array(Mat mat)
{
	/*std::vector<int> array;
	if (mat.isContinuous()) {
		// array.assign(mat.datastart, mat.dataend); // <- has problems for sub-matrix like mat = big_mat.row(i)
		array.assign(mat.data, mat.data + mat.total() * mat.channels());
	}
	else {
		for (int i = 0; i < mat.rows; ++i) {
			array.insert(array.end(), mat.ptr<int>(i), mat.ptr<int>(i) + mat.cols * mat.channels());
		}
	}*/
	
	uint8_t* myData = mat.data;
	int** array2D = 0;
	array2D = new int* [mat.rows];
	for (int h = 0; h < mat.rows; h++)
	{
		array2D[h] = new int[mat.cols];
		for (int w = 0; w < mat.cols; w++)
		{
			array2D[h][w] = myData[h * mat.step + w];
		}
	}
	return array2D;
}

auto Img2Arr (Mat mat)
{
	auto array2D = nc::NdArray<int>((int)mat.rows, (int)mat.cols);
	uint8_t* myData = mat.data;
	for (int h = 0; h < mat.rows; h++)
	{
		for (int w = 0; w < mat.cols; w++)
		{
			array2D(h, w) = myData[h * mat.step + w];
		}
	}
	return array2D;
}

Mat Array2Image(int r, int c, int stride, int** array)
{
	Mat newImage = Mat(r, c, CV_8U, stride);
	uint8_t* newdata = newImage.data;

	for (int h = 0; h < r; h++)
	{
		for (int w = 0; w < c; w++)
		{
			*(newdata + (h * stride + w)) = *(*(array + h) + w);
		}
	}
	return newImage;
}

Mat ImageSlice(Mat image, int n)
{
	Mat imgCopy = Mat(image.rows / n, image.cols / n, CV_8U, (int)image.step / n);

	uint8_t* myData = image.data;
	uint8_t* myCopyData = imgCopy.data;
	int width = image.cols;
	int height = image.rows;
	int _stride = image.step / n;//in case cols != strides
	for (int i = 0; i < height; i += n)
	{
		cout << endl;
		for (int j = 0; j < width; j += n)
		{
			int val = myData[i * image.step + j];
			int idx = (i / n * _stride + j / n);
			*(myCopyData + idx) = val;
		}
	}
#ifdef Debug
	cout << "new array: ";
	for (int i = 0; i < height / n; i++)
	{
		cout << endl;
		for (int j = 0; j < width / n; j++)
		{
			int val = myCopyData[i * _stride + j];
			cout << val << " ";
		}
	}
#endif
	return imgCopy;
}

int* FindPeaks(double arr[], int n, float th_edge)
{
	int* stripes = 0;
	stripes = new int[12];
	float* s_dic = 0;
	s_dic = new float[12];
	int a = 0;

	for (int i = 1; i < n - 1; i++)
	{
		if (arr[i] >= arr[i - 1] && arr[i] >= arr[i + 1]) {
			if (arr[i] > th_edge) {
				stripes[a] = i;
				s_dic[a] = arr[i];
				a++;
			}
		}
	}
	for (int a = 0; a < 11; a++)
	{
		if (stripes[a + 1] - stripes[a] < 25)
			stripes[a + 1] = stripes[a + 2];
	}

	return stripes;
}


