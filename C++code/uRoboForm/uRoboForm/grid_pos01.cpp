#include "grid_pos01.h"

static struct stage32 s32;
static struct stage34 s34;

grid_pos01::grid_pos01(struct stage23 s23)
{
	std::copy(s23.cut_hor.begin(), s23.cut_hor.end(), std::back_inserter(s32.cut_hor));
	std::copy(s23.cut_ver.begin(), s23.cut_ver.end(), std::back_inserter(s32.cut_ver));

	s32.img = s23.img.clone();
	s32.img2 = s23.img2.clone();

}

vector<double> grid_pos01::gradient(const vector<double> &x)
{
	size_t x_size = x.size();
	int dx = 1;
	vector<double> grad(x_size);

	grad[0] = (x[1] - x[0]) / dx;

	for (int i = 1; i <= (x_size-2); i++)
	{
		int a = i + 1;
		int b = i - 1;
		grad[i] = (x[a] - x[b]) / (2 * dx);  // for i in [1,N-2]

	}
	size_t n = x_size - 1;
	size_t m = x_size - 2;
	grad[n] = (x[n] - x[m]) / dx;

	return grad;

}

Mat grid_pos01::cutGrid(const Mat &grid_rot)
{
	int grid_cutRows = 0;
	Mat grid_cut(grid_rot.rows, grid_rot.cols, CV_8U, (int)grid_rot.step);
	uint8_t* grid_cutData = grid_cut.data;

	int len = grid_rot.rows / 2;
	int wid = grid_rot.cols / 2;

	Mat grid_rot2(len, wid, CV_8U, (int)grid_rot.step/2);
	uint8_t* grid_rot2Data = grid_rot2.data;

	int s = 0;
	int t = 0;
	
		for (int i = 0; i < grid_rot.rows; i += 2)
		{
			for (int j = 0; j < (grid_rot.cols - 1); j += 2)
			{
				int val = grid_rot.data[i * grid_rot.step + j];
				size_t idx = (s * grid_rot2.step + t );
				*(grid_rot2Data + idx) = val;

				if (t < wid - 1)
					t++;
			}
			if (s < len - 1)
				s++;
			t = 0;
		}
	
	vector<double> mean_row(len);

	mean_row = Evaluation::Mean1R(grid_rot2);
	double im_mean = Evaluation::MeanR(mean_row);

	double max_val, min_val;
	minMaxLoc(grid_rot2, &min_val, &max_val);

	int val_range = (int)(max_val - min_val);

	std::vector<int> where_out;
	for (int i = 0; i < len; i++) {
		if (((mean_row[i] < im_mean - 0.075 * val_range) || (mean_row[i] >= im_mean + 0.075 * val_range)) && (i < len / 4 || i >= len * 3 / 4)) 
		{
			where_out.push_back(i);
		}
	}

	if (where_out.size() >= 1) 
	{
		where_out.insert(where_out.begin(), 0);
		where_out.push_back(len - 1);

		vector <int> where_arg1 = Evaluation::decumulateInt(where_out);
		int where_arg = max_element(where_arg1.begin(), where_arg1.end()) - where_arg1.begin();
		int x11 = where_out[where_arg] * 2;
		int x20 = where_arg + 1;
		int x22 = where_out[x20] * 2;
		int p = 0;
		int q = 0;
		grid_cutRows = x22 - x11;
		for (int i = x11; i < x22; i++)
		{
			for (int j = 0; j < grid_rot.cols; j++)
			{
				int val = grid_rot.data[i * grid_rot.step + j];
				size_t idx = (p * grid_cut.step + q);
				*(grid_cutData + idx) = val;
				//grid_cut.data[p* grid_cut.step + q] = grid_rot.data[i * grid_rot.step + j];
				if (q < grid_rot.cols - 1)
					q++;
			}
			if (p < grid_rot.rows - 1)
				p++;
			q = 0;
		}
		grid_cut.resize(grid_cutRows);
	}

	else
	{
		grid_cut = grid_rot.clone();
	}

	return grid_cut;
}

struct FP grid_pos01::Find_Peaks(const vector<double>& arr, double dist, double prom)
{
	size_t n = arr.size();
	double maxVal = *max_element(arr.begin(), arr.end());
	vector<int> stripes(n);
	vector<double> s_dic(n);
	int a = 0;
	int count = 0;

