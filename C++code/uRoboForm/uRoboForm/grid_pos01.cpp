#include "grid_pos01.h"

static struct stage32 s32;
static struct stage34 s34;

static void buffer_s32_init() 
{
	s32.cut_hor = {};
	s32.cut_ver = {};

	s32.cut_hor_s = 0;
	s32.cut_ver_s = 0;
	
	s32.img = new int* [1080];;// new int[1080][1440];
	for (int h = 0; h < 1080; h++)
	{
		s32.img[h] = new int[1440];
	}

	s32.img2 = new int* [540];;// new int[1080][1440];
	for (int h = 0; h < 540; h++)
	{
		s32.img2[h] = new int[720];
	}
}

static void buffer_s34_init()
{
	s34.img = new int* [1080];
	for (int h = 0; h < 1080; h++)
	{
		s34.img[h] = new int[1440];
	}

}

static void buffers_init(void)
{
	buffer_s32_init();
	buffer_s34_init();
}

grid_pos01::grid_pos01(struct stage23 s23)
{
	buffers_init();

	s32.cut_hor_s = s23.cut_hor_s;
	s32.cut_ver_s = s23.cut_ver_s;

	std::copy(s23.cut_hor.begin(), s23.cut_hor.end(), std::back_inserter(s32.cut_hor));
	std::copy(s23.cut_ver.begin(), s23.cut_ver.end(), std::back_inserter(s32.cut_ver));

	s32.imgRows = s23.imgRows;
	s32.imgCols = s23.imgCols;
	int rows2 = s32.imgRows / 2;
	int cols2 = s32.imgCols / 2;

	for (int i = 0; i < s23.imgRows; i++)
	{
		memcpy(s32.img[i], s23.img[i], (s23.imgCols * sizeof(int)));
	}

	for (int i = 0; i < (rows2); i++)
	{
		memcpy(s32.img2[i], s23.img2[i], (cols2 * sizeof(int)));
	}
}
 
int grid_pos01::Execute(void) 
{
	if ((s32.cut_ver_s >= 2) && (s32.cut_hor_s >= 2))
	{
		if (s32.cut_ver.front() * 2 < 10)
		{
			s32.cut_ver.pop_front();
		}

		if (s32.cut_hor.front() * 2 < 10)
		{
			s32.cut_hor.pop_front();
		}

		s32.cut_hor.push_front(0);
		s32.cut_ver.push_front(0);

		s32.cut_hor.push_back(s32.imgRows / 2);
		s32.cut_ver.push_back(s32.imgCols / 2);

		int* cut_hor_arr = new int[s32.cut_hor.size()]();
		std::copy(s32.cut_hor.begin(), s32.cut_hor.end(), cut_hor_arr);

		int* cut_ver_arr = new int[s32.cut_ver.size()]();
		std::copy(s32.cut_ver.begin(), s32.cut_ver.end(), cut_ver_arr);

		int** grids = 0;
		grids = new int* [500];
		for (int h = 0; h < 500; h++)
		{
			grids[h] = new int[800];
		}

		memset(grids, 0, sizeof(grids[0][0]) * s32.cut_hor_s * s32.cut_ver_s);

		int image_size = s32.imgCols * s32.imgRows;
		double five_percent = image_size * 0.05;

		for (int row = 0; row < s32.cut_hor.size()- 1; row++)
		{
			for (int col = 0; col < s32.cut_ver.size() - 1; col++)
			{
				int x1 = cut_hor_arr[row] * 2;
				int x2 = (cut_hor_arr[row + 1]) * 2;
				int y1 = cut_ver_arr[col] * 2;
				int y2 = (cut_ver_arr[col + 1]) * 2;

				int** grid0 = 0;
				grid0 = new int* [x2-x1];
				for (int h = 0; h < (x2-x1); h++)
				{
					grid0[h] = new int[y2-y1];
				}

				for (int x = x1; x < x2; x++)
				{
					for (int y = y1; y < y2; y++)
					{
						grid0[x-x1][y-y1] = s32.img[x][y];
					}
			    }



			}

			

		}
	}
	return 0;
}
