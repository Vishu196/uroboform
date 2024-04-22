#include "raw_edges.h"

using namespace cv;
using namespace std;

std::ostream& operator<<(std::ostream& ostr, const stage12& s12)
{
	ostr << "main_d_0: " << s12.main_d_0 << endl;
	ostr << "main_d_1: " << s12.main_d_1 << endl;
	ostr << "Stage 1 complete." << endl;
	return ostr;
}

Mat raw_edges::ImageSliceR(const Mat &image, int n)
{
	Mat newImg;
	resize(image, newImg, Size(), 1.0 / n, 1.0 / n, cv::INTER_NEAREST);
	return newImg;
}

double raw_edges::Calc_main_d(const vector<double> &mean0)
{
	if (mean0.size() < Freq_Range)
	{
		cout << "Mean0 size not sufficient in Calc_main_d " << endl;
		return -1;
	}

	int interval = 50;
	const auto last_index = (int)mean0.size() - Freq_Range;
	const int n1 = (last_index / interval) + 1;
	
	vector<double> t1;
	t1.reserve(n1);

	for (int i = 0; i < last_index; i += interval)
	{
		struct MFreq m = signal_evaluation::Main_FreqR(mean0, i, Freq_Range);
		t1.push_back(1 / m.f_g);
	}
	return Evaluation::Median(t1);;
}

void raw_edges::ExecuteR(const Mat &Image)
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
	cout << s12;
}
