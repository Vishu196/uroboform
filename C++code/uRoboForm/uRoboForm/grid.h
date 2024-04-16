#pragma once
#include <vector>
#include <iostream>
#include <string>


class Grid
{
    public:
    Grid(cv::Mat image, std::string orientation, std::vector<int> coord, std::vector<double> max_posi)
    {
        this->image = image;
        this->px_num = image.rows * image.cols;
        this->orientation = orientation;
        this->im_loc = coord;
            if (orientation == "hor")
            {
                for (int i = 0; i < max_posi.size(); i++)
                {
                    this->max_pos.push_back(max_posi[i] + coord[0]);
                }
                max_pos.resize(max_posi.size());
            }
            else
            {
                for (int i = 0; i < max_posi.size(); i++)
                {
                    this->max_pos.push_back(max_posi[i] + coord[1]);
                }
                max_pos.resize(max_posi.size());            
            }
        
    }

    Grid() = default;

    cv::Mat image;
    int px_num =0;
    std::string orientation;
    std::vector<int> im_loc;
    std::vector<double> max_pos;
};

