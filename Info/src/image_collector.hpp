#ifndef IMAGE_COLLECTOR_HPP
#define IMAGE_COLLECTOR_HPP

#include "image_tool.hpp"

/* Class ImageCollector gets images from source "ImageTool". Its task is
 * to return pixel columns to the main algorithm, class CoreAlgorithm in file
 * algorithm.hpp. The images from source are pushed into a buffer via add_handler().
 * Images are deleted from buffer via delete_handler(). The goal is to get all valid
 * pixel columns of the image stack in source. The pixels of a column are from many images
 * and the row to choose depends on the pixel offset "pixel_shift_value" (shift). The images shift up,
 * there are new values in bottom row(s) of the next image. The pixel to return depend on
 * the shift in the image stack.
 * Example:
 * pixel_shift_value == 2; VGA dimension
 * row image1 = 639
 * row image2 = 637
 * row image3 = 635 
 * etc.
 * The method get_pixel_column() returns one pixel column for each pixel in x direction.
 * After all these pixel columns have returned. Add and delete is called. pixel_shift_value == 2 =>
 * do not delete an image. Next round is:
 * row image1 = 640
 * row image2 = 638
 * row image3 = 636
 * etc.
 * After this round the first image is deleted.
 * Detailed explanation of all methods in cpp-file.*/
class ImageCollector
{
	public:
		ImageCollector(ImageTool * tool, user_parameter& params);
		virtual ~ImageCollector(void) {}
		uint32_t get_y(void) {return img_tool->get_y();}
		uint32_t get_x(void) {return img_tool->get_x();}
		/* Sum all pixel shift and return average value for one image */
		FLOATPOINT get_avg_pixel_shift(void);
		uint32_t get_pixel_column(std::vector<FLOATPOINT>& output);
		uint32_t get_offset_sum(const uint32_t index);
		void init(void);

	private:
		void add_handler(void);
		void delete_handler(void);
		/* store value as next pixel shift in buffer */
		void store_pixel_shift(const FLOATPOINT value);
		void push_image(const cv::Mat& img);

		// the source to get images from
		ImageTool* img_tool;
		user_parameter& params;
		// image stack
		std::deque<cv::Mat1b> buffer;

		// offset is the pixel shift from the previous image (in pixel, integer value)
		int32_t offset;
		// counter is the exact shift to the previous image, with all left from not yet accounted shifts from previous shifts
		FLOATPOINT counter;
		// sum_offset is the sum of all offsets, it is counted up and down, if add or delete is called
		uint32_t sum_offset=0;
		// buffer for offset values
		std::deque<int32_t> offset_array;
		// counter_x for counting up to max x size of the images
		uint32_t counter_x = 0;
		// pixel shift of all buffered images. First element is actual pixel shift of next image,
		// second element is how often this value repeats
		std::list<std::pair<const FLOATPOINT, uint32_t>> fdpx_vec;
		std::mutex buffer_mutex;
		std::condition_variable cv_read, cv_write;
		bool end_calculation = false;
};

#endif
