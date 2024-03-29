#include "find_edges.h"
#include <cmath>

static struct stage21 s21;
static struct stage23 s23;

find_edges::find_edges(struct stage12 s12)
{
	s21.img = s12.img;
	s21.img2 = s12.img2;
	s21.mean0 = s12.mean0;
	s21.mean1 = s12.mean1;
	
	s21.main_d_0 = s12.main_d_0;
	s21.main_d_1 = s12.main_d_1;
	s21.th_edge = s12.th_edge;

	/*for (int i = 0; i < s12.img.size(); i++)
	{
		copy(s21.img0[i].begin(), s21.img0[i].end(), s21.img[i]);
	}

	for (int i = 0; i < (rows2); i++)
	{
		copy(s21.img20[i].begin(), s21.img20[i].end(), s21.img2[i]);
	}*/
}

struct FP find_edges::Find_Peaks(vector<double> arr,double th_edge)
{
	int n = arr.size();
	vector<int> stripes;
	vector<double> s_dic;
	int a = 0;
	int count = 0;

	for (int i = 1; i < n - 1; i++)
	{
		int u = i - 1;
		int v = i + 1;
		if (arr.at(i) >= arr.at(u) && arr.at(i) >= arr.at(v)) 
		{
			if (arr.at(i) > th_edge) 
			{
				stripes.push_back(i);
				s_dic.push_back(arr.at(i));
				a++;
				if (a>0) 
				{
					int w = a - 1;
					int x = a + 1;
					if (stripes.at(a) - stripes.at(w) < 25)
					{
						stripes.push_back(stripes.at(x));
						s_dic.push_back(s_dic.at(x));
						count++;
					}
				}

			}
		}
	}
	stripes.shrink_to_fit();
	s_dic.shrink_to_fit();

	struct FP peaks;
	peaks.stripes = stripes;
	peaks.s_dic = s_dic;

	return peaks;
}

vector<double> find_edges::RFFT(vector<double> x)
{
	int N = x.size();
	double* x_arr = new double[N]();
	copy(x.begin(), x.end(), x_arr);
	double* y = new double[N]();
	fftw_plan p;

	p = fftw_plan_r2r_1d(N, x_arr, y, FFTW_R2HC, FFTW_ESTIMATE);//fftw_plan_dft_1d(N, in, y, FFTW_FORWARD, FFTW_ESTIMATE);

	fftw_execute(p);
	
	fftw_destroy_plan(p);

	vector<double> yy;
	yy[0] = y[0];
	int j = 0;
	for (int i = 1; i < N; i+=2)
	{
		yy.push_back(y[i-j]);
		j++;
	}

	int k = 1;
	for (int i = 2; i < N + 1; i+=2)
	{
		yy.push_back(y[N - k]);
		k++;
	}
	
	yy.shrink_to_fit();
	delete[] y;
	return yy;
}

vector<double> find_edges::IRFFT(vector<double> x)
{
	int N = x.size();
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

	vector<double> yy;
	for (int i = 0; i < N; i++) 
	{
		y[i] /= N;
		yy.push_back(y[i]);
	}

	delete[] xx;
	yy.shrink_to_fit();
	return yy;
}

vector<double>  find_edges::Bandfilter(vector<double> x, int x0, int x1)
{
	vector<double> f_x = RFFT(x);

	vector<double> f_x_cut;
	f_x_cut = f_x;

	for (int i = 0; i < x0; i++)
	{
		f_x_cut.push_back(0);
	}

	for (int i = x1; i <= x.size(); i++)
	{
		f_x_cut.push_back(0);
	}

	vector<double> x_cut = IRFFT(f_x_cut);

	return x_cut;
}

vector<int> find_edges::ArgSort(vector<double> s_dic)
{
	vector<int> indice_arr;
	vector<double> sorted_arr;
	sorted_arr = s_dic ;

	sort(sorted_arr.begin(), sorted_arr.end());
	
	for (int i = 0; i < s_dic.size(); i++)
	{
		for (int j = 0; j < (s_dic.size()); j++)
		{
			if (sorted_arr[i] == s_dic[j])
			{
				indice_arr.push_back(j);
				break;
			}
		}

	}

	return indice_arr;
}

