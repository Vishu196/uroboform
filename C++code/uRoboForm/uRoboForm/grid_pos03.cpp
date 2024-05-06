#include "grid_pos03.h"
#include "constants.h"
#include"Evaluation.h"
#include "debug_logs.h"

using namespace std;
using namespace cv;

std::ostream& operator<<(std::ostream& ostr, const stage56& s56)
{
	cout << "index: " << s56.index << endl;
	cout << "ori: " << s56.is_hor << endl;
	cout << "k:" << s56.k << endl;
	cout << "xi: " << s56.xi << endl;
	cout << "zi: " << s56.zi << endl;
	cout << "Stage 5 complete." << endl;
	return ostr;
}

bool isGreater(double n)
{
	return n > 100;
}

Grid** grid_pos03::checkGrid(const stage45& s45)
{
	Grid** grids01 = s45.grids;

	for (int row = 0; row < s45.gridRows; row++)
	{
		for (int col = 0; col < s45.gridCols; col++)
		{
			size_t r = grids01[row][col].get_max_pos().size();
			if (r > 0)
			{
				vector<double> max_pos_arr(r);
				copy(grids01[row][col].get_max_pos().begin(), grids01[row][col].get_max_pos().end(), max_pos_arr.begin());
				vector<double> m_pos_de = Evaluation::decumulate(max_pos_arr);
				bool con = any_of(m_pos_de.begin(), m_pos_de.end(), isGreater);
				while (con)
				{
					/*vector<int> m_pos_de1;
					m_pos_de1.reserve(r);

					for (int e = 0; e < r; e++)
					{
						if (m_pos_de[e] > 100)
						{
							m_pos_de1.push_back(e);
						}
					}

					reverse(m_pos_de1.begin(), m_pos_de1.end());*/

					for (auto i = m_pos_de.rbegin(); i != m_pos_de.rend(); ++i)
					{
						if (*i > 100)
						{
							int i_fill = *i;
							int fill1 = i_fill + 1;

							double mean_maxPos = (grids01[row][col].get_max_pos()[i_fill] + grids01[row][col].get_max_pos()[fill1]) / 2;
							grids01[row][col].get_max_pos().insert(grids01[row][col].get_max_pos().begin() + fill1, mean_maxPos);

						}
					}

				}

				if (r >= 1)
				{
					if (((row != (s45.gridRows - 1)) && (grids01[row][col].is_hor == true) && (((grids01[row + 1][col].im_loc[0]) - (grids01[row][col].get_max_pos().back())) > 85)) || ((col != (s45.gridCols - 1)) && (grids01[row][col].is_hor == false) && (((grids01[row][col + 1].im_loc[1]) - (grids01[row][col].get_max_pos().back())) > 115)))
					{
						double new_mp = Evaluation::MeanR(max_pos_arr) + (grids01[row][col].get_max_pos().back());
						grids01[row][col].get_max_pos().push_back(new_mp);
					}
					else if (((row != 0) && (grids01[row][col].is_hor == true) && (((grids01[row][col].get_max_pos()[0]) - (grids01[row][col].im_loc[0])) > 85)) || ((col != 0) && (grids01[row][col].is_hor == false) && (((grids01[row][col].get_max_pos()[0]) - (grids01[row][col].im_loc[1])) > 85)))
					{
						double new_mp = Evaluation::MeanR(max_pos_arr) + (grids01[row][col].get_max_pos().front());
						grids01[row][col].get_max_pos().insert(grids01[row][col].get_max_pos().begin(), new_mp);
					}
				}
			}
		}
	}

	return grids01;
}

vector<int> grid_pos03::linspace(double start, double end, int num)
{
	vector<int> bounds;
	bounds.reserve(num);

	if (0 != num)
	{
		if (1 == num)
			bounds.push_back((int)start);
		else
		{
			double delta = (end - start) / (num - 1);

			for (auto i = 0; i < (num - 1); ++i)
			{
				bounds.push_back((int)(start + delta * i));
			}
			bounds.push_back((int)end);
		}
	}
	bounds.shrink_to_fit();
	return bounds;
}