	for (int i = 1; i < n - 1; i++)
	{
		int e = i - 1;
		int f = i + 1;
		if (arr[i] >= arr[e] && arr[i] >= arr[f])
		{
				stripes[a] = i;
				s_dic[a] = arr[i];
				a++;
				if (a > 0)
				{
					int a1 = a - 1;
					int a2 = a + 1;
					if (stripes[a] - stripes[a1] < dist)
					{
						stripes[a] = stripes[a2];
						s_dic[a] = s_dic[a2];
						count++;						
					}
				}
			
		}
	}
	
	if (prom > 0.0)
	{
		for (int i = 0; i < count; i++)
		{
			if ((maxVal - s_dic[i]) > prom)
			{
				stripes.erase(stripes.begin() + i);
				s_dic.erase(s_dic.begin() + i);
				count--;
			}

		}
	}

	else
	{
		for (int i = 0; i < count; i++)
		{
			if ((maxVal - s_dic[i]) < prom)
			{
				stripes.erase(stripes.begin() + i);
				s_dic.erase(s_dic.begin() + i);
				count--;
			}

		}
	}
	stripes.resize(count);
	s_dic.resize(count);

	struct FP peaks;
	peaks.stripes = stripes;
	peaks.s_dic = s_dic;
	
	return peaks;
}

struct MFreq grid_pos01::Main_FreqR(const vector<double> &B0, int start, int stop)
{
	double f_g = 0.0;
	const int size = stop - start;

	vector<double> B (size);

	vector<double> image_window (size);

	for (int k = 0; k < size; k++)
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
		image_window[i] = B1[i] * wFun[i];
	}

	vector<double> y1 = signal_evaluation::FFTR(image_window);

	double n_g = signal_evaluation::Spek_InterpolR(y1);
	uint32_t size_B = size;
	f_g = n_g / size_B;

	struct MFreq mf;
	mf.Image_window = image_window;
	mf.n_g = n_g;
	mf.f_g = f_g;

	return mf;
}

//to do
double* gauss_limited(double x, double k, double sigma, double mu, double offset,  int max_cut)
{
	return 0;
}

struct subPX grid_pos01::subpx_gauss(const vector<double> &B_cut, struct FP B_max, struct FP B_min, double d_m)
{
	struct subPX p;
	p.max_pos;
	p.pres;
	int xmin;
	int xmax;

	for (int i_b = 0; i_b < B_max.stripes.size(); i_b++)
	{
		int mid = B_max.stripes[i_b];

		if (B_min.stripes.size() >=2)
		{
			for (int i_0 = 0; i_0 < B_min.stripes.size(); i_0++)
			{
				if (B_min.stripes[i_0] < mid)
					xmin = i_0;
			}
			for (int i_1 = 0; i_1 <= B_min.stripes.size(); i_1++)
			{
				if (B_min.stripes[B_min.stripes.size() - i_1] > mid)
					xmax = i_1;
			}

			try
			{
				if (xmin!=xmax)
				{
					//to do
					//curve fit function pending
				}
			}
			catch (const runtime_error)
			{
				cout << "Optimal Parameters not found for image at x =' " << mid << endl;
			}
			catch (std::exception&)
			{
				continue;
			}
		}
	}

	
	return p;
}

struct subPX grid_pos01::subpx_parabel(const vector<double> &B_cut, struct FP B_max, struct FP B_min, double d_m)
{
	subPX p;
	p.max_pos;
	int xmin{};
	int xmax{};

