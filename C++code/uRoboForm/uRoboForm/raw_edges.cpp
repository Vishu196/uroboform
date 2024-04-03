#include "raw_edges.h"

static struct stage12 s12;

raw_edges::raw_edges(Mat ImageR)
{
	Mat image = ImageR;
}

Mat raw_edges::ImageSliceR(Mat image, int n)
{
	Mat imgCopy = Mat(image.rows / n, image.cols / n, CV_8U, (int)image.step / n);

	uint8_t* myData = image.data;
	uint8_t* myCopyData = imgCopy.data;
	int width = image.cols;
	int height = image.rows;
	int _stride = (int)image.step / n;//in case cols != strides
	for (int i = 0; i < height; i += n)
	{
		for (int j = 0; j < width; j += n)
		{
			int val = myData[i * image.step + j];
			int idx = (i / n * _stride + j / n);
			*(myCopyData + idx) = val;
		}
	}
	return imgCopy;
}

double raw_edges::Main_FreqR(const vector<double> &B0, int start, int stop)
{
	double f_g = 0.0;
	const int size = stop - start;

	vector<double> B(size);

	vector<double> image_window(size);
	
	for (int(k) = 0; k < size; k++) 
	{
		int w = k + start;
		B[k] = B0[w];
	}
	
	double Mean = Evaluation::MeanR(B);

	vector<double> B1(size);

	for (int i = 0; i < size; i++)
	{
		double x = B[i] - Mean;
		B1[i] = x;
	}

	vector<double> wFun = signal_evaluation::BlackmanWindowR(size);
	
	for (int i = 0; i < size; i++)
	{
		double iw = B1[i] * wFun[i];
		image_window[i] = iw;
	}

	vector<double> y1 = signal_evaluation::FFTR(image_window);

	double n_g = signal_evaluation::Spek_InterpolR(y1);
	uint32_t size_B = size;
	f_g = n_g / size_B;

	return f_g;
}

double raw_edges::Calc_main_d(const vector<double> &mean0, int freq_range)
{
	double main_d;
	const int n1 = (((int)mean0.size() - freq_range) / 50) + 1;
	
	vector<double> t1(n1);
	int j = 0;

	for (int i = 0; i < (mean0.size() - freq_range); i += 50)
	{
		double tmp = Main_FreqR(mean0, i, i + freq_range);
		t1[j] = (1 / tmp);
		j++;
	}

	main_d = Evaluation::Median(t1);

	return main_d;
}

struct stage12 raw_edges::ExecuteR(Mat Image, int freq_range)
{
	Mat Image2 = ImageSliceR(Image, 2);
	
	s12.mean0 = Evaluation::Mean0R(Image2);
	s12.main_d_0 = Calc_main_d(s12.mean0, freq_range);
	
	s12.mean1 = Evaluation::Mean1R(Image2);
	s12.main_d_1 = Calc_main_d(s12.mean1, freq_range);

	s12.th_edge = Evaluation::MeanR(s12.mean0);

	s12.img = Image;
	s12.img2 = Image2;
	
	return s12;
}
