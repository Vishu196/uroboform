#include "grid_pos03.h"

using namespace std;
using namespace cv;

std::ostream& operator<<(std::ostream& ostr, const stage56& s56)
{
	cout << "k:" << s56.k << endl;
	cout << "xi: " << s56.xi << endl;
	cout << "zi: " << s56.zi << endl;
	cout << "grid_pos03 complete." << endl;
	return ostr;
}

double grid_pos03::calc_d_k(vector<vector <double>> lines)
{
	int n = (int)lines.size();
	const int n1 = n - 1;
	const int n2 = n - 2;
	double line_0, line_n;
	if (lines[1][0] == lines[0][0])
	{
		vector<double> line_arr(2);
		line_arr[0] = lines[0][1];
		line_arr[1] = lines[1][1];
		line_0 = Evaluation::MeanR(line_arr);
	}
	else
	{
		line_0 = lines[0][1];
	}

	if (lines[n1][0] == lines[n2][0])
	{
		vector<double> line_arr(2);
		line_arr[0] = lines[n1][1];
		line_arr[1] = lines[n2][1];
		line_n = Evaluation::MeanR(line_arr);
	}
	else
	{
		line_n = lines[lines.size() - 1][1];
	}
	return (line_n - line_0) / ((lines[n1][0] - lines[0][0]) / 200);
}

double grid_pos03::get_d_k(Grid**& cgrids, int gRows, int gCols)
{
	vector<vector<double>>lines_hor;
	vector<vector<double>> lines_ver;
	for (int row = 0; row < gRows; row++)
	{
		for (int col = 0; col < gCols; col++)
		{
			Grid field = cgrids[row][col];

			if (field.max_pos.size() >= 1)
			{
				if (field.orientation == "hor")
				{
					vector<double> h1;
					h1.push_back(grid_pos02::get_mask_pos(field, row, col, 0));
					h1.push_back(field.max_pos.front());
					lines_hor.push_back(h1);

					vector<double> h2;
					h2.push_back(grid_pos02::get_mask_pos(field, row, col, field.max_pos.size() - 1));
					h2.push_back(field.max_pos.back());
					lines_hor.push_back(h2);
				}
				else
				{
					vector<double> v1;
					v1.push_back(grid_pos02::get_mask_pos(field, row, col, 0));
					v1.push_back(field.max_pos.front());
					lines_ver.push_back(v1);

					vector<double> v2;
					v2.push_back(grid_pos02::get_mask_pos(field, row, col, field.max_pos.size() - 1));
					v2.push_back(field.max_pos.back());
					lines_ver.push_back(v2);
				}
			}
		}
	}
	double d_k_mean = 0;

	if ((lines_ver.size() >= 3) && (lines_hor.size() >= 3))
	{
		sort(lines_hor.begin(), lines_hor.end());
		sort(lines_ver.begin(), lines_ver.end());
		double d_k_hor = calc_d_k(lines_hor);
		double d_k_ver = calc_d_k(lines_ver);
		d_k_mean = (d_k_hor + d_k_ver) / 2;
	}
	else
	{
		d_k_mean = 200 / px_size;
	}
	return d_k_mean;
}

vector<vector<list<int>>> grid_pos03::grid_params(void)
{
	vector<vector<list<int>>> look_up(200, vector<list<int>>(2));

	for (int i = 0; i < 200; i++)
	{
		int col = i % 10;
		int d_col = (col - 5) * 2 * grid_width;

		int row = (int)(i / 10);
		int d_row = (row - 10) * grid_height;

		if (((int)(i / 10)) % 2 == 0)
		{
			//trial method
			list<int>i0;
			i0.push_back(d_row);
			i0.push_back(d_col);
			look_up[i][0] = i0;

			list<int>i1;
			i1.push_back(d_row);
			i1.push_back(d_col + grid_width);
			look_up[i][1] = i1;
		}
		else
		{
			list<int>i0;
			i0.push_back(d_row);
			i0.push_back(d_col + grid_width);
			look_up[i][0] = i0;

			list<int>i1;
			i1.push_back(d_row);
			i1.push_back(d_col);
			look_up[i][1] = i1;
		}
	}

	return look_up;
}

