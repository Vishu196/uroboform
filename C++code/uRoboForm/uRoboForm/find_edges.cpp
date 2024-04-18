#include "find_edges.h"
#include <cmath>

using namespace std;

std::ostream& operator<<(std::ostream& ostr, const stage23& s23)
{
	ostr << "cut_hor: ";
	for (auto v : s23.cut_hor)
		ostr << v << ",";
	ostr << endl << "cut_ver: ";
	for (auto v : s23.cut_ver)
		ostr << v << ",";
	ostr << endl;
	ostr << "Stage 2 complete." << endl;
	return ostr;
}

peaks find_edges::Find_Peaks(const vector<double> &arr,double th_edge)
{
	size_t n = arr.size();
	vector<int> stripes(50);
	vector<double> s_dic(80);
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

	peaks peaks;
	peaks.stripes = stripes;
	peaks.s_dic = s_dic;

	return peaks;
}

indexes find_edges::Line_Index(const vector<double>& mean_range_in, double th_edge, int i0, int rank)
{
	indexes index;

	index.s_max, index.s_min;
	size_t s = mean_range_in.size();
	int x1 = int(s / 6);
	vector<double> mean_range(s);
	mean_range = signal_evaluation::Bandfilter(mean_range_in, 0, x1);

	vector<double> mean_rangeN(s);
	
	for (int i = 0; i < s; i++)
	{
		mean_rangeN[i] = mean_range[i] * (-1);
	}

	peaks peaks_max = Find_Peaks(mean_range, th_edge);
	peaks peaks_min = Find_Peaks(mean_rangeN, -th_edge);

	if (peaks_max.stripes.size() >= 1 && rank <= peaks_max.s_dic.size())
	{
		vector<int> indice_arr = Evaluation::ArgSort(peaks_max.s_dic);
		int tmp = indice_arr[peaks_max.s_dic.size() - rank];
		index.s_max = peaks_max.stripes[tmp] + i0;
	}
	else
	{
		index.s_max = nan("");
	}
	if ((peaks_max.stripes.size() >= 3 && peaks_min.stripes.size() >= 3))
	{
		if (peaks_min.stripes.front() > 25)
		{
			peaks_min.stripes.insert(peaks_min.stripes.begin(), 0);
			peaks_min.s_dic.insert(peaks_min.s_dic.begin(), mean_rangeN.front());
		}
		if (s - peaks_min.stripes.at((peaks_min.stripes.size() - 1)) > 25)
		{
			size_t e = s - 1;
			peaks_min.stripes.insert(peaks_min.stripes.end(), (int)e);
			peaks_min.s_dic.insert(peaks_min.s_dic.end(), mean_rangeN.at(e));
		}

		const int s_dic_min_size = ((int)peaks_min.stripes.size()) - 2;
		vector<double> s_dic_min(s_dic_min_size);
		for (auto i = 0; i < s_dic_min_size; i++)
		{
			s_dic_min[i] = peaks_min.s_dic[i + 1]; 
		}

		int n_0 = (int)(max_element(s_dic_min.begin(), s_dic_min.end()) - s_dic_min.begin());
		int h = n_0 + 1;
		index.s_min = peaks_min.stripes[h] + i0;
	}
	else
	{
		index.s_min = nan("");
	}

	return index;
}

Detect_throu find_edges::Detect_Through(const vector<double> &im_col, double th_edge)
{
	Detect_throu thro;

	const int conditionValue = 33;
	size_t size = im_col.size();
	thro.through_loc.reserve(size / 2);

	vector<bool> signbit(size);
	for(int i = 0; i < size; i++)
	{ 
		signbit[i] = !(std::signbit(im_col[i] - th_edge));
	}
	
	thro.through_loc.push_back(0);
	const size_t n = size - 1;
	for (int i = 0; i < n; i++)
	{
		int h = i + 1;
		if (signbit[i] != signbit[h])
		{
			thro.through_loc.push_back(i);
		}
	}

	thro.through_loc.push_back(size);

	vector<int> d_through;
	d_through = Evaluation::decumulate(thro.through_loc);

	thro.cut_through.reserve(d_through.size()); // Reserve space upfront

	for (int i = 0; i < (d_through.size()); i++)
	{
		if (d_through[i] > conditionValue)
		{
			thro.cut_through.push_back(i);
		}
	}

	return thro;
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
				cut_arr.erase(cut_arr.begin() + i_cut);
			}
		}
	}

	vector<int> cut_ver_de = Evaluation::decumulate(cut_arr);
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
		if (close_edges.at(i_close) < (close_edges.size() - 2))
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
		double d_m_0 = abs(Evaluation::MeanR(ver0));

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
		double d_m_1 = abs(Evaluation::MeanR(ver1));

		if (d_m_0 > d_m_1)
		{
			cut_arr.erase(cut_arr.begin() + close_edges[i_close]);
		}
		else
		{
			cut_arr.erase(cut_arr.begin() + close_edges[i_close+1]);
		}
	}

	for (int i = 0; i < cut_arr.size(); i++)
	{
		cut_list.push_back(cut_arr[i]);
	}

	//delete[] close_edges1;
	return cut_list;
}

