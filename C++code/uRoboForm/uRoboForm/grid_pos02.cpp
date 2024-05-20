#include "grid_pos02.h"
#include "Evaluation.h"
#include "signal_evaluation.h"
#include "constants.h"

using namespace std;
using namespace cv;

std::ostream& operator<<(std::ostream& ostr, const stage45& s45)
{
	cout << "Stage 4 complete." << endl;
	return ostr;
}

Mat grid_pos02::cutGrid(const Mat& grid_rot)
{
	int grid_cutRows = 0;
	Mat grid_cut(grid_rot.rows, grid_rot.cols, CV_8U, (int)grid_rot.step);
	uint8_t* grid_cutData = grid_cut.data;

	const int len = grid_rot.rows / 2;
	const int wid = grid_rot.cols / 2;

	Mat grid_rot2(len, wid, CV_8U);
	resize(grid_rot, grid_rot2, Size(), 0.5, 0.5, cv::INTER_NEAREST);

	vector<double> mean_row = Evaluation::Mean1R(grid_rot2);
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
double* gauss_limited(double x, double k, double sigma, double mu, double offset, int max_cut)
{
	return 0;
}

void grid_pos02::subpx_gauss(const vector<double>& B_cut, struct peaks B_max, struct peaks B_min, double d_m, vector<double>& max_pos)
{
	int xmin = 0;
	int xmax = 0;

	for (int i_b = 0; i_b < B_max.index.size(); ++i_b)
	{
		int mid = B_max.index[i_b];
		int b_min_size = B_min.index.size();
		if (b_min_size >= 2)
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

void grid_pos02::subpx_parabel(const vector<double>& B_cut, struct peaks B_max, struct peaks B_min, double d_m, vector<double>& max_pos)
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
					xmin = int(i_b - d_m / 4) + 3;
			}
			for (int c = B_min.index.size() - 1; c >= 0; c--)
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
					vector<int> x(x_size);
					iota(x.begin(), x.end(), xmin);

					Mat W0 = Mat::eye(x_size, x_size, CV_64F);
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
					max_pos.emplace_back(-a_dach0.at<double>(1, 0) / a_dach0.at<double>(2, 0) / 2);
				}
			}
			catch (const exception)
			{
				continue;
			}
		}
	}
}

void grid_pos02::subpx_phase(const Mat& cutGrid, vector<double>& max_pos)
{
	vector<double> B0 = Evaluation::Mean0R(cutGrid);
	double B = Evaluation::MeanR(B0);
	int y = cutGrid.cols;
	if (y >= 60)
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

		double A1 = (real(F_k)) * (real(F_k));
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
		if (d < d_mean / 2)
			d += d_mean;

		for (int i_max = 0; i_max < (m.n_g - 1); ++i_max)
			max_pos.emplace_back(d + i_max * d_mean);
	}
	else
		max_pos.clear();
}

void grid_pos02::subpx_max_pos(const Mat& cutGrid, string mode, vector<double>& max_pos)
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

		double filt = (double)y / 10;
		vector<double> B_cut = signal_evaluation::Bandfilter(Evaluation::Mean0R(cutGrid), 0, (int)filt);
		double d_min = stripe_width / (double)1000 * 2 / px_size0 / 2;
		double prom = ((*std::max_element(B_cut.begin(), B_cut.begin() + y)) - (*std::min_element(B_cut.begin(), B_cut.begin() + y))) * 0.2;

		vector<double> B_cut_N(y);
		transform(B_cut.begin(), B_cut.end(), B_cut_N.begin(), std::negate<double>());

		peaks B_max = Evaluation::Find_Peaks(B_cut, 0.0, d_min, prom);
		peaks B_min = Evaluation::Find_Peaks(B_cut_N, 0.0, d_min, 0.0);

		if ((B_max.index.size() >= 1) && (B_min.index.size() >= 1))
		{
			if ((B_min.index[0] > B_max.index[0]) && (B_max.index[0] >= 0.9 * d_min))
				B_min.index.insert(B_min.index.begin(), 0);
			if ((B_min.index[B_min.index.size() - 1] < B_max.index[B_max.index.size() - 1]) && (y - B_max.index[B_max.index.size() - 1] >= 0.9 * d_min))
				B_min.index.insert(B_min.index.end(), (y - 1));
			if ((B_max.index[0] - B_min.index[0] > 0) && (B_max.index[0] - B_min.index[0] < 0.8 * d_min))
				B_min.index.erase(B_min.index.begin());

			double d_m = Evaluation::MeanR(Evaluation::decumulate(B_max.index));
			//to do
			if (mode == "gauss")
			{
				subpx_gauss(B_cut, B_max, B_min, d_m, max_pos);
			}

			else if (mode == "parabel")
			{
				subpx_parabel(B_cut, B_max, B_min, d_m, max_pos);
			}
		}
	}
	//return p;
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

void grid_pos02::Execute(stage34 s34)
{
	stage45 s45;
	if (s34.edges_sufficient)
	{
		s45.img = s34.img;
		s45.gridRows = s34.gridRows;
		s45.gridCols = s34.gridCols;
		s45.grids = s34.grids;
		s45.edges_sufficient = s34.edges_sufficient;

		string mode = "parabel";
		const int image_size = s45.img.cols * s45.img.rows;
		const double five_percent = image_size * 0.05;

		for (int row = 0; row < (s45.gridRows); ++row)
		{
			for (int col = 0; col < (s45.gridCols); ++col)
			{
				vector<double> max_pos;
				max_pos.reserve(15);
				const int grid_rot_size = s45.grids[row][col].image.rows * s45.grids[row][col].image.cols;
				if ((grid_rot_size >= five_percent) || (s45.grids[row][col].is_hor == false && row == 0 && col == 1) || (s45.grids[row][col].is_hor == true && row == 1 && col == 0))
				{
					Mat grid_cut = cutGrid(s45.grids[row][col].image);
					subpx_max_pos(grid_cut, mode, max_pos);
					modify_max_pos(max_pos);
					s45.grids[row][col].addmaxPos(max_pos);
				}

				for(const auto& vi: s45.grids[row][col].get_max_pos())
					cout << vi << endl;
			}
		}
#ifdef WITH_DEBUGGING
		cout << s45;
#endif
	}

	else
		s45 = {};

	fifo.push(s45);
}