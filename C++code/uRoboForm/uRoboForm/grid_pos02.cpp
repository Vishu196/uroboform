#include "grid_pos02.h"

bool isGreater(double n)
{
	return n > 100;
}

Grid** grid_pos02::checkGrid(Grid** &grids01, int gRows, int gCols)
{
	for (int row = 0; row < gRows; row++)
	{
		for (int col = 0; col < gCols; col++)
		{
			size_t r = grids01[row][col].max_pos.size();
			vector<double> max_pos_arr(r);
			copy(grids01[row][col].max_pos.begin(), grids01[row][col].max_pos.end(), max_pos_arr.begin());
			vector<double> m_pos_de = Evaluation::decumulateDouble(max_pos_arr);
			bool con = any_of(m_pos_de.begin(), m_pos_de.end(), isGreater);
			while (con)
			{
				vector<int> m_pos_de1(r);
				int f = 0;
				for (int e = 0; e < r; e++)
				{
					if (m_pos_de[e] > 100)
					{
						m_pos_de1[f] = e;
						f++;
					}
				}
				
				m_pos_de1.resize(f);

				reverse(m_pos_de1.begin(), m_pos_de1.end());

				for (int i = 0; i < f ; i++)
				{
					int i_fill = m_pos_de1[i];
					int fill1 = i_fill + 1;
					
					double mean_maxPos = (grids01[row][col].max_pos[i_fill] + grids01[row][col].max_pos[fill1]) / 2;
					grids01[row][col].max_pos.insert(grids01[row][col].max_pos.begin() + fill1, mean_maxPos);
				}
			}

			if (r >= 1)
			{
				bool c1 = row != gRows - 1;
				bool c2 = grids01[row][col].orientation == "hor";
				bool c3 = ((grids01[row+1][col].im_loc.front()) - (grids01[row][col].max_pos.back())) > 85;
				bool c4 = col != gCols - 1;;
				bool c5 = grids01[row][col].orientation == "ver";
				bool c6 = ((grids01[row][col + 1].im_loc.back()) - (grids01[row][col].max_pos.back())) > 115;

				bool c11 = row != 0;
				bool c22 = grids01[row][col].orientation == "hor";
				bool c33 = ((grids01[row][col].max_pos.front()) - (grids01[row][col].im_loc.front())) > 85;
				bool c44 = col != 0;
				bool c55 = grids01[row][col].orientation == "ver";
				bool c66 = ((grids01[row][col].max_pos.front()) - (grids01[row][col].im_loc.back())) > 85;

				if ( (c1 && c2 && c3) || (c4 && c5 && c6) )
				{
					double new_mp = Evaluation::MeanR(max_pos_arr) + (grids01[row][col].max_pos.back());
					grids01[row][col].max_pos.push_back(new_mp);
				}
				else if ((c11 && c22 && c33) || (c44 && c55 && c66))
				{
					double new_mp = Evaluation::MeanR(max_pos_arr) + (grids01[row][col].max_pos.front());
					grids01[row][col].max_pos.insert(grids01[row][col].max_pos.begin(),new_mp);
				} 				
			}
		}
	}
	return grids01;
}