	for (int i_b = 0; i_b < B_max.stripes.size(); i_b++)
	{
		int mid = B_max.stripes[i_b];

		if (B_min.stripes.size() >= 2)
		{
			for (int i_0 = 0; i_0 < B_min.stripes.size(); i_0++)
			{
				if (B_min.stripes[i_0] < mid)
					xmin = int(i_b-d_m/4)+3;
			}
			for (int i_1 = 0; i_1 <= B_min.stripes.size(); i_1++)
			{
				if (B_min.stripes[B_min.stripes.size() - i_1] > mid)
					xmax = int(i_b + d_m / 4) + 3;
			}

			try
			{
				if (xmin != xmax)
				{
					int i = 0;
					int x_size = xmax - xmin;
					vector<int> x (x_size);
					for (int p = xmin; p < xmax; p++)
					{
						x[i] = p;
						i++;
					}

					Mat W0 = Mat::eye(x_size,x_size,CV_8U);

					Mat col_x3(x_size,1, CV_8U);
					for (int i = 0; i < x_size; i++)
					{
						col_x3.data[i*col_x3.step+1] = x[i] * x[i];
					}

					Mat Phi(x_size, 3, CV_8U);
					Phi.col(0).setTo(1);
					Phi.col(1).copyTo(x);
					Phi.col(2).copyTo(col_x3);

					Mat PhiT = Phi.t();
				
					Mat B_cut01(x_size, 1, CV_8U);
					for (int i = xmin; i < xmax; i++)
					{
						B_cut01.data[i * B_cut01.step + 1] = B_cut[i];
					}

					Mat a_dach0 = (PhiT * W0 * Phi).inv() * PhiT * W0 * B_cut01;
					p.max_pos.push_back(-a_dach0.at<double>(1) / a_dach0.at<double>(2) / 2);

				}
			}
			catch (const exception)
			{
				continue;
			}
		}
	}

	p.pres = {};

	return p;
}

struct subPX grid_pos01::subpx_phase(const Mat &cutGrid)
{
	struct subPX p;
	p.max_pos;
	p.pres;

	vector<double> B0 = Evaluation::Mean0R(cutGrid);

	double B = Evaluation::MeanR(B0);

	int y = cutGrid.cols;
	if (y >=60)
	{
		struct MFreq m = Main_FreqR(B0, 0, y);
		double d_mean = 1 / m.f_g;

		vector<int> B_arange(y);
		iota(begin(B_arange), end(B_arange), 0);

		vector<complex<double>> exp_fac;
		vector<complex<double>> F_k1(y);
		complex<double> F_k = (0, 0); 

		for (int i = 0; i < y; i++)
		{
			exp_fac[i] = (0, (-2 * M_PI) * (B_arange[i]) / y);
			F_k1[i] = m.Image_window[i] * exp(exp_fac[i] * m.n_g);
			F_k += F_k1[i];
		}

		double Phi = std::arg(F_k);
		double d = Phi / (2 * M_PI * m.f_g);

		double A1 = pow(real(F_k),2);
		double A2 = pow(imag(F_k), 2);
		double A = pow((A1 + A2), 0.5);

		size_t lenImg = m.Image_window.size();
		vector<int> ImgWin_arange(lenImg);
		iota(begin(ImgWin_arange), end(ImgWin_arange), 0);

		vector<double> y1(lenImg);
		vector<double> y_cos(lenImg);
		for (int i = 0; i < lenImg; i++)
		{
			y1[i] = (2 * M_PI * m.f_g * (ImgWin_arange[i])) - Phi;
			y_cos[i] = A * cos(y1[i]);
		}

		if (d < d_mean/2)
		{
			d += d_mean;
		}

		for (int i_max = 0; i_max < (m.n_g-1); i_max++)
		{
			p.max_pos.push_back(d + i_max * d_mean);
		}

	}
	else
	{
		p.max_pos.clear();
	}
	p.pres.clear();

	return p;
}

