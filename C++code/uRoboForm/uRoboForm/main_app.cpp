// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <numeric>
#include <vector>
#include <NumCpp/Core/Slice.hpp>
#include "NumCpp.hpp"

#include <cstdlib>
#include <iostream>
#include <chrono>
#include <thread>
//#define Debug
using namespace cv;
using namespace std;
//using namespace xt;


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



auto Image2Array(Mat mat)
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
	auto array2D = nc::NdArray<int>((int)mat.rows, (int)mat.cols);
	uint8_t* myData = mat.data;
	for (int h = 0; h < mat.rows; h++)
	{
		for (int w = 0; w < mat.cols; w++)
		{
			array2D(h,w) = myData[h * mat.step + w];
		}
	}
	return array2D;
	/*uint8_t* myData = mat.data;
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
	return array2D;*/
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

int main(int argc, char** argv)
{
	// Read the image file
	Mat image = imread("D:/Vaishnavi/C++Trial/Images/001.bmp",IMREAD_GRAYSCALE);
	//imshow("displayimage", image);
	//waitKey(0);

	using std::chrono::high_resolution_clock;
	using std::chrono::duration_cast;
	using std::chrono::duration;
	using std::chrono::milliseconds;

	auto t1 = high_resolution_clock::now();

	
	auto ImageArray = Image2Array(image);
	//cout << "ImageArray: " << ImageArray << endl;
	auto image2 = ImageArray(nc::Slice(0,ImageArray.numRows(), 2), nc::Slice(0,ImageArray.numCols(),2));
	
	auto Mean0Arr = nc::mean(image2, nc::Axis::ROW);
	auto Mean1Arr = nc::mean(image2, nc::Axis::COL);
	//cout << "Mean0Arr: " << Mean0Arr << endl;
	//cout << "\n\n\n";
	//cout << "Mean1Arr: " << Mean1Arr << endl;
	//float* Mean0Arr = Mean0(ArrayRows, ArrayCols, ImageArray);
	//float* Mean1Arr = Mean1(ArrayRows, ArrayCols, ImageArray);

	auto t2 = high_resolution_clock::now();
	/* Getting number of milliseconds as an integer. */
	auto ms_int = duration_cast<milliseconds>(t2 - t1);

	/* Getting number of milliseconds as a double. */
	duration<double, std::milli> ms_double = t2 - t1;

	std::cout << ms_int.count() << "ms\n";
	std::cout << ms_double.count() << "ms\n";
	for (int h = 0; h < image2.numRows(); h++)
	{
		for (int w = 0; w < image2.numCols(); w++)
		{
			//cout << image2(h, w) << ",";

		}
		//printf("\n \n \n");
	}
	

	// Converting the array of pixels to new image and displaying the image:
	/*Mat NewImage = Array2Image(image.rows, image.cols, image.step, ImageArray);
	//imwrite("D:/Vaishnavi/C++ Trial/Images/NewImage.bmp", NewImage);
	//imshow("image2",NewImage);
	//waitKey(0);
	*/
#ifdef Debug
	int rows = image2.numRows();
	int cols = image2.numCols();
	cout << "No. of rows: " << rows << endl;
	cout << "No. of cols: " << cols << endl;

	int rows2 = image2.rows;
	int cols2 = image2.cols;
	cout << "No. of rows: " << rows2 << endl;
	cout << "No. of cols: " << cols2 << endl;

	printf("mean0: \n");
	for (int w = 0; w < image2.cols; w++)
	{
		printf("%f,", Mean0Arr[w]);
	}
	printf("\n");

	printf("mean1: \n");
	for (int h = 0; h < image2.rows; h++)
	{
		printf("%f,", Mean1Arr[h]);
	}
	printf("\n");
	return 0;
#endif
	return 0;
}