//int* find_edges::insertXint(int size, int* arr,int x, int pos)
//{
//	int* temp = new int[size+1];
//	std::copy(arr, arr + size, temp);
//	
//	if (pos > size)
//		return NULL;
//	if (pos == size)
//	{
//		temp[size] = x;
//	}
//	else
//	{
//		// shift elements forward 
//		for (int i = size; i > pos; i--)
//			temp[i] = temp[i - 1];
//
//		// insert x at pos 
//		temp[pos] = x;
//	} 
//	delete[] arr;
//	return temp;
//}
//
//double* find_edges::insertXdouble(int size, double* arr, double x, int pos)
//{
//	double* temp = new double[size + 1]();
//	std::copy(arr, arr + size, temp); // Suggested by comments from Nick and Bojan
//	
//	if (pos > size)
//		return NULL;
//	if (pos == size)
//	{
//		temp[size] = x;
//	}
//	else
//	{
//		// shift elements forward 
//		for (int i = size; i >= pos; i--)
//		{
//			temp[i] = temp[i - 1];
//		}
//		// insert x at pos 
//		temp[pos] = x;
//	}
//	delete[] arr;
//	return temp;
//}

double find_edges::std_dev(vector<double> arr, int start, int stop)
{
	double sum = 0.0, mean, standardDeviation = 0.0;
	int i;
	int size = stop - start;

	vector<double> B;

	for (int k = 0; k < size; k++)
	{
		const int w = k + start;
		B.push_back(arr[w]);
	}

	for (i = 0; i < size; ++i) 
	{
		sum += B.at(i);
	}

	mean = sum / size;

	for (i = 0; i < size; ++i) 
	{
		standardDeviation += pow(B.at(i) - mean, 2);
	}

	return sqrt(standardDeviation / size);
}

struct LI find_edges::Line_Index(vector<double> mean_range_in, double th_edge, int i0, int rank)
{
	double s_max, s_min;
	int s = mean_range_in.size();
	int x1 = int(s / 6);
	vector<double> mean_range;
	mean_range = Bandfilter(mean_range_in, 0, x1);

	vector<double> mean_rangeN;
	for(int i = 0; i < s; i++)
	{
		mean_rangeN.push_back(mean_range.at(i) * (-1));
	}

	struct FP peaks_max = Find_Peaks(mean_range, th_edge);
	struct FP peaks_min = Find_Peaks(mean_rangeN, -th_edge);

	if (peaks_max.stripes.size() >= 1 && rank <= peaks_max.s_dic.size())
	{
		vector<int> indice_arr;
		indice_arr = ArgSort(peaks_max.s_dic);
		int tmp = indice_arr[peaks_max.s_dic.size() - rank];
		s_max = peaks_max.stripes[tmp] + i0;
		indice_arr.clear();
		indice_arr.shrink_to_fit();
	}
	else
	{
		s_max = nan("");
	}
	if ((peaks_max.stripes.size() >= 3 && peaks_min.stripes.size() >= 3))
	{
		if (peaks_min.stripes.front() > 25)
		{
			peaks_min.stripes.insert(peaks_min.stripes.begin(), 0);
			peaks_min.s_dic.insert(peaks_min.s_dic.begin(), mean_rangeN.front());
			/*peaks_min.stripes = insertXint(peaks_min.stripe_size, peaks_min.stripes, 0, 0);
			peaks_min.s_dic = insertXdouble(peaks_min.s_dic_size, peaks_min.s_dic, mean_rangeN[0], 0);
			peaks_min.stripe_size++;
			peaks_min.s_dic_size++;*/
		}
		if (s-peaks_min.stripes.at((peaks_min.stripes.size() - 1)) > 25)
		{
			int e = s - 1;
			peaks_min.stripes.insert(peaks_min.stripes.end(), e);
			peaks_min.s_dic.insert(peaks_min.s_dic.end(), mean_rangeN.at(e));
			//peaks_min.stripes = insertXint(peaks_min.stripe_size, peaks_min.stripes, (s-1), (peaks_min.stripe_size));
			//peaks_min.s_dic = insertXdouble(peaks_min.s_dic_size, peaks_min.s_dic, mean_rangeN[(s - 1)], (peaks_min.s_dic_size));
			//peaks_min.stripe_size++;
			//peaks_min.s_dic_size++;
		}

		const int s_dic_min_size = (peaks_min.stripes.size()) - 2;
		vector<double> s_dic_min;
		for (int i = 0; i < s_dic_min_size; i++)
		{
			if (i < s_dic_min_size)
			{
				int e = i + 1;
				s_dic_min.push_back(peaks_min.s_dic.at(e));
			}
			
		}

		int n_0 = (max_element(s_dic_min.begin(), s_dic_min.begin() + s_dic_min_size) - s_dic_min.begin());
		int h = n_0 + 1;
		s_min = peaks_min.stripes.at(h) + i0;
	}
	else
	{
		s_min = nan("");
	}

