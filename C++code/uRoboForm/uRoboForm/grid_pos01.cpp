#include "grid_pos01.h"

static struct stage32 s32;
static struct stage34 s34;

static void buffer_s32_init() 
{
	s32.cut_hor = {};
	s32.cut_ver = {};

	s32.imgRows = 0;
	s32.imgCols = 0;
	s32.cut_hor_s = 0;
	s32.cut_ver_s = 0;
	
	s32.img = new int* [1080];;// new int[1080][1440];
	for (int h = 0; h < 1080; h++)
	{
		s32.img[h] = new int[1440];
	}

	s32.img2 = new int* [540];;// new int[1080][1440];
	for (int h = 0; h < 540; h++)
	{
		s32.img2[h] = new int[720];
	}
}

static void buffer_s34_init()
{
	s34.img = new int* [1080];
	for (int h = 0; h < 1080; h++)
	{
		s34.img[h] = new int[1440];
	}

	/*s34.grids = new Grid*[3];
	for (int i = 0; i < 3; i++)
	{
		s34.grids[i] = new Grid(s32.img, s32.imgRows, orientation, im_loc, Grid::max_pos);
	}*/
}

static void buffers_init(void)
{
	buffer_s32_init();
	buffer_s34_init();
}

grid_pos01::grid_pos01(struct stage23 s23)
{
	buffers_init();

	s32.cut_hor_s = s23.cut_hor_s;
	s32.cut_ver_s = s23.cut_ver_s;

	std::copy(s23.cut_hor.begin(), s23.cut_hor.end(), std::back_inserter(s32.cut_hor));
	std::copy(s23.cut_ver.begin(), s23.cut_ver.end(), std::back_inserter(s32.cut_ver));

	s32.imgRows = s23.imgRows;
	s32.imgCols = s23.imgCols;
	int rows2 = s32.imgRows / 2;
	int cols2 = s32.imgCols / 2;

	for (int i = 0; i < s23.imgRows; i++)
	{
		memcpy(s32.img[i], s23.img[i], (s23.imgCols * sizeof(int)));
	}

	for (int i = 0; i < (rows2); i++)
	{
		memcpy(s32.img2[i], s23.img2[i], (cols2 * sizeof(int)));
	}
}

double* grid_pos01::Mean0R(int rows, int cols, int** array)
{
	double* Mean0Arr = 0;
	Mean0Arr = new double[cols]();

	double avg = 0.0;
	double sum = 0.0;
	int x = 0;
	for (int w = 0; w < cols; w++)
	{
		sum = 0;
		for (int h = 0; h < rows; h++)
		{
			x = *(*(array + h) + w);
			sum += (double)x;
		}
		avg = sum / (double)rows;
		Mean0Arr[w] = avg;
	}
	return Mean0Arr;
}

double* grid_pos01::Mean1R(int rows, int cols, int** array)
{
	double* Mean1Arr = 0;
	Mean1Arr = new double[rows]();

	double avg = 0.0;
	double sum = 0.0;
	int x = 0;
	for (int h = 0; h < rows; h++)
	{
		sum = 0;
		for (int w = 0; w < cols; w++)
		{
			x = *(*(array + h) + w);
			sum += (double)x;
		}
		avg = sum / (double)cols;
		Mean1Arr[h] = avg;
	}
	return Mean1Arr;
}

double grid_pos01::MeanR(int rows, double* mean0)
{
	double sum = 0.0;
	double meanR = 0.0;
	for (int i = 0; i < rows; i++)
	{
		sum += mean0[i];
	}

	meanR = sum / rows;
	return meanR;
}

double* grid_pos01::RFFT(double* x, int x_size)
{
	int N = x_size;
	double* y = new double[N]();
	fftw_plan p;

	p = fftw_plan_r2r_1d(N, x, y, FFTW_R2HC, FFTW_ESTIMATE);//fftw_plan_dft_1d(N, in, y, FFTW_FORWARD, FFTW_ESTIMATE);

	fftw_execute(p);

	fftw_destroy_plan(p);

	double* yy = new double[N]();
	yy[0] = y[0];
	int j = 0;
	for (int i = 1; i < N; i += 2)
	{
		yy[i] = y[i - j];
		j++;
	}
	int k = 1;
	for (int i = 2; i < N + 1; i += 2)
	{
		yy[i] = y[N - k];
		k++;
	}

	delete[] y;
	return yy;
}

double* grid_pos01::IRFFT(double* x, int x_size)
{
	int N = x_size;
	double* y = new double[N]();

	double* xx = new double[N]();
	xx[0] = x[0];
	//xx[1] = x[1];
	for (int i = 1; i < N / 2; i++)
	{
		xx[i] = x[(2 * i) - 1];
	}

	for (int i = N; i > N / 2; i--)
	{
		xx[i - 1] = x[2 * (N - i) + 2];
	}

	fftw_plan p;

	p = fftw_plan_r2r_1d(N, xx, y, FFTW_HC2R, FFTW_ESTIMATE);//fftw_plan_dft_1d(N, in, y, FFTW_FORWARD, FFTW_ESTIMATE);

	fftw_execute(p);

	fftw_destroy_plan(p);
	for (int i = 0; i < N; i++) {
		y[i] /= N;
	}

	delete[] xx;

	return y;
}

