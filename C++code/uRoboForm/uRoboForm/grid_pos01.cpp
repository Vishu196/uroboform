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

Mat grid_pos01::cutGrid(const Mat &grid_rot)
{
	int grid_cutRows = 0;
	Mat grid_cut(grid_rot.rows, grid_rot.cols, CV_8U, (int)grid_rot.step);
	uint8_t* grid_cutData = grid_cut.data;

	const int len = grid_rot.rows / 2;
	const int wid = grid_rot.cols / 2;

	Mat grid_rot2(len,wid,CV_8U);
	resize(grid_rot, grid_rot2, Size(), 0.5, 0.5, cv::INTER_NEAREST);
	
	vector<double> mean_row =  Evaluation::Mean1R(grid_rot2);
	const double im_mean = Evaluation::MeanR(mean_row);

	double max_val, min_val;
	minMaxLoc(grid_rot2, &min_val, &max_val);

	const int val_range = (int)(max_val - min_val);

	vector<int> where_out;
	where_out.reserve(10);
	for (int i = 0; i < len; ++i)
	{
		if (((mean_row[i] < im_mean - 0.075 * val_range) || (mean_row[i] >= im_mean + 0.075 * val_range)) && (i < len / 4 || i >= len * 3 / 4)) 
			where_out.emplace_back(i);
	}

	if (where_out.size() >= 1) 
	{
		where_out.insert(where_out.begin(), 0);
		where_out.push_back(len - 1);

		vector <int> where_arg1 = Evaluation::decumulate(where_out);
		const int where_arg = max_element(where_arg1.begin(), where_arg1.end()) - where_arg1.begin();
		const int x11 = where_out[where_arg] * 2;
		const int x20 = where_arg + 1;
		const int x22 = where_out[x20] * 2;
		grid_cutRows = x22 - x11;
		
		Mat sourceRegion = grid_rot.rowRange(x11, x22);
		sourceRegion.copyTo(grid_cut.rowRange(0, x22 - x11));
		grid_cut.resize(grid_cutRows);
	}
	else
		grid_cut = grid_rot.clone();

	return grid_cut;
}

//to do
double* gauss_limited(double x, double k, double sigma, double mu, double offset,  int max_cut)
{
	return 0;
}

void grid_pos01::subpx_gauss(const vector<double> &B_cut, struct peaks B_max, struct peaks B_min, double d_m, vector<double>& max_pos)
{
	int xmin = 0;
	int xmax = 0;

	for (int i_b = 0; i_b < B_max.index.size(); ++i_b)
	{
		int mid = B_max.index[i_b];
		int b_min_size = B_min.index.size();
		if (b_min_size >=2)
		{
			for (int i_0 = 0; i_0 < b_min_size; ++i_0)
			{
				if (B_min.index[i_0] < mid)
					xmin = i_0;
				if (B_min.index[b_min_size - i_0] > mid)
					xmax = i_0;
			}
			/*for (int i_1 = 0; i_1 <= B_min.index.size(); i_1++)
			{
				if (B_min.index[B_min.index.size() - i_1] > mid)
					xmax = i_1;
			}*/

			//try
			//{
			//	if (xmin!=xmax)
			//	{
			//		//to do
			//		//curve fit function pending
			//	}
			//}
			//catch (const runtime_error)
			//{
			//	//cout << "Optimal Parameters not found for image at x =' " << mid << endl;
			//}
			//catch (std::exception&)
			//{
			//	continue;
			//}
		}
	}
}

