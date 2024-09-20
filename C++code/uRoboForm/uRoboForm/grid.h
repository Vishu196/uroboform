#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "constants.h"

//This class has functions and constructor to create the Grid type object used to perform calculations in the 
// algorithm after finding the edges of grids

class Grid
{
private:
    std::vector<double> max_pos;
public:
    //two contructors are use to initialize the grid class as it is called from two different classes in the 
    //algorithm and initialized accordingly
    Grid(cv::Mat img, bool horizontal) : image(img), px_num(img.rows * img.cols), is_hor(horizontal) {}
    Grid(cv::Mat image, bool is_hor, std::vector<int> coord, std::vector<double> maxPos)
    {
        this->image = image;
        get_px_num(px_num);
        this->is_hor = is_hor;
        this->im_loc = coord;
        addmaxPos(maxPos);
    }

    Grid() = default;

    void get_px_num(int &px_num) { px_num = image.rows * image.cols; }

    //Function used to add the value of max_pos vector based on the orientation
    void addmaxPos(const std::vector<double>& maxPos)
    {
       if (is_hor)
       {
            for (int i = 0; i < maxPos.size(); i++)
            {
                max_pos.push_back(maxPos[i] + im_loc[0]);
            }
       }
        else
        {
            for (int i = 0; i < maxPos.size(); i++)
            {
                max_pos.push_back(maxPos[i] + im_loc[1]);
            }
        }
    }

    //function to get the mask_pos value based on the oreintation
    int get_mask_pos(int row, int col, size_t i_max) const
    {
        int s_index = 0;
        int mask_pos = 0;

        if (is_hor)
        {
            if (row == 0)
            {
                size_t i = i_max + 6;
                s_index = (int)(i - max_pos.size());
            }
            else
            {
                s_index = (int)i_max;
            }
            int r = row - 1;
            mask_pos = (s_index * 200) + 350 + (r * grid_height);
        }
        else
        {
            if (col == 0)
            {
                size_t i = i_max + 8;
                s_index = (int)(i - max_pos.size());
            }
            else
            {
                s_index = (int)i_max;
            }
            int c = col - 1;
            mask_pos = (s_index * 200) + 350 + (c * grid_width);
        }

        return mask_pos;
    }
    
    std::vector<double>& get_max_pos() { return max_pos; }
    const std::vector<double>& get_max_pos() const { return max_pos; }
    cv::Mat image;
    int px_num =0;
    bool is_hor;
    std::vector<int> im_loc;
    
};

