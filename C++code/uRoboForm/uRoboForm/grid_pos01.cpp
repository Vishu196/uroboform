#include "grid_pos01.h"
#include"Evaluation.h"
#include "signal_evaluation.h"
#include "constants.h"
#include "utility.h"
#include "debug_logs.h"

using namespace std;
using namespace cv;
using std::chrono::high_resolution_clock;


std::ostream& operator<<(std::ostream& ostr, const stage34& s34)
{
	std::cout << "GridRows: " << s34.gridRows << endl;
	std::cout << "GridCols: " << s34.gridCols << endl;
	std::cout << "Stage 3 complete" << endl;
	return ostr;
}

vector<double> grid_pos01::gradient(const vector<double> &x)
{
	const auto x_size = x.size();
	vector<double> grad(x_size);

	grad[0] = (x[1] - x[0]);

	for (auto i = 1; i <= (x_size-2); ++i)
		grad[i] = (x[i + 1] - x[i - 1]) / 2;  // for i in [1,N-2]

	grad[x_size - 1] = (x[x_size - 1] - x[x_size - 2]);

	return grad;
}

void get_grids(stage23 &s23, stage34 &s34)
{
	if (s23.cut_ver.front() * 2 < 10)
		s23.cut_ver.erase(s23.cut_ver.begin());

	if (s23.cut_hor.front() * 2 < 10)
		s23.cut_hor.erase(s23.cut_hor.begin());

	s23.cut_hor.insert(s23.cut_hor.begin(),0);
	s23.cut_ver.insert(s23.cut_ver.begin(), 0);

	s23.cut_hor.push_back(s23.img.rows / 2);
	s23.cut_ver.push_back(s23.img.cols / 2);

	const int horsize = s23.cut_hor.size() - 1;
	s34.grids = new Grid * [horsize];
	for (int h = 0; h < horsize; ++h)
	{
		s34.grids[h] = new Grid[(int)s23.cut_ver.size()-1];
	}

	//return s34.grids;
}

Mat grid_pos01::get_gridrot(stage23& s23, const int row, const int col, bool &is_hor)
{
	const int x1 = s23.cut_hor[row] * 2;
	const int x2 = (s23.cut_hor[row + 1] * 2);
	const int y1 = s23.cut_ver[col] * 2;
	const int y2 = (s23.cut_ver[col + 1] * 2);
	const int s1 = x2 - x1;
	const int s2 = y2 - y1;

	Mat grid0(s1, s2, CV_8U);
	uint8_t* grid0Data = grid0.data;
	Mat grid00(s1, s23.img.cols, CV_8U);

	if (x2 > x1 && y2 > y1)
	{
		Mat sourceRegion = s23.img.rowRange(x1, x2);
		sourceRegion.copyTo(grid00.rowRange(0, s1));
		Mat sourceRegion1 = grid00.colRange(y1, y2);
		sourceRegion1.copyTo(grid0.colRange(0, s2));
	}

	if (get_mean_grad(s23, row, col))
	{
		is_hor = true;
		return grid0.t();
	}
	else
	{
		is_hor = false;
		return grid0;
	}
}

bool grid_pos01::get_mean_grad(stage23 &s23, const int row, const int col)
{
	const int x11 = s23.cut_hor[row ];
	const int x22 = s23.cut_hor[row + 1];
	const int y11 = s23.cut_ver[col];
	const int y22 = s23.cut_ver[col + 1];
	const int w1 = x22 - x11;
	const int w2 = y22 - y11;

	Mat mean22(w1, s23.img2.cols, CV_8U);
	Mat mean2 (w1, w2, CV_8U, (int)s23.img2.step);
	uint8_t* mean2Data = mean2.data;

	if (x22 > x11 && y22 > y11)
	{
		Mat sourceRegion = s23.img2.rowRange(x11, x22);
		sourceRegion.copyTo(mean22.rowRange(0, w1));
		Mat sourceRegion1 = mean22.colRange(y11, y22);
		sourceRegion1.copyTo(mean2.colRange(0, w2));
	}

	vector<double> mean_grad0 = gradient(signal_evaluation::Bandfilter(Evaluation::Mean0R(mean2), 0, (w2 / 6)));
	vector<double> mean_grad1 = gradient(signal_evaluation::Bandfilter(Evaluation::Mean1R(mean2), 0, (w1 / 6)));

	for (int i = 0; i < w2; ++i)
	{
		mean_grad0[i] = abs(mean_grad0[i]);
	}

	for (int i = 0; i < w1; ++i)
	{
		mean_grad1[i] = abs(mean_grad1[i]);
	}

	double mean_0grad0 = Evaluation::MeanR(mean_grad0);
	double mean_1grad1 = Evaluation::MeanR(mean_grad1);

	if (mean_1grad1 > mean_0grad0)
		return true;
	else
		return false;
}

void grid_pos01::Execute(stage23 s23) 
{
	stage34 s34;
	
	if ((s23.cut_ver.size() >= 2) && (s23.cut_hor.size() >= 2))
	{
		s34.edges_sufficient = true;
		string mode = "parabel";
		bool is_hor;

		get_grids(s23, s34);

		for (int row = 0; row < (s23.cut_hor.size() - 1); ++row)
		{
			for (int col = 0; col < (s23.cut_ver.size() - 1); ++col)
			{
				Mat grid_rot = get_gridrot(s23, row, col, is_hor);

				s34.grids[row][col] = Grid(grid_rot, is_hor);
				s34.grids[row][col].im_loc.push_back(s23.cut_hor[row] * 2);
				s34.grids[row][col].im_loc.push_back(s23.cut_ver[col] * 2);

				/*for (const auto& vi : s34.grids[row][col].im_loc)
					cout << vi << endl;*/
			}
		}

		s34.img = s23.img;
		s34.gridRows = (int)s23.cut_hor.size() - 1;
		s34.gridCols = (int)s23.cut_ver.size() - 1;

#ifdef WITH_DEBUGGING
		std::cout << s34;
#endif
		
	}

	else
	{
		s34.edges_sufficient = false;
		s34 = {};
		std::cout << " Edges insufficient to process" << endl;
	}

	fifo.push(s34);
	
}

