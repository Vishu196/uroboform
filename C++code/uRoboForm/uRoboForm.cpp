#include <iostream>

#include <opencv2/opencv.hpp>

#include "xtensor/xarray.hpp"
#include "xtensor/xio.hpp"
#include "xtensor/xadapt.hpp"


cv::Mat xarray_to_mat_elementwise(xt::xarray<float> xarr)
{
    int ndims = xarr.dimension();
    assert(ndims == 2 && "can only convert 2d xarrays");
    int nrows = xarr.shape()[0];
    int ncols = xarr.shape()[1];
    cv::Mat mat(nrows, ncols, CV_32FC1);
    for (int rr = 0; rr < nrows; rr++)
    {
        for (int cc = 0; cc < ncols; cc++)
        {
            mat.at<float>(rr, cc) = xarr(rr, cc);
        }
    }
    return mat;
}

xt::xarray<float> mat_to_xarray_elementwise(cv::Mat mat)
{
    int ndims = mat.dims;
    assert(ndims == 2 && "can only convert 2d xarrays");
    int nrows = mat.rows;
    int ncols = mat.cols;
    xt::xarray<float> xarr = xt::empty<float>({ nrows, ncols });
    for (int rr = 0; rr < nrows; rr++)
    {
        for (int cc = 0; cc < ncols; cc++)
        {
            xarr(rr, cc) = mat.at<float>(rr, cc);
        }
    }
    return xarr;
}



int main()
{
    int nrows = 2, ncols = 3;
    float data[150];
    for (int i = 0; i < nrows * ncols; i++)
    {
        data[i] = .1 * i;
    }

    cv::Mat mat(nrows, ncols, CV_32FC1, data, 0);
    std::cout << "mat:\n" << mat << std::endl;

    xt::xarray<float> xarr = xt::adapt(
        (float*)data, nrows * ncols, xt::no_ownership(), std::vector<std::size_t> {nrows, ncols});
    std::cout << "xarr:\n" << xarr << std::endl;

    cv::Mat mat2_ew = xarray_to_mat_elementwise(xarr);
    std::cout << "mat2_ew (from xt::xarray):\n" << mat2_ew << std::endl;

    xt::xarray<float> xarr2_ew = mat_to_xarray_elementwise(mat);
    std::cout << "xarr2_ew (from cv::mat):\n" << xarr2_ew << std::endl;

    return 0;
}