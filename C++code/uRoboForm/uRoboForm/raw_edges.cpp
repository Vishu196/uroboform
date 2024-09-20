#include "raw_edges.h"
#include "Evaluation.h"
#include "signal_evaluation.h"
#include "constants.h"

/*This is the first class for algorithm implementation after sourcing the image
This class takes input of image in MAT format and calculates values of main_d_0, main_d_1, thresold
for further calculation of edges in the next class, so name is given as raw edges*/

using namespace cv;
using namespace std;

//displays the required information on console
std::ostream& operator<<(std::ostream& ostr, const stage12& s12)
{
	ostr << endl;
	ostr << "main_d_0: " << s12.main_d_0 << endl;
	ostr << "main_d_1: " << s12.main_d_1 << endl;
	ostr << "Stage 1 complete." << endl;
	return ostr;
}

//returns the Mat object by resizing the input Mat with a fcator of n
Mat raw_edges::ImageSliceR(const Mat &image, int n)
{
	Mat newImg;
	resize(image, newImg, Size(), 1.0 / n, 1.0 / n, cv::INTER_NEAREST);
	return newImg;
}

//calculates the main_freq vector on input vector and then calculates the median
double raw_edges::Calc_main_d(const vector<double> &mean0)
{
	/*if (mean0.size() < Freq_Range)
	{
		cout << "Mean0 size not sufficient in Calc_main_d " << endl;
		return -1;
	}*/

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

/*This is the main execute function of this class which is called for each image
for every input image in Mat format, it is first sliced by factor of 2, then mean is calculated 
for both axis and values of main_d_0, main_d_1 and th_edge are obtained*/
void raw_edges::ExecuteR(const Mat &Image)
{
	stage12 s12;

	s12.img = Image;
	s12.img2 = ImageSliceR(Image, 2);
	
	s12.mean0 = Evaluation::Mean0R(s12.img2);
	s12.main_d_0 = Calc_main_d(s12.mean0);
	
	s12.mean1 = Evaluation::Mean1R(s12.img2);
	s12.main_d_1 = Calc_main_d(s12.mean1);

	s12.th_edge = Evaluation::MeanR(s12.mean0);

	fifo.push(s12);

#ifdef WITH_DEBUGGING
	cout << s12;
#endif
}
