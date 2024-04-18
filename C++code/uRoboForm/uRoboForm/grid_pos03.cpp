#include "grid_pos03.h"

using namespace std;

std::ostream& operator<<(std::ostream& ostr, const stage56& s56)
{
	cout << "xi: " << s56.xi << endl;
	cout << "zi: " << s56.zi << endl;
	cout << "grid_pos03 complete." << endl;
	return ostr;
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

stage56 grid_pos03::Execute(stage45 s45)
{
	stage56 s56;
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
			Grid field = s45.grids[row][col];

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
					double pa = (-P * s45.k);
					double va = (field.max_pos[i_max] * px_size);
					center.push_back((-P * s45.k) + (field.max_pos[i_max] * px_size));

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
	s56.k = s45.k;
	s56.ind_ori = s45.ind_ori;
	
	fifo.push(s56);
	cout << s56;
	return s56;
}
