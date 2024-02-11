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
	s34.imgRows = 0;
	s34.imgCols = 0;
	s34.img = new int* [1080];
	for (int h = 0; h < 1080; h++)
	{
		s34.img[h] = new int[1440];
	}

	s34.gridRows = 0;
	s34.gridCols = 0;

	s34.grids = new Grid*[3];
	for (int i = 0; i < 3; i++)
	{
		s34.grids[i] = new Grid[3];
	}
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

double* grid_pos01::RFFT(double * x, int x_size)
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

double* decumulateDouble(double* x, int size)
{
	const size_t n = size - 1;
	double* xi = new double[n]();
	double* x1 = new double[n]();
	double* x2 = new double[n]();

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

		delete[] where_arg1;
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

	delete[] grid_rot2;
	delete[] mean_row;
	delete[] max;
	delete[] min;

	return grid_cut;
}

struct FP grid_pos01::Find_Peaks(double* arr, int n, int dist)
{
	int* stripes = new int[n]();
	double* s_dic = new double[n]();
	int a = 0;
	int count = 0;

	for (int i = 1; i < n - 1; i++)
	{
		if (arr[i] >= arr[i - 1] && arr[i] >= arr[i + 1]) 
		{
				stripes[a] = i;
				s_dic[a] = arr[i];
				a++;
				if (a > 0) {
					if (stripes[a] - stripes[a - 1] < dist)
					{
						stripes[a] = stripes[a + 1];
						s_dic[a] = s_dic[a + 1];
						count++;
					}
				}
		}
	}

	struct FP peaks;
	peaks.stripes = stripes;
	peaks.s_dic = s_dic;
	peaks.stripe_size = count;
	peaks.s_dic_size = count;

	return peaks;
}

int* grid_pos01::insertXint(int size, int* arr, int x, int pos)
{
	int* temp = new int[size + 1];
	std::copy(arr, arr + size, temp);

	if (pos > size)
		return NULL;
	if (pos == size)
	{
		temp[size] = x;
	}
	else
	{
		// shift elements forward 
		for (int i = size; i > pos; i--)
			temp[i] = temp[i - 1];

		// insert x at pos 
		temp[pos] = x;
	}
	delete[] arr;
	return temp;
}

int* grid_pos01::deleteXint(int size, int* arr, int pos)
{
	size--;

	if (pos > size)
		return NULL;

	for (int i = size; i > pos; i--)
		arr[i - 2] = arr[i - 1];

	return arr;
}

double* grid_pos01::BlackmanWindowR(int n)
{
	const double a0 = 0.42;
	const double a1 = 0.5;
	const double a2 = 0.08;
	int wLen = n - 1;
	double* wFun = 0;
	wFun = new double[n]();

	for (int i = 0; i < n; ++i)
	{
		double wi = 0.0;
		wi = a0 - (a1 * cos((2 * M_PI * i) / wLen)) + (a2 * cos((4 * M_PI * i) / wLen));
		wFun[i] = wi;
	}
	return wFun;
}

double* grid_pos01::FFTR(double* image_windowR, int size)
{
	const int N = 256;
	fftw_complex* y = 0;
	y = new fftw_complex[N];
	double in[N];
	fftw_plan p;

	for (int i = 0; i < N; i++) {
		if (i < size) {
			in[i] = image_windowR[i];
		}
		else {
			in[i] = 0;
		}
	}
	p = fftw_plan_dft_r2c_1d(N, in, y, FFTW_ESTIMATE);//fftw_plan_dft_1d(N, in, y, FFTW_FORWARD, FFTW_ESTIMATE);

	fftw_execute(p);
	std::complex<double>* yy;
	yy = reinterpret_cast<std::complex<double> *>(y);
	double* y1 = 0;
	y1 = new double[N];

	for (int i = 0; i < N; i++)
	{
		y1[i] = abs(yy[i]);
	}

	fftw_destroy_plan(p);
	delete[] y;
	return y1;
}

double grid_pos01::Spek_InterpolR(double* A) {

	uint32_t A_size = 256;
	uint32_t A2_size = A_size / 2;

	double* A2 = 0;
	A2 = new double[A2_size]();
	for (uint32_t i = 0; i < A2_size; i++)
	{
		A2[i] = A[i];
	}

	int n_0 = (int)std::distance(A2, std::max_element(A2, A2 + A2_size));

	double y_ln1 = log(A[n_0 + 1]);
	double y_ln0 = log(A[n_0]);
	double y_ln_1 = log(A[n_0 - 1]);
	double tmp = (y_ln_1 - y_ln1) / (y_ln_1 - (2 * y_ln0) + y_ln1);
	double n_g = (n_0 + tmp / 2);
	delete[] A2;
	return n_g;
}