double* grid_pos01::Bandfilter(double* x, int x0, int x1, int x_size)
{
	double* f_x = RFFT(x, x_size);

	double* f_x_cut;
	f_x_cut = f_x;

	for (int i = 0; i < x0; i++)
	{
		f_x_cut[i] = 0;
	}

	for (int i = x1; i <= x_size; i++)
	{
		f_x_cut[i] = 0;
	}

	double* x_cut = IRFFT(f_x_cut, x_size);

	return x_cut;
}

double* grid_pos01::gradient(double* x, int x_size)
{
	int dx = 1;
	double* grad = 0;
	grad = new double[x_size]();

	grad[0] = (x[1] - x[0]) / dx;

	for (int i = 1; i <= (x_size-2); i++)
	{
		grad[i] = (x[i + 1] - x[i - 1]) / (2 * dx);  // for i in [1,N-2]

	}
	grad[x_size - 1] = (x[x_size - 1] - x[x_size - 2]) / dx;

	return grad;

}

int* decumulateInt(int* x, int size)
{
	const size_t n = size - 1;
	int* xi = new int[n]();
	int* x1 = new int[n]();
	int* x2 = new int[n]();

	for (size_t i = 0; i < size - 1; i++)
	{
		if (i < n)
		{
			x1[i] = x[i + 1];
		}

	}
	for (int i = 0; i < n; i++)
	{
		x2[i] = x[i];
	}

	for (int i = 0; i < n; i++)
	{
		xi[i] = x1[i] - x2[i];
	}

	delete[] x1;
	delete[] x2;

	return xi;
}

int** grid_pos01::cutGrid(int** grid_rot, int x, int y)
{
	int** grid_cut = 0;
	grid_cut = new int* [x];
	for (int h = 0; h < x; h++)
	{
		grid_cut[h] = new int[y];
	}

	int len = x / 2;
	int wid = y / 2;

	int** grid_rot2 = 0;
	grid_rot2 = new int* [len];
	for (int h = 0; h < len; h++)
	{
		grid_rot2[h] = new int[wid];
	}

	int s = 0;
	int t = 0;
	
		for (int i = 0; i < x; i += 2)
		{
			for (int j = 0; j < (y - 1); j += 2)
			{
				grid_rot2[s][t] = grid_rot[i][j];
				if (t < wid - 1)
					t++;
			}
			if (s < len - 1)
				s++;
			t = 0;
		}
	

	double* mean_row = 0;
	mean_row = new double[len];

	mean_row = Mean1R(len, wid, grid_rot2);
	double im_mean = MeanR(len, mean_row);

	int* max = 0;
	max = new int[len];
	for (int i = 0; i < len; i++)
	{
		max[i] = *std::max_element(grid_rot2[i], grid_rot2[i] + wid);
	}
	int max_val = *std::max_element(max, max + len);

	int* min = 0;
	min = new int[wid];
	for (int i = 0; i < len; i++)
	{
		min[i] = *std::min_element(grid_rot2[i], grid_rot2[i] + wid);
	}
	int min_val = *std::min_element(min, min + len);
	
	int val_range = max_val - min_val;
		// int val_range = max - (min_element(grid_rot2, grid_rot2 + len));

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
		int* where_out_arr = new int[where_out.size()]();
		std::copy(where_out.begin(), where_out.end(), where_out_arr);

		int* where_arg1 = decumulateInt(where_out_arr, (int)where_out.size());
		int where_arg = max_element(where_arg1, where_arg1 + where_out.size()) - where_arg1;
		int x11 = where_out[where_arg] * 2;
		int x22 = where_out[where_arg + 1] * 2;
		int p = 0;
		int q = 0;
		for (int i = x11; i < x22; i++)
		{
			for (int j = 0; j < y; j++)
			{
				grid_cut[p][q] = grid_rot[i][j];
				if (q < y - 1)
					q++;
			}
			if (p < x - 1)
				p++;
			q = 0;
		}
	}

	else
	{
		for (int i = 0; i < x; i ++)
		{
			for (int j = 0; j < y; j ++)
			{
				grid_cut[i][j] = grid_rot[i][j];
			}
		}
	}

	return grid_cut;
}