void grid_pos01::subpx_parabel(const vector<double> &B_cut, struct peaks B_max, struct peaks B_min, double d_m, vector<double>& max_pos)
{
	for (int va = 0; va < B_max.index.size(); ++va)
	{
		int i_b = B_max.index[va];
		int mid = i_b;

		if (B_min.index.size() >= 2)
		{
			int xmin{};
			int xmax{};

			for (int vb = 0; vb < B_min.index.size(); ++vb)
			{
				int i_0 = B_min.index[vb];
				if (i_0 < mid)
					xmin = int(i_b - d_m/4)+3;
			}
			for (int c = B_min.index.size()-1; c>=0 ; c--)
			{
				int i_1 = B_min.index[c];
				if (i_1 > mid)
					xmax = int(i_b + d_m / 4);
			}

			try
			{
				if ((xmin != xmax) && (xmin != 0) && (xmax != 0))
				{
					int x_size = xmax - xmin;
					vector<int> x (x_size);
					iota(x.begin(), x.end(), xmin);
					
					Mat W0 = Mat::eye(x_size,x_size, CV_64F);
					Mat Phi(x_size, 3, CV_64F);					
					Phi.col(0).setTo(1);

					for (int i = 0; i < Phi.rows; ++i)
					{
						//Phi.at<double>(i, 0) = 1;
						Phi.at<double>(i, 1) = x[i];
						Phi.at<double>(i, 2) = x[i] * x[i];
					}
					/*cout << "Phi:" << Phi << endl;
					cout << "W0:" << W0 << endl;*/
					Mat PhiT = Phi.t();
					
					int u = 0;
					Mat B_cut01(x_size, 1, CV_64F);
					copy(B_cut.begin() + xmin, B_cut.begin() + xmax, B_cut01.ptr<double>());

					Mat a_dach0(3, 1, CV_64F);
					a_dach0 = (PhiT * W0 * Phi).inv() * PhiT * W0 * B_cut01;
					max_pos.emplace_back(-a_dach0.at<double>(1,0) / a_dach0.at<double>(2,0) / 2);
				}
			}
			catch (const exception)
			{
				continue;
			}
		}
	}
}

void grid_pos01::subpx_phase(const Mat &cutGrid, vector<double>& max_pos)
{
	vector<double> B0 = Evaluation::Mean0R(cutGrid);
	double B = Evaluation::MeanR(B0);
	int y = cutGrid.cols;
	if (y >=60)
	{
		struct MFreq m = signal_evaluation::Main_FreqR(B0, 0, y);
		double d_mean = 1 / m.f_g;

		vector<int> B_arange(y);
		iota(begin(B_arange), end(B_arange), 0);

		vector<complex<double>> exp_fac;
		vector<complex<double>> F_k1(y);
		complex<double> F_k = (0, 0); 

		for (int i = 0; i < y; ++i)
		{
			exp_fac[i] = (0, (-2 * M_PI) * (B_arange[i]) / y);
			F_k1[i] = m.Image_window[i] * exp(exp_fac[i] * m.n_g);
			F_k += F_k1[i];
		}

		double Phi = std::arg(F_k);
		double d = Phi / (2 * M_PI * m.f_g);

		double A1 = (real(F_k))* (real(F_k));
		double A2 = (imag(F_k)) * (imag(F_k));
		double A = sqrt((A1 + A2));

		size_t lenImg = m.Image_window.size();
		vector<int> ImgWin_arange(lenImg);
		iota(begin(ImgWin_arange), end(ImgWin_arange), 0);

		vector<double> y1(lenImg);
		vector<double> y_cos(lenImg);
		for (int i = 0; i < lenImg; ++i)
		{
			y1[i] = (2 * M_PI * m.f_g * (ImgWin_arange[i])) - Phi;
			y_cos[i] = A * cos(y1[i]);
		}
		if (d < d_mean/2)
			d += d_mean;

		for (int i_max = 0; i_max < (m.n_g-1); ++i_max)
			max_pos.emplace_back(d + i_max * d_mean);
	}
	else
		max_pos.clear();
}

