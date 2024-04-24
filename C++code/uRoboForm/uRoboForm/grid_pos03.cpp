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
		line_0 = (line_arr[0] + line_arr[1])/2;
	}
	else
		line_0 = lines[0][1];

	if (lines[n1][0] == lines[n2][0])
	{
		vector<double> line_arr(2);
		line_arr[0] = lines[n1][1];
		line_arr[1] = lines[n2][1];
		line_n = (line_arr[0] + line_arr[1]) / 2;
	}
	else
		line_n = lines[lines.size() - 1][1];
	
	return (line_n - line_0) / ((lines[n1][0] - lines[0][0]) / 200);
}

double grid_pos03::get_d_k(const stage45 &s45)
{
	vector<vector<double>>lines_hor;
	vector<vector<double>> lines_ver;
	for (int row = 0; row < s45.gridRows; row++)
	{
		for (int col = 0; col < s45.gridCols; col++)
		{
			Grid field = s45.grids[row][col];

			if (field.max_pos.size() >= 1)
			{
				if (field.orientation == "hor")
				{
					double h11 = (grid_pos02::get_mask_pos(field, row, col, 0));
					double h12 = (field.max_pos.front());
					lines_hor.push_back({h11,h12});

					double h21 = (grid_pos02::get_mask_pos(field, row, col, field.max_pos.size() - 1));
					double h22 = (field.max_pos.back());
					lines_hor.push_back({ h21,h22 });
				}
				else
				{
					double v11 = (grid_pos02::get_mask_pos(field, row, col, 0));
					double v12 = (field.max_pos.front());
					lines_ver.push_back({ v11,v12 });
					
					double v21 = (grid_pos02::get_mask_pos(field, row, col, field.max_pos.size() - 1));
					double v22 = (field.max_pos.back());
					lines_ver.push_back({ v21,v22 });
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
			look_up[i][0] = { d_row, d_col };
			look_up[i][1] = {d_row, d_col+grid_width};
		}
		else
		{
			look_up[i][0] = { d_row, d_col + grid_width };
			look_up[i][1] = { d_row, d_col };
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
		double sum_vv_01 = 0.0;
		double sum_vv_1 = 0.0;

		for (const auto& val : valid_vals) 
		{
			double vv_0 = val[0];
			double vv_1 = val[1];
			sum_vv_01 += vv_0 * vv_1;
			sum_vv_1 += vv_1;
		}

		av_val = sum_vv_01 / sum_vv_1;
	}
	catch (const std::out_of_range&)
	{
		av_val = nan("");
	}
	return av_val;
}

list<int> grid_pos03::get_look_el(const stage45 &s45)
{
	list<int> look_el;
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

	return look_el;
}

axis grid_pos03::get_center_arr(const stage45 &s45,const stage56 &s56)
{
	list<int> look_el = get_look_el(s45);
	
	axis a;
	a.center_hor;
	a.center_ver;
	int P;

	for (int row = 0; row < s45.gridRows; row++)
	{
		for (int col = 0; col < s45.gridCols; col++)
		{
			vector<double> center;
			Grid& field = s45.grids[row][col];

			for (int i_max = 0; i_max < field.max_pos.size(); i_max++)
			{
				if (i_max < 8)
				{
					int mask_pos = grid_pos02::get_mask_pos(field, row, col, i_max);
					if (field.orientation == "hor")
						P = mask_pos + look_el.front();
					else
						P = mask_pos + look_el.back();
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
				a.center_hor.push_back(t1);
			else
				a.center_ver.push_back(t1);
			
			s45.grids[row][col] = field;
		}
	}

	return a;
}

void Execute_1(stage45& s45)
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
}

void grid_pos03::Execute(stage45 s45, stage56 &s56)
{
	Execute_1(s45);
	double d_k = get_d_k(s45);
	s56.k = d_k * (px_size / 200);
	axis a = get_center_arr(s45, s56);

	s56.xi = weighted_avg(a.center_ver);
	s56.zi = weighted_avg(a.center_hor);
	s56.gridRows = s45.gridRows;
	s56.gridCols = s45.gridCols;
	s56.grids = s45.grids;
	s56.index = s45.index;
	s56.ind_ori = s45.ind_ori;
	
	fifo.push(s56);

	#if DEBUGMODE == DebugMode::AllTimesValues
	#elif DEBUGMODE == DebugMode::AllValues
	#elif DEBUGMODE == DebugMode::FinalValue
	cout << s56;
	#endif
}
