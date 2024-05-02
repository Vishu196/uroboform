#include "grid_pos02.h"
#include "Evaluation.h"
#include "constants.h"
#include "debug_logs.h"

using namespace std;
using namespace cv;

std::ostream& operator<<(std::ostream& ostr, const stage45& s45)
{
	cout << "index: " << s45.index << endl;
	cout << "ori: " << s45.is_hor << endl;
	cout << "grid_pos02 complete." << endl;
	return ostr;
}

bool isGreater(double n)
{
	return n > 100;
}

Grid** grid_pos02::checkGrid(const stage34 &s34)
{
	Grid** grids01 = s34.grids;

	for (int row = 0; row < s34.gridRows; row++)
	{
		for (int col = 0; col < s34.gridCols; col++)
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
					if (((row != (s34.gridRows - 1)) && (grids01[row][col].is_hor == true) && (((grids01[row + 1][col].im_loc[0]) - (grids01[row][col].max_pos.back())) > 85)) || ((col != (s34.gridCols - 1)) && (grids01[row][col].is_hor == false) && (((grids01[row][col + 1].im_loc[1]) - (grids01[row][col].max_pos.back())) > 115)))
					{
						double new_mp = Evaluation::MeanR(max_pos_arr) + (grids01[row][col].max_pos.back());
						grids01[row][col].max_pos.push_back(new_mp);
					}
					else if (((row != 0) && (grids01[row][col].is_hor == true) && (((grids01[row][col].max_pos[0]) - (grids01[row][col].im_loc[0])) > 85)) || ((col != 0) && (grids01[row][col].is_hor == false) && (((grids01[row][col].max_pos[0]) - (grids01[row][col].im_loc[1])) > 85)))
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

RdBinary grid_pos02::ReadBinary(const stage45 &s45, const Mat &img)
{
	struct RdBinary rd;
	rd.is_hor;
	rd.index = 0;
	vector<double> max_mean;
	const int x = img.rows;
	const int y = img.cols;

	if (s45.grids[1][1].max_pos.size() >= 5)
	{
		rd.is_hor = s45.grids[1][1].is_hor;
		if (rd.is_hor)
		{
			if (s45.grids[0][1].max_pos.size() == 9)
				max_mean = s45.grids[0][1].max_pos;
			else if (s45.grids[2][1].max_pos.size() == 9)
				max_mean = s45.grids[2][1].max_pos;
			else
				max_mean.clear();
		}
		else
		{
			if (s45.grids[1][0].max_pos.size() == 7)
				max_mean = s45.grids[1][0].max_pos;
			else if (s45.grids[1][2].max_pos.size() == 7)
				max_mean = s45.grids[1][2].max_pos;
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
				const int w11 = int(s45.grids[1][1].max_pos[0] - d_mean / 4);
				const int w22 = int(s45.grids[1][1].max_pos[0] + d_mean / 4);

				Mat coded_area(w22-w11, y, CV_8U);				
				Mat sourceRegion = img.rowRange(w11, w22);
				sourceRegion.copyTo(coded_area.rowRange(0, w22 - w11));
				
				coded_line = Evaluation::Mean0R(coded_area);
			}
			else
			{
				const int l22 = int(s45.grids[1][1].max_pos[0] + d_mean / 4);
				const int l11 = int(s45.grids[1][1].max_pos[0] - d_mean / 4);
				
				Mat coded_area(x, l22-l11, CV_8U);
				Mat sourceRegion = img.colRange(l11, l22);
				sourceRegion.copyTo(coded_area.colRange(0, l22 - l11));
				
				coded_line = Evaluation::Mean1R(coded_area);
			}

			vector<double> code_mean(9);
			for (int i = 0; i < 9; i++)
				code_mean[i] = Evaluation::Mean(coded_line.begin() + bounds[i], coded_line.begin() + bounds[i+1]);

			double min_val;
			minMaxLoc(img, &min_val, nullptr);

			int amin = min_val; 
			Scalar tempVal = mean(img);
			double means = tempVal.val[0];
			double th = amin + ((means - amin) * 0.85);

			for (size_t i = 1; i < code_mean.size(); i++)
			{
				code_mean[i] < th ? rd.index |= (1u << (8-i)) : rd.index &= ~(1u << (8 - i));
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

void grid_pos02::Execute(stage34 s34)
{
	stage45 s45;

	s45.grids = checkGrid(s34);

	RdBinary I = ReadBinary(s45, s34.img);
	
	s45.gridRows = s34.gridRows;
	s45.gridCols = s34.gridCols;
	s45.index = I.index;
	s45.is_hor = I.is_hor;

	fifo.push(s45);

	cout << s45;
}