int grid_pos01::Execute(void) 
{
	string orientation;
	if ((s32.cut_ver_s >= 2) && (s32.cut_hor_s >= 2))
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

		s32.cut_hor.push_back(s32.imgRows / 2);
		s32.cut_ver.push_back(s32.imgCols / 2);

		int* cut_hor_arr = new int[s32.cut_hor.size()]();
		std::copy(s32.cut_hor.begin(), s32.cut_hor.end(), cut_hor_arr);

		int* cut_ver_arr = new int[s32.cut_ver.size()]();
		std::copy(s32.cut_ver.begin(), s32.cut_ver.end(), cut_ver_arr);

		int** grids = 0;
		grids = new int* [s32.cut_hor_s];
		for (int h = 0; h < s32.cut_hor_s; h++)
		{
			grids[h] = new int[s32.cut_ver_s];
		}

		memset(grids, 0, sizeof(grids[0][0]) * s32.cut_hor_s * s32.cut_ver_s);

		int image_size = s32.imgCols * s32.imgRows;
		double five_percent = image_size * 0.05;

		for (int row = 0; row < (s32.cut_hor.size()- 1); row++)
		{
			for (int col = 0; col < (s32.cut_ver.size() - 1); col++)
			{
				int x1 = cut_hor_arr[row] * 2;
				int x2 = (cut_hor_arr[row + 1]) * 2;
				int y1 = cut_ver_arr[col] * 2;
				int y2 = (cut_ver_arr[col + 1]) * 2;
				int s1 = x2 - x1;
				int s2 = y2 - y1;

				int** grid0 = 0;
				grid0 = new int* [s1];
				for (int h = 0; h < (s1); h++)
				{
					grid0[h] = new int[s2];
				}
				//memset(grid0, 0, sizeof(grid0[0][0]) * s1 * s2);

				if (x2>x1 && y2>y1)
				{
					for (int x = x1; x < x2; x++)
					{
						for (int y = y1; y < y2; y++)
						{
							grid0[x - x1][y - y1] = s32.img[x][y];
						}
					}
				}				

				int x11 = cut_hor_arr[row];
				int x22 = cut_hor_arr[row + 1];
				int y11 = cut_ver_arr[col];
				int y22 = cut_ver_arr[col + 1];
				int w1 = x22 - x11;
				int w2 = y22 - y11;


				int** mean2 = 0;
				mean2 = new int* [w1];
				for (int h = 0; h < (w1); h++)
				{
					mean2[h] = new int[w2];
				}

				for (int x = x11; x < x22; x++)
				{
					for (int y = y11; y < y22; y++)
					{
						mean2[x - x11][y - y11] = s32.img2[x][y];
					}
				}

				double* mean2_0 = 0;
				mean2_0 = new double[w2];
				double* mean2_1 = 0;
				mean2_1 = new double [w1];
				
				mean2_0 = Mean0R(w1, w2, mean2);
				mean2_1 = Mean1R(w1, w2, mean2);

				int wid1 = w2 / 6;
				int wid2 = w1 / 6;

				double* mean_grad000 = new double[w2]();
				mean_grad000 = Bandfilter(mean2_0, 0, wid1, w2);
				double* mean_grad111 = new double[w1]();
				mean_grad111 = Bandfilter(mean2_1, 0, wid2, w1);
				
				double* mean_grad00 = new double[w2]();
				mean_grad00 = gradient(mean_grad000, w2);
				double* mean_grad11 = new double[w1]();
				mean_grad11 = gradient(mean_grad111, w1);

				double* mean_grad0 = 0;
				mean_grad0 = new double[w2];

				for (int i = 0; i < w2; i++)
				{
					mean_grad0[i] = abs(mean_grad00[i]);
				}

				double* mean_grad1 = 0;
				mean_grad1 = new double[w1];

				for (int i = 0; i < w1; i++)
				{
					mean_grad1[i] = abs(mean_grad11[i]);
				}

				double mean_0grad0 = MeanR(w2, mean_grad0);
				double mean_1grad1 = MeanR(w1, mean_grad1);

				int** grid_rot = 0;
				
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
					grid_rot = new int* [s2];
					for (int h = 0; h < (s2); h++)
					{
						grid_rot[h] = new int[s1];
					}

					for (int i = 0; i < s2; i++)
					{
						for (int j = 0; j < s1; j++)
						{
							grid_rot[i][j] = grid0[j][i];
						}
					}
				}

				else
				{					
					grid_rot = new int* [s1];
					for (int h = 0; h < (s1); h++)
					{
						grid_rot[h] = new int[s2];
					}

					for (int i = 0; i < s1; i++)
					{
						for (int j = 0; j < s2; j++)
						{
							grid_rot[i][j] = grid0[i][j];
						}
					}
				}

				int grid_rot_size = s1 * s2;
				if ((grid_rot_size >= five_percent) || (orientation == "ver" && row == 0 && col == 1) || (orientation == "hor" && row == 1 && col == 0))
				{
					int** grid_cut = 0;

					if (orientation == "hor")
					{
						grid_cut = new int* [s2];
						for (int h = 0; h < (s2); h++)
						{
							grid_cut[h] = new int[s1];
						}
						grid_cut = cutGrid(grid_rot, s2, s1);
					}
					else
					{
						grid_cut = new int* [s1];
						for (int h = 0; h < (s1); h++)
						{
							grid_cut[h] = new int[s2];
						}
						grid_cut = cutGrid(grid_rot, s1, s2);
					}

				}


			}
		}
	}
	return 0;
}
