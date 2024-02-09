#include "grid_pos02.h"

static struct stage43 s43;
static struct stage45 s45;

static void buffer_s43_init()
{
	s43.imgRows = 0;
	s43.imgCols = 0;
	s43.img = new int* [1080];;// new int[1080][1440];
	for (int h = 0; h < 1080; h++)
	{
		s43.img[h] = new int[1440];
	}

	s43.gridRows = 0;
	s43.gridCols = 0;
	s43.grids = new Grid * [3];
	for (int h = 0; h < 3; h++)
	{
		s43.grids[h] = new Grid[3];
	}
}

static void buffer_s45_init()
{
	s45.gridRows = 0;
	s45.gridCols = 0;
	s45.grids = new Grid * [3];
	for (int h = 0; h < 3; h++)
	{
		s45.grids[h] = new Grid[3];
	}

	s45.index = 0;
	s45.k = 0;
	s45.ind_ori = " ";
	s45.grid_ht = 0;
	s45.grid_wid = 0;

}

static void buffers_init(void)
{
	buffer_s43_init();
	buffer_s45_init();
}

grid_pos02::grid_pos02(struct stage34 s34)
{
	buffers_init();

	s43.imgRows = s34.imgRows;
	s43.imgCols = s34.imgCols;
	s43.gridRows = s34.gridRows;
	s43.gridCols = s34.gridCols;

	for (int i = 0; i < s34.imgRows; i++)
	{
		memcpy(s43.img[i], s34.img[i], (s34.imgCols * sizeof(int)));
	}

	for (int i = 0; i < s34.gridRows; i++)
	{
		memcpy(s43.grids[i], s34.grids[i], (s34.gridCols * sizeof(Grid)));
	}
}

double* decumulateDouble(double* x, int size)
{
	const size_t n = size - 1;
	double* xi = new double[n]();
	double* x1 = new double[n]();
	double* x2 = new double[n]();

	for (size_t i = 0; i < size - 1; i++)
	{
		if (i < n)
		{
			x1[i] = x[i + 1];
		}

	}
	for (int i = 0; i < n; i++)
	{
		x2[i] = x[i];
	}

	for (int i = 0; i < n; i++)
	{
		xi[i] = x1[i] - x2[i];
	}

	delete[] x1;
	delete[] x2;

	return xi;
}

double* Mean0R(int rows, int cols, int** array)
{
	double* Mean0Arr = 0;
	Mean0Arr = new double[cols]();

	double avg = 0.0;
	double sum = 0.0;
	int x = 0;
	for (int w = 0; w < cols; w++)
	{
		sum = 0;
		for (int h = 0; h < rows; h++)
		{
			x = *(*(array + h) + w);
			sum += (double)x;
		}
		avg = sum / (double)rows;
		Mean0Arr[w] = avg;
	}
	return Mean0Arr;
}

double* Mean1R(int rows, int cols, int** array)
{
	double* Mean1Arr = 0;
	Mean1Arr = new double[rows]();

	double avg = 0.0;
	double sum = 0.0;
	int x = 0;
	for (int h = 0; h < rows; h++)
	{
		sum = 0;
		for (int w = 0; w < cols; w++)
		{
			x = *(*(array + h) + w);
			sum += (double)x;
		}
		avg = sum / (double)cols;
		Mean1Arr[h] = avg;
	}
	return Mean1Arr;
}

double MeanR(int rows, double* mean0)
{
	double sum = 0.0;
	double meanR = 0.0;
	for (int i = 0; i < rows; i++)
	{
		sum += mean0[i];
	}

	meanR = sum / rows;
	return meanR;
}

