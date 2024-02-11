#pragma once
#include <vector>
#include <iostream>
#include <string>

using namespace std;

class Grid {
    public:
    Grid(int** image, int imgRows, string orientation, list<int> coord, list<double> max_pos)
    {
        this->image = image;
        this->px_num = imgRows;
        this->orientation = orientation;
        this->im_loc = coord;
            if (orientation == "hor")
            {
                this->max_pos.front() = coord.front() + max_pos.front();
                this->max_pos.back() = coord.front() + max_pos.back();
            }
            else
            {
                this->max_pos.front() = coord.back() + max_pos.front();
                this->max_pos.back() = coord.back() + max_pos.back();
            }
        
    }

    Grid() = default;

    int** image;
    int imgRows;
    int px_num;
    std::string orientation;
    list<int> im_loc;
    list<double> max_pos;
};