void find_edges::Execute(const stage12 &s12)
{
	stage23 s23;

	if ((s12.main_d_0 > 13) && (s12.main_d_1 > 13))
	{	
		double s_max, s_min;
		const size_t mid = s12.mean0.size() / 2;
		const size_t i0 = mid - search_range;
		const size_t i1 = mid + search_range;
		const size_t R = i1 - i0;

		vector<double> mean_range0(s12.mean0.begin() + i0, s12.mean0.begin() + i1);

		int rank = 0;
		size_t len = s12.img2.rows;
		vector<double> im_col(len);
		while (rank < 5)
		{
			rank += 1;
			indexes index = Line_Index(mean_range0, s12.th_edge, (int)i0, rank);
			s_max = index.s_max;
			int s_m = (int) s_max;
			bool res = isnan(s_max);
			if (!res)
			{
				size_t x = s12.img2.rows / 6;
				vector<double> img_col(len);
				for (int i = 0; i < len; i++)
				{
					img_col[i] = (double)s12.img2.data[i * s12.img2.step + s_m];
				}
				
				im_col = signal_evaluation::Bandfilter(img_col, 0, x);

				size_t n1 = len - 150;
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
			s23.cut_hor.clear();
			vector<int> cut_hor_arr;
			if (rank != 5)
			{
				Detect_throu t = Detect_Through(im_col, s12.th_edge);

				for (int i = 0; i < t.cut_through.size(); i++)
				{
					int a = t.through_loc[t.cut_through[i]] + 1;
					int b1 = t.cut_through[i] + 1;
					int b = t.through_loc[i];
					int c = t.through_loc[t.cut_through[i]] + 1;
					if ((t.through_loc[t.cut_through[i]] == 0) && (im_col[a] > s12.th_edge))
					{
						s23.cut_hor.push_back(t.through_loc[b1]);
					}
					else if (t.cut_through[i] == (t.through_loc.size() - 2))
					{
						s23.cut_hor.push_back(t.through_loc[t.cut_through[i]]);
					}
					else if (im_col[c] > s12.th_edge)
					{
						if (t.through_loc[t.cut_through[i]] > 10)
						{
							s23.cut_hor.push_back(t.through_loc[t.cut_through[i]]);
						}
						s23.cut_hor.push_back(t.through_loc[b1]);
					}
				}
				//copy(cut_hor.begin(), cut_hor.end(), cut_hor_arr);
				////to do
				//cut_hor = Delete_Edges(cut_hor_arr, 300);
			}

		}
		catch (const std::out_of_range)
		{
			s23.cut_hor.clear();
		}
		if (s23.cut_hor.size() >= 2)
		{
			/*list<int>::iterator it = s23.cut_hor.begin();
			advance(it, 1);
			int j0 = s23.cut_hor.front();
			int j1 = *it;*/
			int j0 = s23.cut_hor.front();
			int s1 = s23.cut_hor[1] - s23.cut_hor[0];
			int k = 0;
			vector<double> mean_range1(s1);
			for (int i = j0; i < s23.cut_hor[1]; i++)
			{
				mean_range1[k] = s12.mean1[i];
				k++;
			}
			indexes index = Line_Index(mean_range1,s12.th_edge, j0, rank=1);
			s_max = index.s_max;
			s_min = index.s_min;
			int s_m = (int)s_max;
			int s_mi = (int)s_min;

			try
			{
				size_t y = s12.img2.cols / 6;
				size_t wid = s12.img2.cols;
				vector<double> img_row_l(wid);
				for (int i = 0; i < wid; i++)
				{
					img_row_l[i] = (double)s12.img2.data[s_mi * s12.img2.step + i];
				}

				vector<double> im_row_low(wid);
				im_row_low = signal_evaluation::Bandfilter(img_row_l, 0, y);

				size_t n2 = wid - 200;
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
					Detect_throu t = Detect_Through(im_row_low, s12.th_edge);

					for (int i_row_l = 0; i_row_l < t.cut_through.size(); i_row_l++)
					{
						if (t.through_loc.at(t.cut_through.at(i_row_l))  != 0)
						{
							s23.cut_ver.push_back(t.through_loc.at(t.cut_through.at(i_row_l)));
						}
					}
				}

				vector<double> img_row;
				for (int i = 0; i < wid; i++)
				{
					img_row.push_back((double)s12.img2.data[s_m * s12.img2.step + i]);
				}

				vector<double> im_row;
				im_row = signal_evaluation::Bandfilter(img_row, 0, y);

				size_t n3 = wid - 200;
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
					Detect_throu t1 = Detect_Through(im_row, s12.th_edge);

					for (int i_row_h = 0; i_row_h < t1.cut_through.size(); i_row_h++)
					{
						if (t1.through_loc.at(t1.cut_through.at(i_row_h)+1) != im_row.size())
						{
							int m = t1.cut_through.at(i_row_h) + 1;
							s23.cut_ver.push_back(t1.through_loc.at(m));
						}
					}
				}
				sort(s23.cut_ver.begin(), s23.cut_ver.end());
				//s23.cut_ver.sort();
				//to do
				/*vector<int> cut_ver_arr;
				copy(cut_ver.begin(), cut_ver.end(), cut_ver_arr);
				cut_ver = Delete_Edges(cut_ver_arr, 300);*/

			}
			catch (const std::out_of_range)
			{
				s23.cut_ver.clear();
			}
		}
		else
		{
			s23.cut_ver.clear();
		}
	}
	else
	{
		s23.cut_hor.clear();
		s23.cut_ver.clear();
	}

	s23.img = s12.img.clone();
	s23.img2 = s12.img2.clone();

	fifo.push(s23);
	cout << s23;
}