Grid** grid_pos02::checkGrid(Grid** grids)
{
	for (int row = 0; row < s43.gridRows; row++)
	{
		for (int col = 0; col < s43.gridCols; col++)
		{
			int r = s43.grids[row][col].max_pos.size();
			double* max_pos_arr = new double[r]();
			copy(s43.grids[row][col].max_pos.begin(), s43.grids[row][col].max_pos.end(), max_pos_arr);
			double* m_pos_de = decumulateDouble(max_pos_arr, r);
			bool con = std::any_of(m_pos_de, m_pos_de+r, [](int x) { return x > 100; });
			while (con)
			{

				for (int i_fill = 0; i_fill < 10; i_fill++)
				{

				}

			}

			if (r >= 1)
			{
				bool c1 = row != s43.gridRows - 1;
				bool c2 = s43.grids[row][col].orientation == "hor";
				bool c3 = ((s43.grids[row+1][col].im_loc.front()) - (s43.grids[row][col].max_pos.back())) > 85;
				bool c4 = col != s43.gridCols - 1;;
				bool c5 = s43.grids[row][col].orientation == "ver";
				bool c6 = ((s43.grids[row][col + 1].im_loc.back()) - (s43.grids[row][col].max_pos.back())) > 115;
				
				if ( (c1 && c2 && c3) || (c4 && c5 && c6) )
				{
					double new_mp = MeanR(r, max_pos_arr) + (s43.grids[row][col].max_pos.back());
					s43.grids[row][col].max_pos.push_back(new_mp);			
				}

				bool c11 = row != 0;
				bool c22 = s43.grids[row][col].orientation == "hor";
				bool c33 = ((s43.grids[row][col].max_pos.front()) - (s43.grids[row][col].im_loc.front())) > 85;
				bool c44 = col != 0;
				bool c55 = s43.grids[row][col].orientation == "ver";
				bool c66 = ((s43.grids[row][col].max_pos.front()) - (s43.grids[row][col].im_loc.back())) > 85;

				else if ((c11 && c22 && c33) || (c44 && c55 && c66))
				{
					double new_mp = MeanR(r, max_pos_arr) + (s43.grids[row][col].max_pos.front());
					s43.grids[row][col].max_pos.push_front(new_mp);
				}					
			}
		}
	}
	return s43.grids;
}

std::vector<int> linspace(double start, double end, int num)
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
	return bounds;
}

struct RdBinary grid_pos02::ReadBinary(Grid** cgrids, int** img, int x, int y)
{
	string ori;
	int code;
	list<double> max_mean;

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
		int p = max_mean.size();
		double* max_mean_arr = new double[p]();
		copy( max_mean.begin(), max_mean.end(), max_mean_arr);
		double d_mean = MeanR(p ,max_mean_arr);

		if ((max_mean.size() >= 4) && (d_mean >= 50) && (d_mean <= 70))
		{
			double start = max_mean_arr[0] - d_mean * 3 / 4;
			double end = max_mean_arr[3] + d_mean * 3 / 4;
			std::vector<int> bounds = linspace(start, end, 10);
			double* coded_line = 0;
			if (ori == "hor")
			{
				int w1 = int(cgrids[1][1].max_pos.front() + d_mean / 4) - int(cgrids[1][1].max_pos.front() - d_mean / 4);
				int** coded_area = 0;
				coded_area = new int* [w1];
				for (int h = 0; h < (w1); h++)
				{
					coded_area[h] = new int[y];
				}

				for (int i = 0; i < w1; i++)
				{
					for (int j = 0; j < y; j++)
					{
						coded_area[w1][y] = s43.img[w1][y];
					}

				}
				coded_line = new double[w1];

				coded_line = Mean0R(w1, y, coded_area);
			}
			else
			{
				int l1 = int(cgrids[1][1].max_pos.front() + d_mean / 4) - int(cgrids[1][1].max_pos.front() - d_mean / 4);
				int** coded_area = 0;
				coded_area = new int* [x];
				for (int h = 0; h < (x); h++)
				{
					coded_area[h] = new int[l1];
				}

				for (int i = 0; i < x; i++)
				{
					for (int j = 0; j < l1; j++)
					{
						coded_area[x][l1] = s43.img[x][l1];
					}

				}

				coded_line = new double[l1];

				coded_line = Mean1R(x, l1, coded_area);
			}

			double* coded_mean = new double[9];
			for (int i = 0; i < 9; i++)
			{
				int row_size = bounds[i + 1] - bounds[i];
				double* coded_line_p = new double[row_size];
				for (int l = bounds[i]; l < bounds[i + 1]; l++)
				{
					coded_line_p[l] = coded_line[l];
				}
				coded_mean[i] = MeanR(row_size, coded_line_p);
				double th;


				delete[] coded_line_p;
			}
			delete[] max_mean_arr;
			delete[] coded_line;
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
