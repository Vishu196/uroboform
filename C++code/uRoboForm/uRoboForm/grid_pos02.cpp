#include "grid_pos02.h"

using namespace std;
using namespace cv;

std::ostream& operator<<(std::ostream& ostr, const stage45& s45)
{
	cout << "index: " << s45.index << endl;
	cout << "ori: " << s45.ind_ori << endl;
	cout << "grid_pos02 complete." << endl;
	return ostr;
}

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
			if (r > 0)
			{
				vector<double> max_pos_arr(r);
				copy(grids01[row][col].max_pos.begin(), grids01[row][col].max_pos.end(), max_pos_arr.begin());
				vector<double> m_pos_de = Evaluation::decumulate(max_pos_arr);
				bool con = any_of(m_pos_de.begin(), m_pos_de.end(), isGreater);
				while (con)
				{
					vector<int> m_pos_de1;
					m_pos_de1.reserve(r);

					for (int e = 0; e < r; e++)
					{
						if (m_pos_de[e] > 100)
						{
							m_pos_de1.push_back(e);
						}
					}

					reverse(m_pos_de1.begin(), m_pos_de1.end());

					for (auto i = 0; i < m_pos_de1.size(); i++)
					{
						int i_fill = m_pos_de1[i];
						int fill1 = i_fill + 1;

						double mean_maxPos = (grids01[row][col].max_pos[i_fill] + grids01[row][col].max_pos[fill1]) / 2;
						grids01[row][col].max_pos.insert(grids01[row][col].max_pos.begin() + fill1, mean_maxPos);
					}
				}

				if (r >= 1)
				{
					if (((row != (gRows - 1)) && (grids01[row][col].orientation == "hor") && (((grids01[row + 1][col].im_loc[0]) - (grids01[row][col].max_pos.back())) > 85)) || ((col != (gCols - 1)) && (grids01[row][col].orientation == "ver") && (((grids01[row][col + 1].im_loc[1]) - (grids01[row][col].max_pos.back())) > 115)))
					{
						double new_mp = Evaluation::MeanR(max_pos_arr) + (grids01[row][col].max_pos.back());
						grids01[row][col].max_pos.push_back(new_mp);
					}
					else if (((row != 0) && (grids01[row][col].orientation == "hor") && (((grids01[row][col].max_pos[0]) - (grids01[row][col].im_loc[0])) > 85)) || ((col != 0) && (grids01[row][col].orientation == "ver") && (((grids01[row][col].max_pos[0]) - (grids01[row][col].im_loc[1])) > 85)))
					{
						double new_mp = Evaluation::MeanR(max_pos_arr) + (grids01[row][col].max_pos.front());
						grids01[row][col].max_pos.insert(grids01[row][col].max_pos.begin(), new_mp);
					}
				}
			}
		}
	}
	return grids01;
}

vector<int> grid_pos02::linspace(double start, double end, int num)
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

RdBinary grid_pos02::ReadBinary(Grid**& cgrids, const Mat& img)
{
	string ori;
	int code = 0;
	vector<double> max_mean;
	const int x = img.rows;
	const int y = img.cols;

	if (cgrids[1][1].max_pos.size() >= 5)
	{
		ori = cgrids[1][1].orientation;
		if (ori == "hor")
		{
			if (cgrids[0][1].max_pos.size() == 9)
				max_mean = cgrids[0][1].max_pos;
			else if (cgrids[2][1].max_pos.size() == 9)
				max_mean = cgrids[2][1].max_pos;
			else
				max_mean.clear();
		}
		else
		{
			if (cgrids[1][0].max_pos.size() == 7)
				max_mean = cgrids[1][0].max_pos;
			else if (cgrids[1][2].max_pos.size() == 7)
				max_mean = cgrids[1][2].max_pos;
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
			
			if (ori == "hor")
			{
				const int w11 = int(cgrids[1][1].max_pos[0] - d_mean / 4);
				const int w22 = int(cgrids[1][1].max_pos[0] + d_mean / 4);

				Mat coded_area(w22-w11, y, CV_8U);				
				Mat sourceRegion = img.rowRange(w11, w22);
				sourceRegion.copyTo(coded_area.rowRange(0, w22 - w11));
				
				coded_line = Evaluation::Mean0R(coded_area);
			}
			else
			{
				const int l22 = int(cgrids[1][1].max_pos[0] + d_mean / 4);
				const int l11 = int(cgrids[1][1].max_pos[0] - d_mean / 4);
				
				Mat coded_area(x, l22-l11, CV_8U);
				Mat sourceRegion = img.colRange(l11, l22);
				sourceRegion.copyTo(coded_area.colRange(0, l22 - l11));
				
				coded_line = Evaluation::Mean1R(coded_area);
			}

			vector<double> code_mean(9);
			for (int i = 0; i < 9; i++)
			{
				int u = i + 1;
				std::vector<double> coded_line_p(coded_line.begin() + bounds[i], coded_line.begin() + bounds[u]);
				code_mean[i] = Evaluation::MeanR(coded_line_p);
			}

			double min_val;
			minMaxLoc(img, &min_val, nullptr);

			int amin = min_val; 
			Scalar tempVal = mean(img);
			double means = tempVal.val[0];
			double th = amin + ((means - amin) * 0.85);

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

			code = stoi(bin_str, nullptr, 2);
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
			s_index = (int)(i - field.max_pos.size());
		}
		else
		{
			s_index = (int)i_max;
		}
		int r = row - 1;
		mask_pos = (s_index * 200) + 350 + (r * grid_height);
	}
	else
	{
		if (col == 0)
		{
			size_t i = i_max + 8;
			s_index = (int)(i - field.max_pos.size());
		}
		else
		{
			s_index = (int)i_max;
		}
		int c = col - 1;
		mask_pos = (s_index * 200) + 350 + (c * grid_width);
	}

	return mask_pos;
}

void grid_pos02::Execute(stage34 s34)
{
	stage45 s45;

	s45.grids = checkGrid(s34.grids, s34.gridRows, s34.gridCols);

	struct RdBinary I = ReadBinary(s45.grids, s34.img);
	
	s45.gridRows = s34.gridRows;
	s45.gridCols = s34.gridCols;
	s45.index = I.index;
	s45.ind_ori = I.ind_ori;

	fifo.push(s45);

	#if DEBUGMODE == DebugMode::AllTimesValues
	#elif DEBUGMODE == DebugMode::AllValues
	cout << s45;
	#endif
}