#include "find_edges.h"
#include <cmath>

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

	int cols2 = s21.imgCols / 2;
	int rows2 = s21.imgRows / 2;

	memcpy(s21.mean0, s12.mean0, (cols2) * sizeof(double));
	memcpy(s21.mean1, s12.mean1, (rows2) * sizeof(double));
	
	s21.main_d_0 = s12.main_d_0;
	s21.main_d_1 = s12.main_d_1;
	s21.th_edge = s12.th_edge;

	for (int i = 0; i < s12.imgRows; i++)
	{
		memcpy(s21.img[i], s12.img[i], (s21.imgCols * sizeof(int)));
	}

	for (int i = 0; i < (rows2); i++)
	{
		memcpy(s21.img2[i], s12.img2[i], (cols2 * sizeof(int)));
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
	double* y = new double[N]();
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

	delete[] y;
	return yy;
}

double* find_edges::IRFFT(double* x, int x_size)
{
	int N = x_size;
	double* y = new double[N]();

	double* xx = new double[N]();
	xx[0] = x[0];
	//xx[1] = x[1];
	for (int i = 1; i < N/2; i ++)
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
	for (int i = 0; i < N; i++) {
		y[i] /= N;
	}

	delete[] xx;

	return y;
}

double* find_edges::Bandfilter(double* x, int x0, int x1, int x_size)
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

int* find_edges::ArgSort(double* s_dic, int s_dic_size)
{
	int* indice_arr = new int[s_dic_size]();
	double* sorted_arr = new double[s_dic_size]();
	std::copy(s_dic, s_dic + s_dic_size, sorted_arr);

	std::sort(sorted_arr, sorted_arr + s_dic_size);

	for (int i = 0; i < s_dic_size; i++)
	{
		for (int j = 0; j < (s_dic_size); j++)
		{
			if (sorted_arr[i] == s_dic[j])
			{
				indice_arr[i] = j;
				break;
			}
		}

	}
	delete[] sorted_arr;
	return indice_arr;
}

