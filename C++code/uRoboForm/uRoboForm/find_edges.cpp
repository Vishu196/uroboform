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
	int* stripes = new int[n]();
	double* s_dic = new double[n]();
	int a = 0;
	int count = 0;

	for (int i = 1; i < n - 1; i++)
	{
		if (arr[i] >= arr[i - 1] && arr[i] >= arr[i + 1]) {
			if (arr[i] > th_edge) {
				stripes[a] = i;
				s_dic[a] = arr[i];
				a++;
				if (a>0) {
					if (stripes[a] - stripes[a-1] < 25)
					{
						stripes[a] = stripes[a + 1];
						s_dic[a] = s_dic[a + 1];
						count++;
					}
				}

			}
		}
	}
	/*for (int a = 0; a < 12; a++)
	{
		if (stripes[a + 1] - stripes[a] < 25)
		{
			stripes[a + 1] = stripes[a + 2];
			s_dic[a + 1] = s_dic[a + 2];
			count++;
		}
	}*/
	struct FP peaks;
	peaks.stripes = stripes;
	peaks.s_dic = s_dic;
	peaks.stripe_size = count;
	peaks.s_dic_size = count;

	return peaks;
}

double* find_edges::RFFT(double* x, int x_size)
{
	int N = x_size;
	double* y = new double[N];
	fftw_plan p;

	p = fftw_plan_r2r_1d(N, x, y, FFTW_R2HC, FFTW_ESTIMATE);//fftw_plan_dft_1d(N, in, y, FFTW_FORWARD, FFTW_ESTIMATE);

	fftw_execute(p);
	
	fftw_destroy_plan(p);

	double* yy = new double[N]();
	yy[0] = y[0];
	int j = 0;
	for (int i = 1; i < N; i+=2)
	{
		yy[i] = y[i-j];
		j++;
	}
	int k = 1;
	for (int i = 2; i < N + 1; i+=2)
	{
		yy[i] = y[N - k];
		k++;
	}

	return yy;
}

double* find_edges::IRFFT(double* x, int x_size)
{
	int N = x_size;
	double* y = new double[N]();

	double* xx = new double[N]();
	xx[0] = x[0];
	xx[1] = x[1];
	for (int i = 2; i < N/2; i ++)
	{
		xx[i] = x[(2*i) -1];
	}

	for (int i = N; i > N/2 ; i--)
	{
		xx[i-1] = x[2*(N-i) + 2];
	}

	fftw_plan p;

	p = fftw_plan_r2r_1d(N, xx, y, FFTW_HC2R, FFTW_ESTIMATE);//fftw_plan_dft_1d(N, in, y, FFTW_FORWARD, FFTW_ESTIMATE);
	
	fftw_execute(p);
	
	fftw_destroy_plan(p);
	//delete[] in;
	for (int i = 0; i < N; i++) {
		y[i] /= N;
	}

	return y;
}

double* find_edges::Bandfilter(double* x, int* limits, int x_size)
{
	int x0 = limits[0];
	int x1 = limits[1];

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

int* find_edges::ArgSort(double* s_dic, int s_dic_size)
{
	int* indice_arr = new int[s_dic_size]();
	double* sorted_arr = new double[s_dic_size]();

	for (int i = 0; i < s_dic_size; i++)
	{
		for (int j = i; j < (s_dic_size - 1); j++)
		{
			if (s_dic[i] > s_dic[j + 1])
			{
				sorted_arr[i] = s_dic[j + 1];
			}
		}
	}

	for (int i = 0; i < s_dic_size; i++)
	{
		for (int j = i; j < (s_dic_size); j++)
		{
			if (sorted_arr[i] == s_dic[j])
			{
				indice_arr[i] = j;

			}
		}

	}

	return indice_arr;
}

int* find_edges::insertXint(int size, int* arr,int x, int pos)
{
	if (pos>size)
	{
		size++;
		arr[size] = x;
	}
	else
	{
		size++;
		// shift elements forward 
		for (int i = size; i >= pos; i--)
			arr[i] = arr[i - 1];

		// insert x at pos 
		arr[pos - 1] = x;
	}

	return arr;
}

double* find_edges::insertXdouble(int size, double* arr, double x, int pos)
{
	if (pos > size)
	{
		size++;
		arr[size] = x;
	}
	else
	{
		size++;
		// shift elements forward 
		for (int i = size; i >= pos; i--)
			arr[i] = arr[i - 1];

		// insert x at pos 
		arr[pos - 1] = x;
	}
	return arr;
}

struct LI find_edges::Line_Index(double* mean_range_in,int arr_size, double th_edge, int i0, int rank = 1)
{
	int s_max, s_min;
	int s = arr_size;
	int x1 = int(arr_size / 6);
	int* limits = new int[2];
	limits[0] = 0;
	limits[1] = x1;
	double* mean_range = new double[s]();
	mean_range = Bandfilter(mean_range_in, limits, s);

	double* mean_rangeN = new double[s]();
	for(int i = 0; i <= s; i++)
	{
		mean_rangeN[i] = mean_range[i] * (-1);
	}
	struct FP peaks_max = Find_Peaks(mean_range, s, th_edge);
	struct FP peaks_min = Find_Peaks(mean_rangeN, s, -th_edge);

	if (peaks_max.stripe_size >=1 && rank <= peaks_max.s_dic_size)
	{
		int* indice_arr = new int[peaks_max.s_dic_size]();
		indice_arr = ArgSort(peaks_max.s_dic, peaks_max.s_dic_size);
		int tmp = peaks_max.s_dic_size - rank -1;
		s_max = peaks_max.stripes[tmp] + i0;
	}
	else
	{
		s_max = std::nan("");
	}
	if ((peaks_max.stripe_size >= 3 && peaks_min.stripe_size>=3 ))
	{
		if (peaks_min.stripes[0] > 25)
		{
			peaks_min.stripes = insertXint(peaks_min.stripe_size, peaks_min.stripes, 0, 0);
			peaks_min.s_dic = insertXdouble(peaks_min.s_dic_size, peaks_min.s_dic, mean_rangeN[0], 0);
		}
		if (s-peaks_min.stripes[(peaks_min.stripe_size-1)] > 25)
		{
				peaks_min.stripes = insertXint(peaks_min.stripe_size, peaks_min.stripes, (s-1), (peaks_min.stripe_size+1));
				peaks_min.s_dic = insertXdouble(peaks_min.s_dic_size, peaks_min.s_dic, mean_rangeN[(s - 1)], (peaks_min.s_dic_size+1));
		}

		int s_dic_min_size = peaks_min.s_dic_size - 2;
		double* s_dic_min = new double[s_dic_min_size]();
		for (int i = 0; i < (peaks_min.s_dic_size-1); i++)
		{
			s_dic_min[i] = peaks_min.s_dic[i + 1];
		}

		int n_0 = std::distance(s_dic_min, std::max_element(s_dic_min, s_dic_min + (peaks_min.s_dic_size - 2)));

		s_min = peaks_min.stripes[n_0 + 1] + i0;

	}
	else
	{
		s_min = std::nan("");
	}

	struct LI index;
	index.s_max = s_max;
	index.s_min = s_min;

	return index;
}

int find_edges::Execute(void)
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
	
	rank =+ 1;
	struct LI index = Line_Index(mean_range0, R, s21.th_edge, i0, rank);
	int s_max = index.s_max;
	

	return s_max;
}