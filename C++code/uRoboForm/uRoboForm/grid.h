#pragma once
#include <vector>
#include <iostream>
#include <string>

using namespace std;

class Grid {
public:
    Grid(int** image, int imgRows, string orientation, std::pair<int, int> coord, double max_pos) {
        this->image = image;
        this->px_num = imgRows;
        this->orientation = orientation;
        this->im_loc = coord;
        if (orientation == "hor")
        {
            this->max_pos = coord.first + max_pos;
        }
        else 
        {
            this->max_pos = coord.second + max_pos;
        }
    }


    int** image;
    int imgRows;
    int px_num;
    std::string orientation;
    std::pair<int, int> im_loc;
    double max_pos;
};

