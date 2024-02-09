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

Grid** grid_pos02::checkGrid(Grid** grids, int x, int y)
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

				for (int i_fill = 0; i_fill < ; i_fill++)
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

				else if((c11 && c22 && c33) || (c44 && c55 && c66))
				{
					double new_mp = MeanR(r, max_pos_arr) + (s43.grids[row][col].max_pos.front());
					s43.grids[row][col].max_pos.push_front(new_mp);
				}					
			}
		}
	}
	return s43.grids;
}

