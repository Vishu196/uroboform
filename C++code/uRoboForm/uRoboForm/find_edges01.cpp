#include "find_edges01.h"




static const int freq_range = 150;

find_edges01::find_edges01(nc::NdArray<int>Image)
{
	auto ImageArray = Image;
}

auto find_edges01:: SliceImage()
{
	return image2 = ImageArray(nc::Slice(0, ImageArray.numRows(), 2), nc::Slice(0, ImageArray.numCols(), 2));
}

auto find_edges01::FindMean0()
{
	return Mean0 = nc::mean(image2, nc::Axis::ROW);
}

auto find_edges01::FindMean1()
{
	return Mean1 = nc::mean(image2, nc::Axis::COL);
}

auto find_edges01::BlackmanWindow(int n, bool sflag)
{
	const double a0 = 0.42;
	const double a1 = 0.5;
	const double a2 = 0.08;
	int wLen = n;
	auto wFun = nc::NdArray<double>(n);

	for (int i = 0; i < n; ++i)
	{
		double wi = 0.0;
		wi = a0 - (a1 * cos((2 * M_PI * i) / wLen)) + (a2 * cos((4 * M_PI * i) / wLen));
		wFun[i] = wi;
	}
	return wFun;
}

auto find_edges01::Main_Freq(nc::NdArray<double>B)
{
	auto Mean = nc::mean(B);
	auto B1 = B - Mean;
	auto wFun = BlackmanWindow(150, true);
	auto image_window = B1 * wFun;

	/*FindFFT(256, image_window, y);
	//auto y1 = nc::abs(y);
	double n_g = Spek_Interpol(nc::abs(y));
	double f_g = n_g/ nc::size(B);
	return n_g;
	*/
	return image_window;
}

auto find_edges01::FindMedian()
{
	return main_d = nc::median(median);
}