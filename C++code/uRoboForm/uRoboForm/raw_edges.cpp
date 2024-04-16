#include "raw_edges.h"

using namespace cv;
using namespace std;

Mat raw_edges::ImageSliceR(Mat image, int n)
{
	Mat newImg;
	resize(image, newImg, Size(), 1.0 / n, 1.0 / n, cv::INTER_NEAREST);
	return newImg;
}

double raw_edges::Calc_main_d(const vector<double> &mean0)
{
	int interval = 50;
	double main_d;
	const int n1 = (((int)mean0.size() - freq_range) / interval) + 1;
	
	vector<double> t1(n1);
	int j = 0;

	for (int i = 0; i < (mean0.size() - freq_range); i += interval)
	{
		struct MFreq m = signal_evaluation::Main_FreqR(mean0, i, i + freq_range);
		t1[j] = (1 / m.f_g);
		j++;
	}

	main_d = Evaluation::Median(t1);

	return main_d;
}

void raw_edges::DisplayResult(const stage12 &s12)
{
	cout << "main_d_0: " << s12.main_d_0 << endl;
	cout << "main_d_1: " << s12.main_d_1 << endl;
	cout << "Stage 1 complete." << endl;

}

void raw_edges::ExecuteR(Mat Image)
{
	stage12 s12;

	Mat Image2 = ImageSliceR(Image, 2);
	
	s12.mean0 = Evaluation::Mean0R(Image2);
	s12.main_d_0 = Calc_main_d(s12.mean0);
	
	s12.mean1 = Evaluation::Mean1R(Image2);
	s12.main_d_1 = Calc_main_d(s12.mean1);

	s12.th_edge = Evaluation::MeanR(s12.mean0);

	s12.img = Image;
	s12.img2 = Image2;

	fifo.push(s12);
	DisplayResult(s12);
}
