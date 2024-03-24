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
}



struct stage56 grid_pos03::Execute(void)
{

}