struct MFreq grid_pos01::Main_FreqR(double* B0, int start, int stop)
{
	double f_g = 0.0;
	const int size = stop - start;

	double* B = new double[size]();

	double* image_window = 0;
	image_window = new double[size]();

	for (int k = 0; k < size; k++)
	{
		B[k] = B0[k + start];
	}

	double Mean = MeanR(size, B);

	double* B1 = new double[size]();

	for (int i = 0; i < size; i++)
	{
		B1[i] = B[i] - Mean;
	}

	double* wFun = BlackmanWindowR(size);
	for (int i = 0; i < size; i++)
	{
		image_window[i] = B1[i] * wFun[i];
	}

	double* y1 = FFTR(image_window, size);

	double n_g = Spek_InterpolR(y1);
	uint32_t size_B = size;
	f_g = n_g / size_B;

	delete[] B1;
	delete[] B;
	delete[] wFun;
	delete[] y1;

	struct MFreq mf;
	mf.Image_window = image_window;
	mf.n_g = n_g;
	mf.f_g = f_g;

	return mf;
}

double* gauss_limited(double x, double k, double sigma, double mu, double offset,  int max_cut)
{
	return 0;
}

struct subPX grid_pos01::subpx_gauss(double* B_cut, struct FP B_max, struct FP B_min, double d_m)
{
	list<double> max_pos;
	list<double> pres;
	int xmin;
	int xmax;


	for (int i_b = 0; i_b < B_max.stripe_size; i_b++)
	{
		int mid = B_max.stripes[i_b];

		if (B_min.stripe_size >=2)
		{
			for (int i_0 = 0; i_0 < B_min.stripe_size; i_0++)
			{
				if (B_min.stripes[i_0] < mid)
					xmin = i_0;
			}
			for (int i_1 = 0; i_1 <= B_min.stripe_size; i_1++)
			{
				if (B_min.stripes[B_min.stripe_size-i_1] > mid)
					xmax = i_1;
			}

			try
			{
				if (xmin!=xmax)
				{

				}
			}
			catch (const runtime_error& error)
			{
				cout << "Optimal Parameters not found for image at x =' " << mid << endl;
			}
			catch (std::exception&)
			{
				continue;
			}
		}

	}

	struct subPX p;
	return p;
}

struct subPX grid_pos01::subpx_parabel(double* B_cut, struct FP B_max, struct FP B_min, double d_m)
{
	list<double> max_pos;
	int xmin;
	int xmax;


	for (int i_b = 0; i_b < B_max.stripe_size; i_b++)
	{
		int mid = B_max.stripes[i_b];

		if (B_min.stripe_size >= 2)
		{
			for (int i_0 = 0; i_0 < B_min.stripe_size; i_0++)
			{
				if (B_min.stripes[i_0] < mid)
					xmin = int(i_b-d_m/4)+3;
			}
			for (int i_1 = 0; i_1 <= B_min.stripe_size; i_1++)
			{
				if (B_min.stripes[B_min.stripe_size - i_1] > mid)
					xmax = int(i_b + d_m / 4) + 3;
			}

			try
			{
				if (xmin != xmax)
				{
					int i = 0;
					int x_size = xmax - xmin;
					int* x = new int[x_size];
					for (int p = xmin; p < xmax; p++)
					{
						x[i] = p;
						if (i < x_size)
						{
							i++;
						}
					}

					int** W0 = new int* [x_size];
					for (int h = 0; h < x_size; h++)
					{
						W0[h] = new int[x_size];
					}

					for (int row = 0; row < x_size; row++)
					{
						for (int col = 0; col < x_size; col++)
						{
							if (row == col)
								W0[row][col] = 1;
							else
								W0[row][col] = 0;
						}
					}

					int** arr_x1 = new int*[x_size];
					for (int i = 0; i < x_size; i++)
					{
						arr_x1[i] = new int[1];
					}
					
					for (int row = 0; row < x_size; row++)
						for (int col = 0; col < 1; col++)
							arr_x1[row][col] = 1;

					int** arr_x2 = new int* [x_size];
					for (int i = 0; i < x_size; i++)
					{
						arr_x2[i] = new int[1];
					}

					for (int row = 0; row < x_size; row++)
						for (int col = 0; col < 1; col++)
							arr_x2[row][col] = x[row];

					int** arr_x3 = new int* [x_size];
					for (int i = 0; i < x_size; i++)
					{
						arr_x3[i] = new int[1];
					}

					for (int row = 0; row < x_size; row++)
						for (int col = 0; col < 1; col++)
							arr_x3[row][col] = x[row]*x[row];

					int** Phi = new int* [x_size];
					for (int i = 0; i < x_size; i++)
					{
						Phi[i] = new int[3];
					}

					for (int row = 0; row < x_size; row++)
						for (int col = 0; col < 1; col++)
							Phi[row][col] = arr_x1[row][col];

					for (int row = 0; row < x_size; row++)
						for (int col = 1; col < 2; col++)
							Phi[row][col] = arr_x2[row][col-1];

					for (int row = 0; row < x_size; row++)
						for (int col = 2; col < 3; col++)
							Phi[row][col] = arr_x3[row][col-2];

				}

			}
			catch (const runtime_error& error)
			{
				cout << "Optimal Parameters not found for image at x =' " << mid << endl;
			}
			catch (std::exception&)
			{
				continue;
			}
		}

	}