void grid_pos01::subpx_max_pos(const Mat& cutGrid, string mode, vector<double>& max_pos)
{
	int y = cutGrid.cols;
	
	double px_size0 = px_size / 1000;
	if (mode == "phase")
	{
		subpx_phase(cutGrid, max_pos);
	}
	else
	{
		//vector<double>B = Evaluation::Mean0R(cutGrid);

		double filt = (double)y/10;
		vector<double> B_cut = signal_evaluation::Bandfilter(Evaluation::Mean0R(cutGrid), 0, (int)filt);		
		double d_min = stripe_width / (double)1000 * 2 / px_size0 / 2;
		double prom = ((*std::max_element(B_cut.begin(), B_cut.begin() + y)) - (*std::min_element(B_cut.begin(), B_cut.begin() + y)))*0.2;
		
		vector<double> B_cut_N(y);
		transform(B_cut.begin(), B_cut.end(), B_cut_N.begin(), std::negate<double>());

		peaks B_max = Evaluation::Find_Peaks(B_cut,0.0, d_min, prom);
		peaks B_min = Evaluation::Find_Peaks(B_cut_N,0.0, d_min, 0.0);

		if ((B_max.index.size() >= 1) && (B_min.index.size() >= 1))
		{
			if ((B_min.index[0]>B_max.index[0]) && (B_max.index[0] >= 0.9*d_min))
				B_min.index.insert(B_min.index.begin(),0);
			if ((B_min.index[B_min.index.size() - 1] < B_max.index[B_max.index.size() - 1]) && (y - B_max.index[B_max.index.size() - 1] >= 0.9 * d_min))
				B_min.index.insert(B_min.index.end(), (y - 1));
			if ((B_max.index[0] - B_min.index[0] > 0) && (B_max.index[0] - B_min.index[0] < 0.8*d_min))
				B_min.index.erase(B_min.index.begin());

			double d_m = Evaluation::MeanR(Evaluation::decumulate(B_max.index));
			//to do
			if (mode == "gauss")
			{
			   subpx_gauss(B_cut, B_max,B_min, d_m, max_pos);
			}

			else if (mode == "parabel")
			{
			    subpx_parabel(B_cut, B_max, B_min, d_m, max_pos);
			}
		}
	}
	//return p;
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

	s34.grids = new Grid * [s23.cut_hor.size()-1];
	for (int h = 0; h < (int)(s23.cut_hor.size()-1); ++h)
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

	bool is_ori_hor = get_mean_grad(s23, row, col);

	if (is_ori_hor)
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

void modify_max_pos(vector<double>& max_pos)
{
	size_t r = max_pos.size();
	vector <double> max_pos_de = Evaluation::decumulate(max_pos);

	if ((r > 1) && (max_pos_de.back() > 65))
		max_pos.pop_back();

	if ((r > 1) && (max_pos_de[0] > 65))
		max_pos.erase(max_pos.begin());
}

void grid_pos01::Execute(stage23 s23) 
{
	stage34 s34;
	string mode = "parabel";
	bool is_hor;
	s34.grids = {};

	get_grids(s23, s34);
	
	const int image_size = s23.img.cols * s23.img.rows;
	const double five_percent = image_size * 0.05;

	for (int row = 0; row < (s23.cut_hor.size()-1); ++row)
	{
		for (int col = 0; col < (s23.cut_ver.size()-1); ++col)
		{
			Mat grid_rot = get_gridrot(s23, row, col, is_hor);
			vector<double> max_pos;
			max_pos.reserve(15);
			const int grid_rot_size = grid_rot.rows * grid_rot.cols;

			if ((grid_rot_size >= five_percent) || (is_hor == false && row == 0 && col == 1) || (is_hor == true && row == 1 && col == 0))
			{
				Mat grid_cut = cutGrid(grid_rot);
				subpx_max_pos(grid_cut, mode, max_pos);
				modify_max_pos(max_pos);
			}

			vector<int>coord = { (s23.cut_hor[row] * 2), (s23.cut_ver[col] * 2) };
			s34.grids[row][col] = Grid(grid_rot, is_hor, coord, max_pos);
			
			/*vector<double> max_p = s34.grids[row][col].max_pos;
			for(auto vi:max_p)
				cout << vi << endl;*/
		}
	}

	s34.img = s23.img;
	s34.gridRows = (int)s23.cut_hor.size()-1;
	s34.gridCols = (int)s23.cut_ver.size()-1;

	fifo.push(s34);

	std::cout << s34;
}

