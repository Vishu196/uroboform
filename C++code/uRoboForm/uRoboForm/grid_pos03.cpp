#include "grid_pos03.h"

static struct stage54 s54;
static struct stage56 s56;

static void buffer_s54_init()
{
	s54.gridRows = 0;
	s54.gridCols = 0;
	s54.grids = new Grid * [3];
	for (int h = 0; h < 3; h++)
	{
		s54.grids[h] = new Grid[3];
	}

	s54.index = 0;
	s54.k = 0.0;
	s54.ind_ori = "";
	s54.grid_wid = 0;
	s54.grid_ht = 0;

	s54.look_up = new list<int>*[200];
	for (int i = 0; i < 200; i++)
	{
		s54.look_up[i] = new list<int>[2];
	}
}

static void buffer_s56_init()
{
	s56.gridRows = 0;
	s56.gridCols = 0;
	s56.grids = new Grid * [3];
	for (int h = 0; h < 3; h++)
	{
		s56.grids[h] = new Grid[3];
	}

	s56.index = 0;
	s56.k = 0.0;
	s56.ind_ori = "";
	s56.xi = 0.0;
	s56.zi = 0.0;
}

static void buffers_init(void)
{
	buffer_s54_init();
	buffer_s56_init();
}

grid_pos03::grid_pos03(struct stage45 s45)
{
	buffers_init();

	s54.gridRows = s45.gridRows;
	s54.gridCols = s45.gridCols;

	for (int i = 0; i < s45.gridRows; i++)
	{
		memcpy(s54.grids[i], s45.grids[i], (s45.gridCols * sizeof(Grid)));
	}

	s54.index = s45.index;
	s54.k = s45.k;
	s54.ind_ori = s45.ind_ori;
	s54.grid_wid = s45.grid_wid;
	s54.grid_ht = s45.grid_ht;

	for (int i = 0; i < 200; i++)
	{
		memcpy(s54.look_up[i], s45.look_up[i], (2 * sizeof(int)));
	}
}

int grid_pos03::get_mask_pos(Grid field, int row, int col, int i_max, int grid_wid, int grid_ht)
{
	size_t s_index = 0;
	int mask_pos = 0;

	if (field.orientation == "hor")
	{
		if (row == 0)
		{
			int i = i_max + 6;
			s_index = i - field.max_pos.size();
		}
		else
		{
			s_index = i_max;
		}
		size_t r = row-1;
		mask_pos = s_index * 200 + 350 + r * grid_ht;
	}
	else
	{
		if (col == 0)
		{
			int i = i_max + 8;
			s_index = i - field.max_pos.size();
		}
		else
		{
			s_index = i_max;
		}
		size_t c = col - 1;
		mask_pos = s_index * 200 + 350 + c * grid_wid;
	}

	return mask_pos;
}

double grid_pos03::MeanR(int rows, double* mean0)
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

struct stage56 grid_pos03::Execute(void)
{
	const float px_size = 3.45;
	list<list<double>> center_hor;
	list<list<double>> center_ver;
	list<int> look_el;
	int P;

	if ((s54.index >= 0) && (s54.index < 200))
	{
		if (s54.grids[1][1].orientation == "hor")
		{
			look_el.push_back(s54.look_up[s54.index][0].front());
			look_el.push_back(s54.look_up[s54.index][0].back());
		}
		else
		{
			look_el.push_back(s54.look_up[s54.index][1].front());
			look_el.push_back(s54.look_up[s54.index][1].back());
		}
	}
	else
	{
		look_el.push_back(0);
		look_el.push_back(0);
	}

	for (int row = 0; row < s54.gridRows; row++)
	{
		for (int col = 0; col < s54.gridCols; col++)
		{
			list<double> center;
			Grid field = s54.grids[row][col];

			for (int i_max = 0; i_max < 8; i_max++)
			{
				int mask_pos = get_mask_pos(field, row, col, i_max, s54.grid_wid, s54.grid_ht);
				if (field.orientation == "hor")
				{
					P = mask_pos + look_el.front();
				}
				else
				{
					P = mask_pos + look_el.back();
				}
				// to do - access i_maxth element from the max_pos list
				center.push_back((-P * s54.k) + field.max_pos[i_max]() * px_size);
			}

			double* center_arr = new double[center.size()]();
			std::copy(center.begin(), center.end(), center_arr);

			double cen_mean = MeanR(center.size(), center_arr);

			list<double> t1;
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
		}
	}

	double xi;
	double zi;

	s56.gridRows = s54.gridRows;
	s56.gridCols = s54.gridCols;
	s56.grids = s54.grids;
	s56.index = s54.index;
	s56.k = s54.k;
	s56.ind_ori = s54.ind_ori;
	s56.xi = xi;
	s56.zi = zi;
}