	struct LI index;
	index.s_max = s_max;
	index.s_min = s_min;
	
	return index;
}

vector<int> find_edges::decumulateInt(vector<int> x)
{
	const size_t n = x.size() - 1;
	vector<int> xi;
	vector<int> x1;
	vector<int> x2;

	for (size_t i = 0; i < x.size() - 1; i++)
	{
		if (i < n)
		{
			x1.push_back(x.at(i + 1));
		}
		
	}
	for (int i = 0; i < n; i++)
	{
		x2[i] = x[i];
	}

	for (int i = 0; i < n; i++)
	{
		xi.push_back(x1.at(i) - x2.at(i));
	}

	x1.clear();
	x2.clear();
	x1.shrink_to_fit();
	x2.shrink_to_fit();
	xi.shrink_to_fit();
	
	return xi;
}

struct DT find_edges::Detect_Through(vector<double> im_col, double th_edge)
{
	int size = im_col.size();
	vector<double> im_diff;
	for (int i = 0; i < size; i++)
	{
		im_diff.push_back(im_col.at(i) - th_edge);
	}
	
	int n = size - 1;
	vector<bool> signbit;
	for(int i = 0; i < size; i++)
	{ 
		signbit[i] = !(std::signbit(im_diff.at(i)));
	}
	
	vector<bool> th_through;
	vector<int>  through_loc;
	int count = 0;
	for (int i = 0; i < n; i++)
	{
		int h = i + 1;
		if (signbit.at(i) == signbit.at(h))
		{
			th_through.at(i) = false;
		}
		else 
		{
			th_through.at(i) = true;
		}

		if (th_through.at(i) == true)
		{
			through_loc[count] = i;
			count++  ;
		}
	}

	/*int* through_loc = new int[count]();
	std::copy(through_loc1, through_loc1 + count, through_loc);*/
	
	through_loc.insert(through_loc.begin(), 0);
	
	through_loc.insert(through_loc.end(), size);

	vector<int> d_through;
	d_through = decumulateInt(through_loc);

	vector<int> cut_through;
	for (int i = 0; i < (d_through.size()); i++)
	{
		if (d_through[i] > 33)
		{
			cut_through.push_back(i); 
		}
	}

	through_loc.shrink_to_fit();
	cut_through.shrink_to_fit();

	/*int* cut_through = new int[j]();
	std::copy(cut_through1, cut_through1 + j, cut_through);
	*/
	struct DT thro;
	thro.through_loc = through_loc;
	thro.cut_through = cut_through;

	im_diff.clear();
	signbit.clear();
	th_through.clear();
	im_diff.shrink_to_fit();
	signbit.shrink_to_fit();
	th_through.shrink_to_fit();

	return thro;
}