	struct subPX p;
	return p;

}

struct subPX grid_pos01::subpx_phase(int** cutGrid, int x, int y)
{
	list<double> max_pos;
	list<double> pres;
	double* B0 = new double[y];
	B0 = Mean0R(x, y, cutGrid);

	double B = MeanR(y, B0);

	if (y >=60)
	{
		struct MFreq m = Main_FreqR(B0, 0, y);
		double d_mean = 1 / m.f_g;

		double* B_arange = new double[y];
		for (int i = 0; i < y; i++)
		{
			B_arange[i] = i;
		}
		
		complex<double> exp_fac = (0, -(2 * M_PI)*(*B_arange)/y);

		complex<double>* F_k1 = new complex<double>[y];
		complex<double> F_k = (0,0);

		for (int i = 0; i < y; i++) 
		{
			F_k1[i] = m.Image_window[i] * exp(exp_fac * m.n_g);
			F_k += F_k1[i];
		}

		double Phi = std::arg(F_k);
		double d = Phi / (2 * M_PI * m.f_g);

		double A1 = pow(real(F_k),2);
		double A2 = pow(imag(F_k), 2);
		double A = pow((A1 + A2), 0.5);

		double y = (2 * M_PI * m.f_g * (*B_arange)) - Phi;
		double y_cos = A * cos(y);

		if (d < d_mean/2)
		{
			d += d_mean;
		}

		for (int i_max = 0; i_max < (m.n_g-1); i_max++)
		{
			max_pos.push_back(d + i_max * d_mean);
		}

		delete[] F_k1;
		delete[] B_arange;
	}
	else
	{
		max_pos.clear();
	}
	pres.clear();

	struct subPX p;
	p.max_pos = max_pos;
	p.pres = pres;

	
	return p;
}

