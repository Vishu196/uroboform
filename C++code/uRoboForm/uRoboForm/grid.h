#pragma once
#include <vector>
#include <iostream>
#include <string>

using namespace std;

class Grid {
    public:
    Grid(Mat image, string orientation, vector<int> coord, vector<double> max_pos)
    {
        this->image = image;
        this->px_num = image.rows * image.cols;
        this->orientation = orientation;
        this->im_loc = coord;
            if (orientation == "hor")
            {
                for (double& d : max_pos)
                    d += coord[0];
                //transform(max_pos.begin(), max_pos.end(), max_pos.begin(), [&](double x) {x + coord[0]; });
                this->max_pos = max_pos;
            }
            else
            {
                for (double& d : max_pos)
                    d += coord[1];
                //transform(max_pos.begin(), max_pos.end(), max_pos.begin(), [&](double x) {x + coord[1]; });
                this->max_pos = max_pos;
            }
        
    }

    Grid() = default;

    Mat image;
    int px_num =0;
    std::string orientation;
    vector<int> im_loc;
    vector<double> max_pos;
};