bool is_x_nan(double x)
{
	return isnan(x);
}

double grid_pos03::weighted_avg(const vector<vector<double>> &center)
{
	vector<vector<double>> valid_vals;
	double av_val = 0.0;

	for (int i = 0; i < center.size(); i++)
	{
		if (!(any_of(center[i].begin(), center[i].end(), is_x_nan)))
		{
			valid_vals.push_back(center[i]);
		}
	}
	try
	{
		vector<double> vv_0((int)valid_vals.size());
		vector<double> vv_1((int)valid_vals.size());
		vector<double> vv_01((int)valid_vals.size());

		for (size_t i = 0; i < valid_vals.size(); i++)
		{
			vv_0[i] = valid_vals[i][0];
			vv_1[i] = valid_vals[i][1];
			vv_01[i] = vv_0[i] * vv_1[i];
		}

		av_val = reduce(vv_01.begin(), vv_01.end()) / reduce(vv_1.begin(), vv_1.end());
	}
	catch (const std::out_of_range&)
	{
		av_val = nan("");
	}
	return av_val;
}

void grid_pos03::Execute(stage45 s45, stage56 &s56)
{
	for (int row = 0; row < s45.gridRows; row++)
	{
		for (int col = 0; col < s45.gridCols; col++)
		{
			Grid& field = s45.grids[row][col];

			if (field.max_pos.size() > 0)
			{
				if (((field.orientation == "hor") && ((field.max_pos.size() == 7) || (row == (s45.gridRows - 1)))) || ((field.orientation == "ver") && ((field.max_pos.size() >= 9) || (col == (s45.gridCols - 1)))))
				{
					field.max_pos.erase(field.max_pos.begin());
				}
			}
		}
	}

	double d_k = get_d_k(s45.grids, s45.gridRows, s45.gridCols);
	s56.k = d_k * (px_size / 200);

	vector<vector<double>> center_hor;
	vector<vector<double>> center_ver;
	list<int> look_el;
	int P;

	vector<vector<list<int>>> look_up = grid_params();

	if ((s45.index >= 0) && (s45.index < 200))
	{
		if (s45.grids[1][1].orientation == "hor")
		{
			look_el.push_back(look_up[s45.index][0].front());
			look_el.push_back(look_up[s45.index][0].back());
		}
		else
		{
			look_el.push_back(look_up[s45.index][1].front());
			look_el.push_back(look_up[s45.index][1].back());
		}
	}
	else
	{
		look_el.push_back(0);
		look_el.push_back(0);
	}

	for (int row = 0; row < s45.gridRows; row++)
	{
		for (int col = 0; col < s45.gridCols; col++)
		{
			vector<double> center;
			Grid &field = s45.grids[row][col];

			for (int i_max = 0; i_max < field.max_pos.size(); i_max++)
			{
				if (i_max < 8)
				{
					int mask_pos = grid_pos02::get_mask_pos(field, row, col, i_max);
					if (field.orientation == "hor")
					{
						P = mask_pos + look_el.front();
					}
					else
					{
						P = mask_pos + look_el.back();
					}
					double pa = (-P * s56.k);
					double va = (field.max_pos[i_max] * px_size);
					center.push_back((-P * s56.k) + (field.max_pos[i_max] * px_size));

				}
			}

			double cen_mean = Evaluation::MeanR(center);

			vector<double> t1;
			t1.push_back(cen_mean);
			t1.push_back(field.px_num);

			if (field.orientation == "hor")
			{
				center_hor.push_back(t1);
			}
			else
			{
				center_ver.push_back(t1);
			}
			s45.grids[row][col] = field;
		}
	}

	s56.xi = weighted_avg(center_ver);
	s56.zi = weighted_avg(center_hor);

	s56.gridRows = s45.gridRows;
	s56.gridCols = s45.gridCols;
	s56.grids = s45.grids;
	s56.index = s45.index;
	s56.ind_ori = s45.ind_ori;
	
	fifo.push(s56);
	cout << s56;
}
