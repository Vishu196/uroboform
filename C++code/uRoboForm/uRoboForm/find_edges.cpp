#include "find_edges.h"
#include <cmath>

static struct stage21 s21;
static struct stage23 s23;

find_edges::find_edges(struct stage12 s12)
{
	s21.img = s12.img.clone();
	s21.img2 = s12.img2.clone();
	s21.mean0 = s12.mean0;
	s21.mean1 = s12.mean1;
	
	s21.main_d_0 = s12.main_d_0;
	s21.main_d_1 = s12.main_d_1;
	s21.th_edge = s12.th_edge;

}

struct FP find_edges::Find_Peaks(const vector<double> &arr,double th_edge)
{
	int n = (int)arr.size();
	vector<int> stripes(n);
	vector<double> s_dic(n);
	int a = 0;
	int count = 0;

	for (int i = 1; i < n - 1; i++)
	{
		int u = i - 1;
		int v = i + 1;
		if (arr[i] >= arr[u] && arr[i] >= arr[v])
		{
			if (arr[i] > th_edge)
			{
				stripes[a] = i;
				s_dic[a] = arr[i];
				a++;
				if (a>0) 
				{
					int p = a - 1;
					int w = a - 2;
					int x = a + 1;
					if (stripes[a] - stripes[p] < 25)
					{
						stripes[a] = stripes[x];
						s_dic[a] = s_dic[x];
						count++;
					}
				}

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

struct LI find_edges::Line_Index(const vector<double> &mean_range_in, double th_edge, int i0, int rank)
{
	double s_max, s_min;
	int s = (int)mean_range_in.size();
	int x1 = int(s / 6);
	vector<double> mean_range(s);
	mean_range = signal_evaluation::Bandfilter(mean_range_in, 0, x1);

	vector<double> mean_rangeN(s);
	for(int i = 0; i < s; i++)
	{
		mean_rangeN[i] = mean_range[i] * (-1);
	}

	struct FP peaks_max = Find_Peaks(mean_range, th_edge);
	struct FP peaks_min = Find_Peaks(mean_rangeN, -th_edge);

	if (peaks_max.stripes.size() >= 1 && rank <= peaks_max.s_dic.size())
	{
		vector<int> indice_arr(peaks_max.s_dic.size());
		indice_arr = Evaluation::ArgSort(peaks_max.s_dic);
		int tmp = indice_arr[peaks_max.s_dic.size() - rank];
		s_max = peaks_max.stripes[tmp] + i0;
		/*indice_arr.clear();
		indice_arr.shrink_to_fit();*/
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
		}
		if (s-peaks_min.stripes.at((peaks_min.stripes.size() - 1)) > 25)
		{
			int e = s - 1;
			peaks_min.stripes.insert(peaks_min.stripes.end(), e);
			peaks_min.s_dic.insert(peaks_min.s_dic.end(), mean_rangeN.at(e));
		}

		const int s_dic_min_size = ((int)peaks_min.stripes.size()) - 2;
		vector<double> s_dic_min(s_dic_min_size);
		for (int i = 0; i < s_dic_min_size; i++)
		{
			if (i < s_dic_min_size)
			{
				int e = i + 1;
				s_dic_min[i] = peaks_min.s_dic.at(e);
			}
			
		}

		int n_0 = (int)(max_element(s_dic_min.begin(), s_dic_min.begin() + s_dic_min_size) - s_dic_min.begin());
		int h = n_0 + 1;
		s_min = peaks_min.stripes.at(h) + i0;

		/*s_dic_min.clear();
		s_dic_min.shrink_to_fit();*/
	}
	else
	{
		s_min = nan("");
	}

	struct LI index;
	index.s_max = s_max;
	index.s_min = s_min;
	
	/*mean_range.clear();
	mean_range.shrink_to_fit();
	mean_rangeN.clear();
	mean_rangeN.shrink_to_fit();*/
	
	return index;
}
 
struct DT find_edges::Detect_Through(const vector<double> &im_col, double th_edge)
{
	int size = (int)im_col.size();
	vector<double> im_diff(size);
	for (int i = 0; i < size; i++)
	{
		im_diff[i] = im_col[i] - th_edge;
	}
	
	int n = size - 1;
	vector<bool> signbit(size);
	for(int i = 0; i < size; i++)
	{ 
		signbit[i] = !(std::signbit(im_diff[i]));
	}
	
	vector<bool> th_through(size);
	vector<int> through_loc;
	int count = 0;
	for (int i = 0; i < n; i++)
	{
		int h = i + 1;
		if (signbit[i] == signbit[h])
		{
			th_through[i] = false;
		}
		else 
		{
			th_through[i] = true;
		}

		if (th_through[i] == true)
		{
			through_loc.push_back(i);
			count++  ;
		}
	}

	//th_through.shrink_to_fit();
	
	through_loc.insert(through_loc.begin(), 0);
	
	through_loc.insert(through_loc.end(), size);

	vector<int> d_through;
	d_through = Evaluation::decumulateInt(through_loc);

	vector<int> cut_through;
	for (int i = 0; i < (d_through.size()); i++)
	{
		if (d_through[i] > 33)
		{
			cut_through.push_back(i); 
		}
	}

	/*through_loc.shrink_to_fit();
	cut_through.shrink_to_fit();*/

	struct DT thro;
	thro.through_loc = through_loc;
	thro.cut_through = cut_through;

	/*im_diff.clear();
	signbit.clear();
	th_through.clear();
	im_diff.shrink_to_fit();
	signbit.shrink_to_fit();
	th_through.shrink_to_fit();*/

	return thro;
}

vector<int> find_edges::deleteXint(vector<int> arr, int pos)
{
	int size = (int)arr.size();

	if (pos > size)
		return arr;
	
	for (int i = size; i > pos; i--)
	{
		int a = i - 2;
		int b = i - 1;
		arr[a] = arr[b];
	}
	
	return arr;
}

double IntMeanR(const vector<int> &mean0)
{
	int sum = 0;
	double meanR = 0.0;
	for (int i = 0; i < mean0.size(); i++)
	{
		sum += mean0[i];
	}

	meanR = (double)sum / (double)mean0.size();
	return meanR;
}

//to do
list<int> find_edges::Delete_Edges(vector<int> cut_arr, int ideal_d)
{
	list<int>cut_list;
	for (int i_cut = ((int)cut_arr.size() - 1); i_cut >= 0; i_cut--)
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

	vector<int> cut_ver_de = Evaluation::decumulateInt(cut_arr);
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
	for (int i_close = ((int) close_edges.size() - 1); i_close >= 0; i_close--)
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
		if (close_edges.at(i_close) < (close_edges.size() - 2));
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
		const int mid = (int)s21.mean0.size() / 2;
		const int search_range = 150;
		int i0 = mid - search_range;
		int i1 = mid + search_range;
		int R = i1 - i0;

		vector<double> mean_range0(R);
		for (int i = i0; i < i1; i++)
		{
			int a = i - i0;
			mean_range0[a] = s21.mean0[i];
		}
		
		int rank = 0;
		int len = (int)s21.img2.rows;
		vector<double> im_col(len);
		while (rank < 5)
		{
			rank += 1;
			struct LI index = Line_Index(mean_range0, s21.th_edge, i0, rank);
			s_max = index.s_max;
			int s_m = (int) s_max;
			bool res = isnan(s_max);
			if (!res)
			{
				int x = (int)s21.img2.rows / 6;
				vector<double> img_col(len);
				for (int i = 0; i < len; i++)
				{
					img_col[i] = (double)s21.img2.data[i * s21.img2.step + s_m];
				}
				
				im_col = signal_evaluation::Bandfilter(img_col, 0, x);

				int n1 = len - 150;
				vector<double> std_col(n1);
				for (int i = 0; i < n1; i ++)
				{
					double tmp = Evaluation::std_dev(im_col, i, i + 150);
					std_col[i] = tmp;
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
					int a = t.through_loc[t.cut_through[i]] + 1;
					int b1 = t.cut_through[i] + 1;
					int b = t.through_loc[i];
					int c = t.through_loc[t.cut_through[i]] + 1;
					if ((t.through_loc[t.cut_through[i]] == 0) && (im_col[a] > s21.th_edge))
					{
						cut_hor.push_back(t.through_loc[b1]);
					}
					else if (t.cut_through[i] == (t.through_loc.size() - 2))
					{
						cut_hor.push_back(t.through_loc[t.cut_through[i]]);
					}
					else if (im_col[c] > s21.th_edge)
					{
						if (t.through_loc[t.cut_through[i]] > 10)
						{
							cut_hor.push_back(t.through_loc[t.cut_through[i]]);
						}
						cut_hor.push_back(t.through_loc[b1]);
					}
				}
				//copy(cut_hor.begin(), cut_hor.end(), cut_hor_arr);
				////to do
				//cut_hor = Delete_Edges(cut_hor_arr, 300);
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
			int k = 0;
			vector<double> mean_range1(s1);
			for (int i = j0; i < j1; i++)
			{
				mean_range1[k] = s21.mean1[i];
				k++;
			}
			struct LI index = Line_Index(mean_range1,s21.th_edge, j0, rank=1);
			s_max = index.s_max;
			s_min = index.s_min;
			int s_m = (int)s_max;
			int s_mi = (int)s_min;

			try
			{
				int y = (int)s21.img2.cols / 6;
				int wid = (int)s21.img2.cols;
				vector<double> img_row_l(wid);
				for (int i = 0; i < wid; i++)
				{
					img_row_l[i] = (double)s21.img2.data[s_mi * s21.img2.step + i];
				}

				vector<double> im_row_low(wid);
				im_row_low = signal_evaluation::Bandfilter(img_row_l, 0, y);

				int n2 = wid - 200;
				vector<double> std_row(n2);
				for (int i = 0; i < n2; i++)
				{
					double tmp = Evaluation::std_dev(im_row_low, i, i + 200);
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
					img_row.push_back((double)s21.img2.data[s_m * s21.img2.step + i]);
				}

				vector<double> im_row;
				im_row = signal_evaluation::Bandfilter(img_row, 0, y);

				int n3 = wid - 200;
				vector<double> std_row_h;
				for (int i = 0; i < n3; i++)
				{
					double tmp = Evaluation::std_dev(im_row, i, i + 200);
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
				/*vector<int> cut_ver_arr;
				copy(cut_ver.begin(), cut_ver.end(), cut_ver_arr);
				cut_ver = Delete_Edges(cut_ver_arr, 300);*/

			}
			catch (const std::out_of_range& err)
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

	s23.img = s21.img.clone();
	s23.img2 = s21.img2.clone();
	
	return s23;
}