RdBinary grid_pos03::ReadBinary(const stage56& s56, const Mat& img)
{
	struct RdBinary rd;
	rd.is_hor;
	rd.index = 0;
	vector<double> max_mean;
	const int x = img.rows;
	const int y = img.cols;

	if (s56.grids[1][1].get_max_pos().size() >= 5)
	{
		rd.is_hor = s56.grids[1][1].is_hor;
		if (rd.is_hor)
		{
			if (s56.grids[0][1].get_max_pos().size() == 9)
				max_mean = s56.grids[0][1].get_max_pos();
			else if (s56.grids[2][1].get_max_pos().size() == 9)
				max_mean = s56.grids[2][1].get_max_pos();
			else
				max_mean.clear();
		}
		else
		{
			if (s56.grids[1][0].get_max_pos().size() == 7)
				max_mean = s56.grids[1][0].get_max_pos();
			else if (s56.grids[1][2].get_max_pos().size() == 7)
				max_mean = s56.grids[1][2].get_max_pos();
			else
				max_mean.clear();
		}

		vector<double> max_mean_de = Evaluation::decumulate(max_mean);
		double d_mean = Evaluation::MeanR(max_mean_de);

		if ((max_mean.size() >= 4) && (d_mean >= 50) && (d_mean <= 70))
		{
			const double start = max_mean[0] - (d_mean * 3 / 4);
			const double end = max_mean[3] + (d_mean * 3 / 4);
			vector<int> bounds = linspace(start, end, 10);
			vector<double> coded_line;

			if (rd.is_hor)
			{
				const int w11 = int(s56.grids[1][1].get_max_pos()[0] - d_mean / 4);
				const int w22 = int(s56.grids[1][1].get_max_pos()[0] + d_mean / 4);

				Mat coded_area(w22 - w11, y, CV_8U);
				Mat sourceRegion = img.rowRange(w11, w22);
				sourceRegion.copyTo(coded_area.rowRange(0, w22 - w11));

				coded_line = Evaluation::Mean0R(coded_area);
			}
			else
			{
				const int l22 = int(s56.grids[1][1].get_max_pos()[0] + d_mean / 4);
				const int l11 = int(s56.grids[1][1].get_max_pos()[0] - d_mean / 4);

				Mat coded_area(x, l22 - l11, CV_8U);
				Mat sourceRegion = img.colRange(l11, l22);
				sourceRegion.copyTo(coded_area.colRange(0, l22 - l11));

				coded_line = Evaluation::Mean1R(coded_area);
			}

			vector<double> code_mean(9);
			for (int i = 0; i < 9; i++)
				code_mean[i] = Evaluation::Mean(coded_line.begin() + bounds[i], coded_line.begin() + bounds[i + 1]);

			double min_val;
			minMaxLoc(img, &min_val, nullptr);

			int amin = min_val;
			Scalar tempVal = mean(img);
			double means = tempVal.val[0];
			double th = amin + ((means - amin) * 0.85);

			for (size_t i = 1; i < code_mean.size(); i++)
			{
				code_mean[i] < th ? rd.index |= (1u << (8 - i)) : rd.index &= ~(1u << (8 - i));
			}

		}
		else
			rd.index = 300;
	}
	else
	{
		rd.index = 400;
		//rd.ind_ori = "non";
	}

	return rd;
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

double grid_pos03::get_d_k(const stage56 &s56)
{
	vector<vector<double>>lines_hor;
	vector<vector<double>> lines_ver;
	for (int row = 0; row < s56.gridRows; row++)
	{
		for (int col = 0; col < s56.gridCols; col++)
		{
			Grid field = s56.grids[row][col];

			if (field.get_max_pos().size() >= 1)
			{
				if (field.is_hor)
				{
					double h11 = field.get_mask_pos(row, col, 0);
					double h12 = field.get_max_pos().front();
					lines_hor.push_back({h11,h12});

					double h21 = field.get_mask_pos(row, col, field.get_max_pos().size() - 1);
					double h22 = field.get_max_pos().back();
					lines_hor.push_back({ h21,h22 });
				}
				else
				{
					double v11 = field.get_mask_pos(row, col, 0);
					double v12 = field.get_max_pos().front();
					lines_ver.push_back({ v11,v12 });
					
					double v21 = field.get_mask_pos(row, col, field.get_max_pos().size() - 1);
					double v22 = field.get_max_pos().back();
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

list<int> grid_pos03::get_look_el(const stage56 &s56)
{
	list<int> look_el;
	vector<vector<list<int>>> look_up = grid_params();

	if ((s56.index >= 0) && (s56.index < 200))
	{
		if (s56.grids[1][1].is_hor)
		{
			look_el.push_back(look_up[s56.index][0].front());
			look_el.push_back(look_up[s56.index][0].back());
		}
		else
		{
			look_el.push_back(look_up[s56.index][1].front());
			look_el.push_back(look_up[s56.index][1].back());
		}
	}
	else
	{
		look_el.push_back(0);
		look_el.push_back(0);
	}

	return look_el;
}

axis grid_pos03::get_center_arr(const stage56 &s56)
{
	list<int> look_el = get_look_el(s56);
	
	axis a;
	a.center_hor;
	a.center_ver;
	int P;

	for (int row = 0; row < s56.gridRows; row++)
	{
		for (int col = 0; col < s56.gridCols; col++)
		{
			vector<double> center;
			const Grid& field = s56.grids[row][col];

			for (int i_max = 0; i_max < field.get_max_pos().size(); i_max++)
			{
				if (i_max < 8)
				{
					int mask_pos = field.get_mask_pos(row, col, i_max);
					if (field.is_hor)
						P = mask_pos + look_el.front();
					else
						P = mask_pos + look_el.back();
					center.push_back((-P * s56.k) + (field.get_max_pos()[i_max] * px_size));
				}
			}

			double cen_mean = Evaluation::MeanR(center);

			vector<double> t1;
			t1.push_back(cen_mean);
			t1.push_back(field.px_num);

			if (field.is_hor)
				a.center_hor.push_back(t1);
			else
				a.center_ver.push_back(t1);
			
			//s45.grids[row][col] = field;
		}
	}

	return a;
}

void Execute_1(stage56& s56)
{
	for (int row = 0; row < s56.gridRows; row++)
	{
		for (int col = 0; col < s56.gridCols; col++)
		{
			Grid& field = s56.grids[row][col];

			if (field.get_max_pos().size() > 0)
			{
				if (((field.is_hor == true) && ((field.get_max_pos().size() == 7) || (row == (s56.gridRows - 1)))) || ((field.is_hor == false) && ((field.get_max_pos().size() >= 9) || (col == (s56.gridCols - 1)))))
				{
					field.get_max_pos().erase(field.get_max_pos().begin());
				}
			}
		}
	}
}

void grid_pos03::Execute(stage45 s45)
{
	stage56 s56;
	if (s45.edges_sufficient)
	{
		s56.gridRows = s45.gridRows;
		s56.gridCols = s45.gridCols;

		s56.grids = checkGrid(s45);

		RdBinary I = ReadBinary(s56, s45.img);
		s56.index = I.index;
		s56.is_hor = I.is_hor;

		Execute_1(s56);

		double d_k = get_d_k(s56);
		s56.k = d_k * (px_size / 200);

		axis a = get_center_arr(s56);
		s56.xi = weighted_avg(a.center_ver);
		s56.zi = weighted_avg(a.center_hor);
	}

	else
	{
		s56.index = (int)nan("");
		s56.k = nan("");
		s56.is_hor = ("");
		s56.grids = {};
		s56.xi = nan("");
		s56.zi = nan("");
	}

	fifo.push(s56);
	cout << s56;
}