vector<int> find_edges::deleteXint(vector<int> arr, int pos)
{
	int size = arr.size();

	if (pos > size)
		return arr;
	
	for (int i = size; i > pos; i--)
	{
		int a = i - 2;
		int b = i - 1;
		arr.at(a) = arr.at(b);
	}
	
	return arr;
}

double IntMeanR(vector<int> mean0)
{
	int sum = 0;
	double meanR = 0.0;
	for (int i = 0; i < mean0.size(); i++)
	{
		sum += mean0.at(i);
	}

	meanR = (double)sum / (double)mean0.size();
	return meanR;
}

int op_subtract(int x, int ideal_d) 
{
	x = x - ideal_d;
	return x;
}

//to do
list<int> find_edges::Delete_Edges(vector<int> cut_arr, int ideal_d)
{
	list<int>cut_list;
	for (int i_cut = (cut_arr.size() - 1); i_cut >= 0; i_cut--)
	{
		for (int j = 0; j < cut_arr.size(); j++)
		{
			bool c1 = abs(cut_arr.at(j) - cut_arr.at(i_cut)) > (ideal_d - 40);
			bool c2 = abs(cut_arr.at(j) - cut_arr.at(i_cut)) > (ideal_d + 40);
			bool c = (!(c1 && c2));

			if (c)
			{
				cut_arr = deleteXint( cut_arr,i_cut);
			}
		}
	}

	vector<int> cut_ver_de = decumulateInt(cut_arr);
	vector<int> close_edges;

	for (int i = 0; i < cut_arr.size(); i++)
	{
		if (cut_ver_de[i] < ideal_d - 20)
		{
			close_edges.push_back(i);
		}
	}

	/*int* close_edges = new int[p];
	std::copy(close_edges1, close_edges1 + p, close_edges);*/
	for (int i_close = (close_edges.size() - 1); i_close >= 0; i_close--)
	{
		list<int>d_cut_ver_0; 
		list<int> d_cut_ver_1;
		d_cut_ver_0.clear();
		d_cut_ver_1.clear();

		int vl = close_edges.at(i_close);
		int vl1 = vl - 1;
		int vl2 = vl + 1;
		int vl3 = vl + 2;
		if (close_edges.at(i_close)>0)
		{
			int x0 = abs(cut_arr.at(vl) - cut_arr.at(vl1));
			int x1 = abs(cut_arr.at(vl2) - cut_arr.at(vl1));
			d_cut_ver_0.push_back(x0);
			d_cut_ver_1.push_back(x1);
		}
		if (close_edges.at(i_close) < close_edges.size() - 2);
		{
			int x0 = abs(cut_arr.at(vl) - cut_arr.at(vl3));
			int x1 = abs(cut_arr.at(vl2) - cut_arr.at(vl3));
			d_cut_ver_0.push_back(x0);
			d_cut_ver_1.push_back(x1);
		}

		vector<int> ver0;
		vector<int> ver01;
		copy(d_cut_ver_0.begin(), d_cut_ver_0.end(), ver01.begin());
		//transform(ver01.begin(), ver01.end(), ver0.begin(), op_subtract);
		for (int i = 0; i < d_cut_ver_0.size(); i++)
		{
			ver0[i] = ver01[i] - ideal_d;
		}
		double d_m_0 = abs(IntMeanR(ver0));

		/*int* ver11 = new int[d_cut_ver_1.size()];
		int* ver1 = new int[d_cut_ver_1.size()];
		copy(d_cut_ver_1.begin(), d_cut_ver_1.end(), ver11);

		*/
		vector<int> ver1;
		vector<int> ver11;
		copy(d_cut_ver_1.begin(), d_cut_ver_1.end(), ver11.begin());
		for (int i = 0; i < d_cut_ver_0.size(); i++)
		{
			ver1[i] = ver11[i] - ideal_d;
		}
		//transform(ver11.begin(), ver11.end(), ver1.begin(), op_subtract);
		double d_m_1 = abs(IntMeanR(ver1));

		if (d_m_0 > d_m_1)
		{
			cut_arr = deleteXint(cut_arr, close_edges[i_close]);
		}
		else
		{
			cut_arr = deleteXint(cut_arr, close_edges[i_close+1]);
		}
	}

	
	for (int i = 0; i < cut_arr.size(); i++)
	{
		cut_list.push_back(cut_arr[i]);
	}

	//delete[] close_edges1;
	return cut_list;
}