struct subPX grid_pos01::subpx_max_pos(int** cutGrid, int x, int y, int stripe_width, float px_size, string mode)
{
	struct subPX p;
	if (mode == "phase")
	{
		p = subpx_phase(cutGrid,x,y);
	}
	else
	{
		double* B = new double[y];
		B = Mean0R(x, y, cutGrid);

		double filt = 64 / 640 * y;

		double* B_cut = new double[y]();
		B_cut = Bandfilter(B, 0, (int)filt, y);
		
		double d_min = stripe_width / (double)1000 * 2 / px_size / 2;

		double prom = (*std::max_element(B_cut, B_cut + y)) - (*std::min_element(B_cut, B_cut + y));
		
		double* B_cut_N = new double[y]();
		for (int i = 0; i < y; i++)
		{
			B_cut_N[i] = B_cut[i] * (-1);
		}

		//the functiom Find_peaks needs to be chnaged as per current requirement and then called here
		// the below func call is incorrect 
		struct FP B_max = Find_Peaks(B_cut, y, d_min);
		struct FP B_min = Find_Peaks(B_cut_N, y, d_min);

		if ((B_max.stripe_size>=1) && (B_min.stripe_size>=1))
		{
			if ((B_min.stripes[0]>B_max.stripes[0]) && (B_max.stripes[0] >= 0.9*d_min))
			{
				B_min.stripes = insertXint(B_min.stripe_size, B_min.stripes, 0, 0);
				B_min.stripe_size++;
			}
			if ((B_min.stripes[B_min.stripe_size-1] < B_max.stripes[B_max.stripe_size-1]) && (y- B_max.stripes[B_max.stripe_size-1] >= 0.9*d_min))
			{
				B_min.stripes = insertXint(B_min.stripe_size, B_min.stripes, (y - 1), (B_min.stripe_size));
				B_min.stripe_size++;
			}
			if ((B_max.stripes[0] - B_min.stripes[0] > 0) && (B_max.stripes[0] - B_min.stripes[0] < 0.8*d_min))
			{
				B_min.stripes = deleteXint(B_min.stripe_size, B_min.stripes, 0);
				B_min.stripe_size--;
			}

			int* d0 = new int[B_max.stripe_size]();
			d0 = decumulateInt(B_min.stripes, B_min.stripe_size);

			double* dB_Min = new double [B_min.stripe_size]();
						
			for (int i = 0; i < B_min.stripe_size; i++) 
			{
				dB_Min[i] = (double)B_min.stripes[i];
			}

			double d_m = MeanR(B_min.stripe_size, dB_Min);

			if (mode == "gauss")
			{
				int e = 0;
			//	p = subpx_gauss(B_cut, struct FP B_max, struct FP B_min, d_m);
			}

			else if (mode == "parabel")
			{
				int t = 0;
			//	p = subpx_parabel(B_cut, B_max.stripes, B_min.stripes, d_m);
			}
		}
	}
	
	return p;
}

stage34 grid_pos01::Execute(void) 
{
	string orientation;
	const int h1 = 800;
	const int h2 = 20;
	const int stripe_width = 100;
	const float px_size = 3.45;
	string mode = "gauss";

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
		s32.cut_hor_s++;
		s32.cut_ver.push_front(0);
		s32.cut_ver_s++;

		s32.cut_hor.push_back(s32.imgRows / 2);
		s32.cut_hor_s++;
		s32.cut_ver.push_back(s32.imgCols / 2);
		s32.cut_ver_s++;

		int* cut_hor_arr = new int[s32.cut_hor.size()]();
		std::copy(s32.cut_hor.begin(), s32.cut_hor.end(), cut_hor_arr);

		int* cut_ver_arr = new int[s32.cut_ver.size()]();
		std::copy(s32.cut_ver.begin(), s32.cut_ver.end(), cut_ver_arr);

		Grid** grids = 0;
		grids = new Grid* [s32.cut_hor.size()];
		for (int h = 0; h < s32.cut_hor.size(); h++)
		{
			grids[h] = new Grid[s32.cut_ver.size()];
		}

		memset(grids, 0, sizeof(grids[0][0]) * s32.cut_hor_s * s32.cut_ver_s);

		int image_size = s32.imgCols * s32.imgRows;
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

				struct subPX p;
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

					p = subpx_max_pos(grid_cut, s1, s2, stripe_width, px_size, mode);

					int r = p.max_pos.size();
					double* max_pos_arr = new double[r]();
					copy(p.max_pos.begin(), p.max_pos.end(), max_pos_arr);
					
					double* max_pos_de = decumulateDouble(max_pos_arr, r);
					
					if ((r>1) && (max_pos_de[r]>65))
					{
						p.max_pos.pop_back();
					}

					if ((r > 1) && (max_pos_de[0] > 65))
					{
						p.max_pos.pop_front();
					}

				}
				else
				{
					p.max_pos.clear();
					p.pres.clear();
				}

				list<int> coord;
				coord.push_back(cut_hor_arr[row] * 2);
				coord.push_back(cut_ver_arr[col] * 2);
				grids[row][col] = Grid(grid_rot, image_size, orientation, coord, p.max_pos);
			}
		}
		s34.imgCols = s32.imgCols;
		s34.imgRows = s32.imgRows;

		for (int i = 0; i < s32.imgRows; i++)
		{
			memcpy(s34.img[i], s32.img[i], (s32.imgCols * sizeof(int)));
		}

		s34.gridRows = s32.cut_hor.size();
		s34.gridCols = s32.cut_ver.size();

		for (int i = 0; i < s32.cut_hor.size(); i++)
		{
			memcpy(s34.grids[i], grids[i], (s32.cut_ver.size() * sizeof(Grid)));
		}

	}
		return s34;
}
