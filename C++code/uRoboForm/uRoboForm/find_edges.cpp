#include "find_edges.h"

static struct stage21 s21;
static struct stage23 s23;

static void buffer_s21_init() {
	s21.mean0 = 0;
	s21.mean1 = 0;
	s21.mean0 = new double[720];
	s21.mean1 = new double[540];


	s21.img = new int* [1080];;// new int[1080][1440];
	for (int h = 0; h < 1080; h++)
	{
		s21.img[h] = new int[1440];
	}

	s21.img2 = new int* [540];;// new int[1080][1440];
	for (int h = 0; h < 540; h++)
	{
		s21.img2[h] = new int[720];
	}
}

static void buffer_s23_init()
{
	s23.cut_hor = new int[2];
	s23.cut_ver = new int[2]; 

	s23.img = new int* [1080];;// new int[1080][1440];
	for (int h = 0; h < 1080; h++)
	{
		s23.img[h] = new int[1440];
	}

	s23.img2 = new int* [540];;// new int[1080][1440];
	for (int h = 0; h < 540; h++)
	{
		s23.img2[h] = new int[720];
	}
}

static void buffers_init(void) 
{
	buffer_s21_init();
	buffer_s23_init();
}

find_edges::find_edges(struct stage12 s12)
{
	buffers_init();
	s21.imgRows = s12.imgRows;
	s21.imgCols = s12.imgCols;

	memcpy(s21.mean0, s12.mean0, (s12.imgCols/2) * sizeof(double));
	memcpy(s21.mean1, s12.mean1, (s12.imgRows/2) * sizeof(double));
	
	s21.main_d_0 = s12.main_d_0;
	s21.main_d_1 = s12.main_d_1;
	s21.th_edge = s12.th_edge;

	for (int i = 0; i < s12.imgRows; i++)
	{
		memcpy(s21.img, s12.img, (s12.imgCols * sizeof(int)));
	}

	for (int i = 0; i < (s12.imgRows / 2); i++)
	{
		memcpy(s21.img2, s12.img2, ((s12.imgCols / 2) * sizeof(int)));
	}
}

struct FP find_edges::Find_Peaks(double* arr, int n, double th_edge)
{
	int* stripes = new int[12];
	double* s_dic = new double[12];
	int a = 0;
	int count = 0;

	for (int i = 1; i < n - 1; i++)
	{
		if (arr[i] >= arr[i - 1] && arr[i] >= arr[i + 1]) {
			if (arr[i] > th_edge) {
				stripes[a] = i;
				s_dic[a] = arr[i];
				a++;
			}
		}
	}
	for (int a = 0; a < 11; a++)
	{
		if (stripes[a + 1] - stripes[a] < 25)
		{
			stripes[a + 1] = stripes[a + 2];
			s_dic[a + 1] = s_dic[a + 2];
			count++;
		}
	}

	struct FP peaks;
	peaks.stripes = stripes;
	peaks.s_dic = s_dic;
	peaks.stripe_size = count;
	peaks.s_dic_size = count;

	return peaks;
}

std::complex<double>* find_edges::RFFT(double* x, int x_size)
{
	int N = 1;
	while (N < x_size)
		N *= 2;
	
	fftw_complex* y = 0;
	y = new fftw_complex[N];
	double* in = new double[N]();
	fftw_plan p;

	for (int i = 0; i < N; i++) {
		if (i < x_size) {
			in[i] = x[i];
		}
		else {
			in[i] = 0;
		}
	}
	p = fftw_plan_dft_r2c_1d(N, in, y, FFTW_ESTIMATE);//fftw_plan_dft_1d(N, in, y, FFTW_FORWARD, FFTW_ESTIMATE);

	fftw_execute(p);
	std::complex<double>* yy;
	yy = reinterpret_cast<std::complex<double> *>(y);
	
	fftw_destroy_plan(p);
	delete[] in;

	return yy;
}

double* find_edges::IRFFT(std::complex<double>* x, int x_size)
{
	int N = 1;
	while (N < x_size)
		N *= 2;

	fftw_complex* y = 0;
	y = new fftw_complex[N];

	fftw_complex* in = new fftw_complex[N]();
	fftw_plan p;

	fftw_complex* xx;
	xx = reinterpret_cast<fftw_complex*>(x);

	for (int i = 0; i < N; i++) 
	{
		if (i < x_size) 
		{
			in[i][0] = xx[i][0];
			in[i][1] = xx[i][1];
		}
		else {
			in[i][0] = 0;
			in[i][1] = 0;
		}
	}
	p = fftw_plan_dft_1d(N, in, y, FFTW_BACKWARD, FFTW_ESTIMATE);//fftw_plan_dft_1d(N, in, y, FFTW_FORWARD, FFTW_ESTIMATE);

	fftw_execute(p);
	double* yy;
	yy = reinterpret_cast<double*>(y);

	fftw_destroy_plan(p);
	delete[] in;

	return yy;
}

double* find_edges::Bandfilter(double* x, int* limits, int x_size)
{
	int x0 = limits[0];
	int x1 = limits[1];

	std::complex<double>* f_x = new std::complex<double>[x_size]();

	f_x = RFFT(x, x_size);

	std::complex<double>* f_x_cut;
	f_x_cut = f_x;

	for (int i = 0; i <= x0; i++)
	{
		f_x_cut[i] = 0;
	}

	for (int i = x1+1; i < x_size; i++)
	{
		f_x_cut[i] = 0;
	}

	double* x_cut = new double[x_size]();

	x_cut = IRFFT(f_x_cut, x_size);

	return x_cut;
}

int find_edges::Line_Index(double* mean_range_in,int arr_size, double th_edge, int i0, int rank = 1)
{
	int s_max, s_min;
	int s = arr_size;
	int x1 = int(arr_size / 6);
	int* limits = new int[2];
	limits[0] = 0;
	limits[1] = x1;
	double* mean_range = new double[s]();
	mean_range = Bandfilter(mean_range_in, limits, s);

	double* mean_rangeN = new double[s];
	for(int i = 0; i <= s; i++)
	{
		mean_rangeN[i] = -mean_range[i];
	}
	struct FP peaks_max = Find_Peaks(mean_range, s, th_edge);
	struct FP peaks_min = Find_Peaks(mean_rangeN, s, -th_edge);

	if (peaks_max.stripe_size >=1 && rank <= peaks_max.s_dic_size)
	{
		s_max = 

	}

	return 0;
}

struct stage23 find_edges::Execute(void)
{
	
	int size_mean0 = s21.imgCols / 2;
	const int mid = size_mean0 / 2;
	const int search_range = 150;
	int i0 = mid - search_range;
	int i1 = mid + search_range;
	int R = i1 - i0;

	double* mean_range0 = 0;
	mean_range0 = new double[R]();
	for (int i = i0; i <= i1; i++)
	{
		mean_range0[i-i0] = s21.mean0[i];
	}

	int rank = 0;
	
	while (rank < 5)
	{
		rank = +1;
		int s_max = Line_Index(mean_range0, s21.th_edge, i0, rank);

	}

	return s23;
}