struct subPX grid_pos01::subpx_max_pos(const Mat& cutGrid, int stripe_width, float px_size, string mode)
{
	int y = cutGrid.cols;
	struct subPX p;
	if (mode == "phase")
	{
		p = subpx_phase(cutGrid);
	}
	else
	{
		vector<double>B;
		B = Evaluation::Mean0R(cutGrid);

		double filt = (double)y/10;

		vector<double> B_cut(y);
		B_cut = signal_evaluation::Bandfilter(B, 0, (int)filt);
		
		double d_min = stripe_width / (double)1000 * 2 / px_size / 2;

		double prom = ((*std::max_element(B_cut.begin(), B_cut.begin() + y)) - (*std::min_element(B_cut.begin(), B_cut.begin() + y)))*0.2;
		
		vector<double> B_cut_N(y);
		for (int i = 0; i < y; i++)
		{
			B_cut_N[i] = B_cut[i] * (-1);
		}

		FP B_max = Find_Peaks(B_cut, d_min, prom);
		FP B_min = Find_Peaks(B_cut_N, d_min, 0.0);

		if ((B_max.stripes.size() >= 1) && (B_min.stripes.size() >= 1))
		{
			if ((B_min.stripes[0]>B_max.stripes[0]) && (B_max.stripes[0] >= 0.9*d_min))
			{
				B_min.stripes.insert(B_min.stripes.begin(),0);
			}
			if ((B_min.stripes[B_min.stripes.size() - 1] < B_max.stripes[B_max.stripes.size() - 1]) && (y - B_max.stripes[B_max.stripes.size() - 1] >= 0.9 * d_min))
			{
				B_min.stripes.insert(B_min.stripes.end(), (y - 1));
			}
			if ((B_max.stripes[0] - B_min.stripes[0] > 0) && (B_max.stripes[0] - B_min.stripes[0] < 0.8*d_min))
			{
				B_min.stripes.erase(B_min.stripes.begin());
			}

			vector<int> d00(B_max.stripes.size());
			d00 = Evaluation::decumulateInt(B_max.stripes);

			vector<double> d0 (d00.size());
						
			for (int i = 0; i < d00.size(); i++)
			{
				d0[i] = (double)d00[i];
			}

			double d_m = Evaluation::MeanR(d0);
			//to do
			if (mode == "gauss")
			{
				int e = 0;
			  p = subpx_gauss(B_cut, B_max,B_min, d_m);
			}

			else if (mode == "parabel")
			{
				int t = 0;
			    p = subpx_parabel(B_cut, B_max, B_min, d_m);
			}
		}
	}

	return p;
}

