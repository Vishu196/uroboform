#pragma once
#include <vector>
#include <iostream>
#include <string>

class Grid {
public:
    Grid(std::vector<int> image, std::string orientation, std::pair<int, int> coord, int max_pos) {
        this->image = image;
        this->px_num = image.size();
        this->orientation = orientation;
        this->im_loc = coord;
        if (orientation == "hor") {
            this->max_pos = std::make_pair(coord.first + max_pos, coord.second);
        }
        else {
            this->max_pos = std::make_pair(coord.first, coord.second + max_pos);
        }
    }

private:
    std::vector<int> image;
    int px_num;
    std::string orientation;
    std::pair<int, int> im_loc;
    std::pair<int, int> max_pos;
};

