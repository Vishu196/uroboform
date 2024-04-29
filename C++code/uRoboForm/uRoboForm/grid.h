#pragma once
#include <vector>
#include <iostream>
#include <string>


class Grid
{
    public:
    Grid(cv::Mat image, bool is_hor, std::vector<int> coord, std::vector<double> max_pos)
    {
        this->image = image;
        this->px_num = image.rows * image.cols;
        this->is_hor = is_hor;
        this->im_loc = coord;
            if (is_hor)
            {
                for (int i = 0; i < max_pos.size(); i++)
                {
                    this->max_pos.push_back(max_pos[i] + coord[0]);
                }
                max_pos.resize(max_pos.size());
            }
            else
            {
                for (int i = 0; i < max_pos.size(); i++)
                {
                    this->max_pos.push_back(max_pos[i] + coord[1]);
                }
                max_pos.resize(max_pos.size());            
            }
    }

    Grid() = default;

    cv::Mat image;
    int px_num =0;
    bool is_hor;
    std::vector<int> im_loc;
    std::vector<double> max_pos;
};