int grid_pos01::Execute(void) 
{
	string mode = "gauss";
	string orientation;
	Grid** grids = 0;

	if ((s32.cut_ver.size() >= 2) && (s32.cut_hor.size() >= 2))
	{
		if (s32.cut_ver.front() * 2 < 10)
		{
			s32.cut_ver.pop_front();
		}

		if (s32.cut_hor.front() * 2 < 10)
		{
			s32.cut_hor.pop_front();
		}

		s32.cut_hor.push_front(0);
		s32.cut_ver.push_front(0);

		s32.cut_hor.push_back(s32.img.rows / 2);
		s32.cut_ver.push_back(s32.img.cols / 2);

		int* cut_hor_arr = new int[s32.cut_hor.size()]();
		std::copy(s32.cut_hor.begin(), s32.cut_hor.end(), cut_hor_arr);

		int* cut_ver_arr = new int[s32.cut_ver.size()]();
		std::copy(s32.cut_ver.begin(), s32.cut_ver.end(), cut_ver_arr);

		//Mat grids(s32.cut_hor.size(), s32.cut_ver.size(), Grid);
		grids = new Grid* [s32.cut_hor.size()];
		for (int h = 0; h < s32.cut_hor.size(); h++)
		{
			grids[h] = new Grid[s32.cut_ver.size()];
		}

		//memset(grids, 0, sizeof(grids[0][0]) * s32.cut_hor.size() * s32.cut_ver.size());

		int image_size = s32.img.cols * s32.img.rows;
		double five_percent = image_size * 0.05;

		for (int row = 0; row < (s32.cut_hor.size()-1); row++)
		{
			for (int col = 0; col < (s32.cut_ver.size()-1); col++)
			{
				int x1 = cut_hor_arr[row] * 2;
				int x2 = (cut_hor_arr[row + 1]) * 2;
				int y1 = cut_ver_arr[col] * 2;
				int y2 = (cut_ver_arr[col + 1]) * 2;
				int s1 = x2 - x1;
				int s2 = y2 - y1;

				Mat grid0(s1, s2, CV_8U, (int)s32.img.step);
				uint8_t* grid0Data = grid0.data;

				if (x2>x1 && y2>y1)
				{
					for (int x = x1; x < x2; x++)
					{
						for (int y = y1; y < y2; y++)
						{
							int o = x - x1;
							int p = y - y1;
							
							int val = s32.img.data[x * s32.img.step + y];
							size_t idx = (o * grid0.step + p);
							*(grid0Data + idx) = val;
						}
					}
				}				

				int x11 = cut_hor_arr[row];
				int x22 = cut_hor_arr[row + 1];
				int y11 = cut_ver_arr[col];
				int y22 = cut_ver_arr[col + 1];
				int w1 = x22 - x11;
				int w2 = y22 - y11;

				Mat mean2(w1, w2, CV_8U, (int)s32.img2.step);
				uint8_t* mean2Data = mean2.data;

				for (int x = x11; x < x22; x++)
				{
					for (int y = y11; y < y22; y++)
					{
						int o = x - x11;
						int p = y - y11;

						int val = s32.img2.data[x * s32.img2.step + y];
						size_t idx = (o * mean2.step + p);
						*(mean2Data + idx) = val;
						//mean2.data[o * mean2.step + p] = s32.img2.data[x * s32.img2.step + y];
					}
				}

				vector <double> mean2_0(w2);
				vector <double> mean2_1(w1);
				
				mean2_0 = Evaluation::Mean0R(mean2);
				mean2_1 = Evaluation::Mean1R(mean2);

				int wid1 = w2 / 6;
				int wid2 = w1 / 6;

				vector<double> mean_grad000(w2);
				mean_grad000 = signal_evaluation::Bandfilter(mean2_0, 0, wid1);
				vector<double> mean_grad111(w1);
				mean_grad111 = signal_evaluation::Bandfilter(mean2_1, 0, wid2);
				
				vector<double> mean_grad00(w2);
				mean_grad00 = gradient(mean_grad000);
				vector<double> mean_grad11(w1);
				mean_grad11 = gradient(mean_grad111);

				vector<double> mean_grad0(w2);

				for (int i = 0; i < w2; i++)
				{
					mean_grad0[i] = abs(mean_grad00[i]);
				}

				vector<double> mean_grad1(w1);

				for (int i = 0; i < w1; i++)
				{
					mean_grad1[i] = abs(mean_grad11[i]);
				}

				double mean_0grad0 = Evaluation::MeanR(mean_grad0);
				double mean_1grad1 = Evaluation::MeanR(mean_grad1);

				int gridrot_row = 0;
				int gridrot_col = 0;
				Mat grid_rot(gridrot_row, gridrot_col, CV_8U);
				
				if (mean_1grad1 > mean_0grad0)
				{
					orientation = "hor";
				}
				else
				{
					orientation = "ver";
				}

				if (orientation == "hor")
				{
					gridrot_row = s2;
					gridrot_col = s1;

					grid_rot = grid0.t();
				}

				else
				{					
					gridrot_row = s1;
					gridrot_col = s2;

					grid_rot = grid0;
				}

				struct subPX p;
				int grid_rot_size = s1 * s2;
				if ((grid_rot_size >= five_percent) || (orientation == "ver" && row == 0 && col == 1) || (orientation == "hor" && row == 1 && col == 0))
				{
					int gridcut_row = 0;
					int gridcut_col = 0;
					Mat grid_cut(gridcut_row, gridcut_row, CV_8U,(int) grid_rot.step);

					if (orientation == "hor")
					{
						gridcut_row = s2;
						gridcut_col = s1;
					
						grid_cut = cutGrid(grid_rot);
					}
					else
					{
						gridcut_row = s1;
						gridcut_col = s2;
					
						grid_cut = cutGrid(grid_rot);
					}

					p = subpx_max_pos(grid_cut, stripe_width, px_size/1000, mode);

					size_t r = p.max_pos.size();
					
					vector <double> max_pos_de = Evaluation::decumulateDouble(p.max_pos);
					
					if ((r>1) && (max_pos_de[r]>65))
					{
						p.max_pos.pop_back();
					}

					if ((r > 1) && (max_pos_de[0] > 65))
					{
						p.max_pos.erase(p.max_pos.begin());
					}
				}
				else
				{
					p.max_pos.clear();
					p.pres.clear();
				}

				vector<int> coord(2);
				coord[0]=(cut_hor_arr[row] * 2);
				coord[1]=(cut_ver_arr[col] * 2);
				grids[row][col] = Grid(grid_rot, orientation, coord, p.max_pos);
			}
		}
		
		s34.img = s32.img;

		s34.gridRows = (int)s32.cut_hor.size();
		s34.gridCols = (int)s32.cut_ver.size();

		for (int i = 0; i < s32.cut_hor.size(); i++)
		{
			memcpy(s34.grids[i], grids[i], (s32.cut_ver.size() * sizeof(Grid)));
		}
	}
	
	else
	{
		grids = {};
	}

	return 0;
}