struct stage23 find_edges::Execute(void)
{
	list<int> cut_hor;
	list<int> cut_ver;
	if ((s21.main_d_0 > 13) && (s21.main_d_1 > 13))
	{	
		double s_max, s_min;
		//int size_mean0 = s21.imgCols / 2;
		const int mid = s21.mean0.size() / 2;
		const int search_range = 150;
		int i0 = mid - search_range;
		int i1 = mid + search_range;
		int R = i1 - i0;

		vector<double> mean_range0;
		for (int i = i0; i <= i1; i++)
		{
			int a = i - i0;
			mean_range0.at(a) = s21.mean0.at(i);
		}
		int rank = 0;
		int len = s21.img2.size();
		vector<double> im_col;
		while (rank < 5)
		{
			rank += 1;
			struct LI index = Line_Index(mean_range0, s21.th_edge, i0, rank);
			s_max = index.s_max;
			int s_m = (int) s_max;
			bool res = isnan(s_max);
			if (!res)
			{
				int x = s21.img2.size() / 6;
				vector<double> img_col;
				for (int i = 0; i < im_col.size(); i++)
				{
					img_col.push_back((double)s21.img2[i][s_m]);
				}
				
				im_col = Bandfilter(img_col, 0, x);

				int n1 = len - 150;
				vector<double> std_col;
				for (int i = 0; i < n1; i ++)
				{
					double tmp = std_dev(im_col, i, i + 150);
					std_col.push_back(tmp);
				}
				double c1 = *min_element(std_col.begin(), std_col.begin() + n1);
				double c2 = (*max_element(im_col.begin(), im_col.begin() + len)) - (*min_element(im_col.begin(), im_col.begin() + len));
				double condition1 = c1 / c2;
				if (condition1 <= 0.085)
				{
					break;
				}
			}
	
		}
		try
		{
			cut_hor.clear();
			vector<int> cut_hor_arr;
			if (rank != 5)
			{
				struct DT t = Detect_Through(im_col, s21.th_edge);

				for (int i = 0; i < t.cut_through.size(); i++)
				{
					int a = t.through_loc.at(t.cut_through.at(i)) + 1;
					int b1 = t.cut_through.at(i) + 1;
					int b = t.through_loc.at(b1);
					int c = t.through_loc.at(t.cut_through.at(i)) + 1;
					if ((t.through_loc[t.cut_through.at(i)] == 0) && (im_col.at(a) > s21.th_edge))
					{
						cut_hor.push_back(b);
					}
					else if (t.cut_through[i] == (t.through_loc.size() - 2))
					{
						cut_hor.push_back(t.through_loc.at(t.cut_through.at(i)));
					}
					else if (im_col.at(c) > s21.th_edge)
					{
						if (t.through_loc[t.cut_through.at(i)] > 10)
						{
							cut_hor.push_back(t.through_loc.at(t.cut_through.at(i)));
						}
						cut_hor.push_back(b);
					}
				}
				//std::copy(cut_hor.begin(), cut_hor.end(), cut_hor_arr);
				//to do
				//cut_hor = Delete_Edges(cut_hor_arr, 300, cut_hor.size());
			}

		}
		catch (const std::out_of_range& err)
		{
			cut_hor.clear();
		}
		if (cut_hor.size() >= 2)
		{
			list<int>::iterator it = cut_hor.begin();
			advance(it, 1);
			int j0 = cut_hor.front();
			int j1 = *it;
			int s1 = j1 - j0;
			vector<double> mean_range1;
			for (int i = j0; i <= j1; i++)
			{
					mean_range1.push_back(s21.mean1.at(i));			
			}
			struct LI index = Line_Index(mean_range1,s21.th_edge, j0, rank=1);
			s_max = index.s_max;
			s_min = index.s_min;
			int s_m = (int)s_max;
			int s_mi = (int)s_min;

			try
			{
				int y = s21.img2[0].size() / 6;
				int wid = s21.img2[0].size();
				vector<double> img_row_l;
				for (int i = 0; i < wid; i++)
				{
					img_row_l.push_back((double)s21.img2[s_mi][i]);
				}

				vector<double> im_row_low;
				im_row_low = Bandfilter(img_row_l, 0, y);

				int n2 = wid - 200;
				vector<double> std_row;
				for (int i = 0; i < n2; i++)
				{
					double tmp = std_dev(im_row_low, i, i + 200);
					std_row.push_back(tmp);
				}
				double cc1 = *min_element(std_row.begin(), std_row.begin() + n2);
				double cc2 = (*max_element(im_row_low.begin(), im_row_low.begin() + wid)) - (*min_element(im_row_low.begin(), im_row_low.begin() + wid));
				double condition2 = cc1 / cc2;
				if (condition2 <= 0.085)
				{
					struct DT t = Detect_Through(im_row_low, s21.th_edge);

					for (int i_row_l = 0; i_row_l < t.cut_through.size(); i_row_l++)
					{
						if (t.through_loc.at(t.cut_through.at(i_row_l))  != 0)
						{
							cut_ver.push_back(t.through_loc.at(t.cut_through.at(i_row_l)));
						}
					}
				}

				vector<double> img_row;
				for (int i = 0; i < wid; i++)
				{
					img_row.push_back((double)s21.img2[s_m][i]);
				}

				vector<double> im_row;
				im_row = Bandfilter(img_row, 0, y);

				int n3 = wid - 200;
				vector<double> std_row_h;
				for (int i = 0; i < n3; i++)
				{
					double tmp = std_dev(im_row, i, i + 200);
					std_row_h.push_back(tmp);
				}

				double c11 = *min_element(std_row_h.begin(), std_row_h.begin() + n2);
				double c22 = (*max_element(im_row.begin(), im_row.begin() + wid)) - (*min_element(im_row.begin(), im_row.begin() + wid));
				double condition3 = c11 / c22;
				if (condition3 <= 0.088)
				{
					struct DT t1 = Detect_Through(im_row, s21.th_edge);

					for (int i_row_h = 0; i_row_h < t1.cut_through.size(); i_row_h++)
					{
						if (t1.through_loc.at(t1.cut_through.at(i_row_h)) != 0)
						{
							int m = t1.cut_through.at(i_row_h) + 1;
							cut_ver.push_back(t1.through_loc.at(m));
						}
					}
				}
				cut_ver.sort();
				//to do
				/*int* cut_ver_arr = new int[cut_ver.size()]();
				std::copy(cut_ver.begin(), cut_ver.end(), cut_ver_arr);*/
				//cut_ver = Delete_Edges(cut_ver_arr, 300, cut_ver.size());

			}
			catch (const std::out_of_range &err)
			{
				cut_ver.clear();
			}
		}
		else
		{
			cut_ver.clear();
		}
	}
	else
	{
		cut_hor.clear();
		cut_ver.clear();
	}

	s23.cut_hor.assign(cut_hor.begin(), cut_hor.end());
	s23.cut_ver.assign(cut_ver.begin(), cut_ver.end());

	s23.img = s21.img;
	s23.img2 = s21.img2;
	/*for (int i = 0; i < s21.imgRows; i++)
	{
		memcpy(s23.img[i], s21.img[i], (s21.imgCols * sizeof(int)));
	}

	for (int i = 0; i < (rows2); i++)
	{
		memcpy(s23.img2[i], s21.img2[i], (cols2 * sizeof(int)));
	}
	*/
	
	return s23;
}