int* find_edges::insertXint(int size, int* arr,int x, int pos)
{
	int* temp = new int[size+1];
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

double* find_edges::insertXdouble(int size, double* arr, double x, int pos)
{
	double* temp = new double[size + 1]();
	std::copy(arr, arr + size, temp); // Suggested by comments from Nick and Bojan
	
	if (pos > size)
		return NULL;
	if (pos == size)
	{
		temp[size] = x;
	}
	else
	{
		// shift elements forward 
		for (int i = size; i >= pos; i--)
		{
			temp[i] = temp[i - 1];
		}
		// insert x at pos 
		temp[pos] = x;
	}
	delete[] arr;
	return temp;
}

double find_edges::std_dev(double* arr, int start, int stop)
{
	double sum = 0.0, mean, standardDeviation = 0.0;
	int i;
	int size = stop - start;

	double* B = new double[size]();

	for (int k = 0; k < size; k++)
	{
		B[k] = arr[k + start];
	}

	for (i = 0; i < size; ++i) 
	{
		sum += B[i];
	}

	mean = sum / size;

	for (i = 0; i < size; ++i) 
	{
		standardDeviation += pow(B[i] - mean, 2);
	}

	delete[] B;
	return sqrt(standardDeviation / size);
}

struct LI find_edges::Line_Index(double* mean_range_in,int arr_size, double th_edge, int i0, int rank)
{
	double s_max, s_min;
	int s = arr_size;
	int x1 = int(arr_size / 6);
	double* mean_range = new double[s]();
	mean_range = Bandfilter(mean_range_in, 0, x1, s);

	double* mean_rangeN = new double[s]();
	for(int i = 0; i < s; i++)
	{
		mean_rangeN[i] = mean_range[i] * (-1);
	}
	struct FP peaks_max = Find_Peaks(mean_range, s, th_edge);
	struct FP peaks_min = Find_Peaks(mean_rangeN, s, -th_edge);

	if (peaks_max.stripe_size >=1 && rank <= peaks_max.s_dic_size)
	{
		int* indice_arr = new int[peaks_max.s_dic_size]();
		indice_arr = ArgSort(peaks_max.s_dic, peaks_max.s_dic_size);
		int tmp = indice_arr[peaks_max.s_dic_size - rank];
		s_max = peaks_max.stripes[tmp] + i0;
		delete[] indice_arr;
	}
	else
	{
		s_max = nan("");
	}
	if ((peaks_max.stripe_size >= 3 && peaks_min.stripe_size>=3 ))
	{
		if (peaks_min.stripes[0] > 25)
		{
			peaks_min.stripes = insertXint(peaks_min.stripe_size, peaks_min.stripes, 0, 0);
			peaks_min.s_dic = insertXdouble(peaks_min.s_dic_size, peaks_min.s_dic, mean_rangeN[0], 0);
			peaks_min.stripe_size++;
			peaks_min.s_dic_size++;
		}
		if (s-peaks_min.stripes[(peaks_min.stripe_size-1)] > 25)
		{
			peaks_min.stripes = insertXint(peaks_min.stripe_size, peaks_min.stripes, (s-1), (peaks_min.stripe_size));
			peaks_min.s_dic = insertXdouble(peaks_min.s_dic_size, peaks_min.s_dic, mean_rangeN[(s - 1)], (peaks_min.s_dic_size));
			peaks_min.stripe_size++;
			peaks_min.s_dic_size++;
		}

		const int s_dic_min_size = (peaks_min.s_dic_size) - 1;
		double* s_dic_min = new double[s_dic_min_size]();
		for (int i = 0; i < (peaks_min.s_dic_size-1); i++)
		{
			if (i < peaks_min.s_dic_size - 1) 
			{
				s_dic_min[i] = peaks_min.s_dic[i + 1];
			}
			
		}

		int n_0 = (int)std::distance(s_dic_min, std::max_element(s_dic_min, s_dic_min + (peaks_min.s_dic_size - 1)));

		s_min = peaks_min.stripes[n_0 + 1] + i0;
		delete[] s_dic_min;
	}
	else
	{
		s_min = nan("");
	}

	struct LI index;
	index.s_max = s_max;
	index.s_min = s_min;

	delete[] mean_range;
	delete[] mean_rangeN;
	
	return index;
}

int* find_edges::decumulateInt(int* x, int size)
{
	const size_t n = size - 1;
	int* xi = new int[n]();
	int* x1 = new int[n]();
	int* x2 = new int[n]();

	for (size_t i = 0; i < size-1; i++)
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

struct DT find_edges::Detect_Through(double* im_col, double th_edge , int size)
{
	double* im_diff = new double[size]();
	for (int i = 0; i < size; i++)
	{
		im_diff[i] = im_col[i] - th_edge;
	}
	
	int n = size - 1;
	bool* signbit = new bool[size]();
	for(int i = 0; i < size; i++)
	{ 
		signbit[i] = !(std::signbit(im_diff[i]));
	}
	
	bool* th_through = new bool[n]();
	int* through_loc1 = new int[n]();
	int count = 0;
	for (int i = 0; i < n; i++)
	{
		if (signbit[i] == signbit[i + 1])
		{
			th_through[i] = false;
		}
		else 
		{
			th_through[i] = true;
		}

		if (th_through[i] == true)
		{
			through_loc1[count] = i;
			count++  ;
		}
	}

	int* through_loc = new int[count]();
	std::copy(through_loc1, through_loc1 + count, through_loc);
	
	through_loc = insertXint(count, through_loc, 0, 0);
	count++;
	through_loc = insertXint(count, through_loc, size, count);
	count++;

	int count2 = count - 1;
	int* d_through = new int[count2]();
	d_through = decumulateInt(through_loc, count);

	int* cut_through1 = new int[count2]();
	int j = 0;
	for (int i = 0; i < (count2); i++)
	{
		if (d_through[i] > 33)
		{
			cut_through1[j] = i; 
			j++;
		}
	}

	int* cut_through = new int[j]();
	std::copy(cut_through1, cut_through1 + j, cut_through);
	
	struct DT thro;
	thro.through_loc = through_loc;
	thro.cut_through = cut_through;
	thro.cut_throu_size = j;
	thro.thro_loc_size = count;

	delete[] im_diff;
	delete[] signbit;
	delete[] through_loc1;
	delete[] th_through;
	delete[] cut_through1;

	return thro;
}

int find_edges::Execute(void)
{
	if (s21.main_d_0 && s21.main_d_1)
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
		int len = s21.imgRows / 2;
		double* im_col = new double[len]();
		while (rank < 5)
		{
			rank += 1;
			struct LI index = Line_Index(mean_range0, R, s21.th_edge, i0, rank);
			double s_max = index.s_max;
			int s_m = (int) s_max;
			bool res = isnan(s_max);
			if (!res)
			{
				int x = s21.imgRows / 2 / 6;
				double* img_col = new double[len]();
				for (int i = 0; i < len; i++)
				{
					img_col[i] = (double)s21.img2[i][s_m];
				}
				
				im_col = Bandfilter(img_col, 0, x, len);

				int n1 = len - 150;
				double* std_col = new double[n1]();
				int j = 0;
				for (int i = 0; i < n1; i ++)
				{
					double tmp = std_dev(im_col, i, i + 150);
					std_col[j] = tmp;
					j++;
				}
				double c1 = *min_element(std_col, std_col + n1);
				double c2 = (*max_element(im_col, im_col + len)) - (*min_element(im_col, im_col + len));
				double condition1 = c1 / c2;
				if (condition1 <= 0.085)
				{
					break;
				}
			}
	
		}
	
		try
		{
			list<int> cut_hor;
			cut_hor.clear();
			int* cut_hor_arr = new int[cut_hor.size()]();
			if (rank != 5)
			{
				struct DT t = Detect_Through(im_col, s21.th_edge, len);

				for (int i = 0; i < t.cut_throu_size; i++)
				{
					if ((t.through_loc[t.cut_through[i]] == 0) && (im_col[t.through_loc[t.cut_through[i]]+1] > s21.th_edge)) 
					{
						cut_hor.push_back(t.through_loc[t.cut_through[i] + 1]);
					}
					else if (t.cut_through[i] == (t.thro_loc_size - 2))
					{
						cut_hor.push_back(t.through_loc[t.cut_through[i]]);
					}
					else if (im_col[t.through_loc[t.cut_through[i]] + 1] > s21.th_edge)
					{
						if (t.through_loc[t.cut_through[i]] > 10)
						{
							cut_hor.push_back(t.through_loc[t.cut_through[i]]);
						}
						cut_hor.push_back(t.through_loc[t.cut_through[i] + 1]);
					}
				}
				
				std::copy(cut_hor.begin(), cut_hor.end(), cut_hor_arr);
			}

		}
		catch (const std::exception&)
		{

		}
	
	
	}
	else
	{
		list<int> cut_hor;
		cut_hor.clear();
		list<int> cut_ver;
		cut_ver.clear();
	}
	return 0;
}