vector<int> grid_pos02::linspace(double start, double end, int num)
{
	std::vector<int> bounds;

	if (0 != num)
	{
		if (1 == num)
		{
			bounds.push_back((int)start);
		}
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

struct RdBinary grid_pos02::ReadBinary(Grid** &cgrids, const Mat &img)
{
	string ori;
	int code;
	vector<double> max_mean;
	int x = img.rows;
	int y = img.cols;

	if (cgrids[1][1].max_pos.size() >= 5)
	{
		ori = cgrids[1][1].orientation;

		if (ori == "hor")
		{
			if (cgrids[0][1].max_pos.size() == 9)
			{
				max_mean = cgrids[0][1].max_pos;
			}
			else if (cgrids[2][1].max_pos.size() == 9)
			{
				max_mean = cgrids[2][1].max_pos;
			}
			else
			{
				max_mean.clear();
			}
		}
		else
		{
			if (cgrids[1][0].max_pos.size() == 7)
			{
				max_mean = cgrids[1][0].max_pos;
			}
			else if (cgrids[1][2].max_pos.size() == 7)
			{
				max_mean = cgrids[1][2].max_pos;
			}
			else
			{
				max_mean.clear();
			}
		}
		
		double d_mean = Evaluation::MeanR(max_mean);

		if ((max_mean.size() >= 4) && (d_mean >= 50) && (d_mean <= 70))
		{
			double start = max_mean[0] - (d_mean * 3 / 4);
			double end = max_mean[3] + (d_mean * 3 / 4);
			vector<int> bounds = linspace(start, end, 10);

			int coded_areaRows = 0;
			int coded_areaCols = 0;
			Mat coded_area(coded_areaRows, coded_areaCols, CV_8U, (int)img.step);
			uint8_t* coded_areaData = coded_area.data;
			
			vector<double> coded_line;
			if (ori == "hor")
			{
				int w1 = int(cgrids[1][1].max_pos[0] + d_mean / 4) - int(cgrids[1][1].max_pos[0] - d_mean / 4);
				coded_areaRows = w1;
				coded_areaCols = y;
				
				for (int i = 0; i < w1; i++)
				{
					for (int j = 0; j < y; j++)
					{
						int val = img.data[i * img.step + j];
						size_t idx = ((i * coded_area.step) + j);
						*(coded_areaData + idx) = val;
					}
				}				
				coded_line = Evaluation::Mean0R(coded_area);
			}
			else
			{
				int l1 = int(cgrids[1][1].max_pos[0] + d_mean / 4) - int(cgrids[1][1].max_pos[0] - d_mean / 4);
				
				coded_areaRows = x;
				coded_areaCols = l1;

				for (int i = 0; i < x; i++)
				{
					for (int j = 0; j < l1; j++)
					{
						int val = img.data[i * img.step + j];
						size_t idx = ((i * coded_area.step) + j);
						*(coded_areaData + idx) = val;
					}

				}
				coded_line = Evaluation::Mean1R(coded_area);
			}

			vector<double> code_mean(9);
			for (int i = 0; i < 9; i++)
			{
				int u = i + 1;
				int row_size = bounds[u] - bounds[i];
				vector<double> coded_line_p (row_size);
				for (int l = bounds[i]; l < bounds[u]; l++)
				{
					coded_line_p[l] = coded_line[l];
				}
				code_mean[i] = Evaluation::MeanR(coded_line_p);
			}
			
			vector<int> flat_img (img.rows * img.cols);
			int m = 0;
			for (int r = 0; r < x; r++)
			{
				for (int c = 0; c < y; c++)
				{
					flat_img[m] = img.data[r* img.step + c];
					m++;
				}
			}
				
			int amin = *min_element(flat_img.begin(), flat_img.end());
			double mean = Evaluation::IntMeanR(flat_img);
			double th = amin + ((mean - amin) * 0.85);

			vector<int>code_bin(9);
			for (size_t i = 0; i < code_mean.size(); i++)
			{
				code_bin[i] = code_mean[i] < th ? 1 : 0;
			}
			
			code_bin.erase(code_bin.begin());

			string bin_str = "";

			for (int i : code_bin) 
			{
					string curr = to_string(i);
					bin_str += curr;
			}

			int code = stoi(bin_str);				
		}
		else
			code = 300;
	}
	else
	{
		code = 400;
		ori = "non";
	}

	struct RdBinary rd;
	rd.index = code;
	rd.ind_ori = ori;

	return rd;
}

int grid_pos02::get_mask_pos(Grid field, int row, int col, size_t i_max)
{
	int s_index = 0;
	int mask_pos = 0;

	if (field.orientation == "hor")
	{
		if (row == 0)
		{
			size_t i = i_max + 6;
			s_index = i - field.max_pos.size();
		}
		else
		{
			s_index = i_max;
		}
		int r = row - 1;
		mask_pos = (s_index * 200) + 350 + (r * grid_height);
	}
	else
	{
		if (col == 0)
		{
			size_t i = i_max + 8;
			s_index = i - field.max_pos.size();
		}
		else
		{
			s_index = i_max;
		}
		int c = col - 1;
		mask_pos = (s_index * 200) + 350 + (c * grid_width);
	}

	return mask_pos;
}

double grid_pos02::calc_d_k(vector<vector <double>> lines)
{
	size_t n = lines.size();
	const int n1 = n - 1;
	const int n2 = n - 3;
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

double grid_pos02::get_d_k(Grid** &cgrids, int gRows, int gCols)
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
					h1.push_back(get_mask_pos(field, row, col, 0));
					h1.push_back(field.max_pos.front());
					lines_hor.push_back(h1);
					
					vector<double> h2;
					h2.push_back(get_mask_pos(field, row, col, field.max_pos.size() - 1));
					h2.push_back(field.max_pos.back());
					lines_hor.push_back(h2);
				}
				else
				{
					vector<double> v1;
					v1.push_back(get_mask_pos(field, row, col, 0));
					v1.push_back(field.max_pos.front());
					lines_ver.push_back(v1);

					vector<double> v2;
					v2.push_back(get_mask_pos(field, row, col, field.max_pos.size() - 1));
					v2.push_back(field.max_pos.back());
					lines_ver.push_back(v2);
				}
			}
		}
	}
	double d_k_mean = 0;

	if ((lines_ver.size() >= 3) && (lines_hor.size()>=3))
	{
		sort(lines_hor.begin(),lines_hor.end());
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

void grid_pos02::DisplayResult(const stage45& s45)
{
	fifo.push(s45);

	cout << "index: " << s45.index << endl;
	cout << "ori: " << s45.ind_ori << endl;
	cout << "k: " << s45.k << endl;
	cout << "grid_pos02 complete." << endl;
}

void grid_pos02::Execute(stage34 s34)
{
	stage45 s45;

	s45.grids = checkGrid(s34.grids, s34.gridRows, s34.gridCols);

	struct RdBinary I = ReadBinary(s45.grids, s34.img);

	for (int row = 0; row < s34.gridRows; row++)
	{
		for (int col = 0; col < s34.gridCols; col++)
		{
			Grid field = s45.grids[row][col];

			bool c1 = field.orientation == "hor";
			bool c2 = field.max_pos.size() == 7;
			bool c3 = row == s34.gridRows - 1;
			bool c4 = field.orientation == "ver";
			bool c5 = field.max_pos.size() >= 9;
			bool c6 = col == s34.gridCols - 1;
			if ((c1 && (c2 || c3)) || (c4 && (c5 || c6)))
			{
				field.max_pos.erase(field.max_pos.begin());
			}
		}
	}

	double d_k = get_d_k(s45.grids, s34.gridRows, s34.gridCols);
	s45.k = d_k * (px_size / 200);

	s45.gridRows = s34.gridRows;
	s45.gridCols = s34.gridCols;
	s45.index = I.index;
	s45.ind_ori = I.ind_ori;

	DisplayResult(s45);
}