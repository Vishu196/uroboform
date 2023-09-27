#include "image_tool.hpp"
#include <algorithm>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <thread>

// direction could be 0..3, & op calculates higher values back to 0..3
#define MASK_DIRECTION 3

const char* PREVIEW_WINDOW_NAME = "pic_eval";

/* Constructor checks if wli-area is valid (dimension). May use values from source, if it is empty.
 * Sets the rect of wli-area and the dimension (x,y). */
ImageTool::ImageTool(user_parameter& p, Image_Source* source, const std::string filename_wli,
					const std::string subfolder_wli) : source(source), params(p),
					direction(params.direction & MASK_DIRECTION), rect_wli(params.area_wli)
{
	// if area is empty => set area to full picture
	if (rect_wli.empty())
		rect_wli = cv::Rect2i(0,0,source->get_x(),source->get_y());
	else
		check_area(rect_wli);

	if (params.debug_queue)
		wli_image_logger = new ImageLogger(params.parent_folder / subfolder_wli, filename_wli);
	cv::namedWindow(PREVIEW_WINDOW_NAME, cv::WINDOW_NORMAL);
	cv::setWindowProperty(PREVIEW_WINDOW_NAME, cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
}

void ImageTool::run()
{
	std::thread thr([&] {
		while (1)
		{
			// get image from source
			cv::Mat1b image;
			try
			{
				image = source->get_next();
			}
			catch (const std::exception&)
			{
				source_is_empty = true;
				// source is empty, exit thread
				cv_read.notify_one();
				break;
			}
			// step is distance in bytes to next pixel, i.e. 1 is 1byte/pixel
			// if more than 1 byte we have a color image which has to be converted
			if (!params.stop_at_stage || (params.stop_at_stage > user_parameter::E_STAGE_TOOL))
			{
				if (image.step.buf[1] != 1)
				{
					cv::Mat1b img2(image.rows, image.cols);
					cv::cvtColor(image, img2, cv::COLOR_BGR2GRAY);
					image = img2;
				}
				// handle roi and rotation
				{
					std::unique_lock<std::mutex> guard(buffer_mutex);
					cv_write.wait(guard, [&] {return buffer.size() <= max_buffered_images; });
					const FLOATPOINT shft = get_next_pixelshift(image);
					image = rotate(image(rect_wli), direction);
					buffer.push_back({ image, shft });
					std::cout << "tool image buffered " << buffer.size() << " ";
					std::cout << buffer.back().second << std::endl;
				}
			}
			cv_read.notify_one();
		}
	});
	thr.detach();
}

// check if specified area is correct and try to correct it, if values out of range or mistakenly swapped
void ImageTool::check_area(cv::Rect2i& area)
{
	const int32_t tmp_x = source->get_x();
	const int32_t tmp_y = source->get_y();

	// check if entered points are out of dimension of the image => throws error
	if ((area.x+area.width) > tmp_x)
		throw(std::runtime_error("Please check your input parameters: wli x1/x2 cannot be greater than x!"));

	if ((area.y+area.height) > tmp_y)
		throw(std::runtime_error("Please check your input parameters: wli y1/y2 cannot be greater than y!"));
}

// rotates 90 degrees counterclockwise (matlab's rot90())
cv::Mat1b ImageTool::rotate(const cv::Mat1b& image, uint8_t direction)
{
	direction &= MASK_DIRECTION;
	cv::Mat1b rotated_image(image);
	switch (direction)
	{
		// one time clockwise
		case 1:
			cv::rotate(image, rotated_image, cv::ROTATE_90_CLOCKWISE);
			break;
		// upside down
		case 2:
			cv::rotate(image, rotated_image, cv::ROTATE_180);
			break;
		// three times clockwise = one time counterclockwise
		case 3:
			cv::rotate(image, rotated_image, cv::ROTATE_90_COUNTERCLOCKWISE);
			break;
		// default: do not rotate
		default:
			rotated_image = image;
			break;
	}
	return rotated_image;
}

// public method for getting the next rotated, cropped image
cv::Mat1b ImageTool::get_next(void)
{
	if (source_is_empty && buffer.size()==0)
		throw(NoMorePictures("No more images from source"));

	cv::Mat1b image;
	{
		std::unique_lock<std::mutex> guard(buffer_mutex);
		// we wait until something in buffer and pixel shift is properly set
		cv_read.wait(guard, [&] {return ((buffer.size() != 0) && !std::isnan(buffer.front().second)) || (source_is_empty && buffer.size() == 0); });
		if (source_is_empty && buffer.size() == 0)
			throw(NoMorePictures("No more images from source"));
		std::cout << "Tool delivered with buffer " << buffer.size() << std::endl;
		pixel_shift_value = buffer.front().second;
		image = buffer.front().first;
		buffer.pop_front();
		if (pixel_shift_logger != nullptr)
			pixel_shift_logger->log(pixel_shift_value, "\n");
	}
	cv_write.notify_one();
	// write image for debugging
	if (wli_image_logger != NULL)
		wli_image_logger->log(image);
	if (params.show_preview)
	{
		static int i = 0;
		i += 1;
		cv::putText(image, std::to_string(i), cv::Point(10, 15),
			cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 255, 255));
		cv::imshow(PREVIEW_WINDOW_NAME, image);
		cv::waitKey(1);
	}
	return image;
}

ImageTool_externFDPX::ImageTool_externFDPX(Image_Source* source, user_parameter& params,
										   const std::string filename_wli, const std::string subfolder_wli)
										   : ImageTool(params, source, filename_wli, subfolder_wli)
{
	// first open file and copy all the data to buffer
	std::string line;
	std::ifstream file;
	file.open(params.pixel_step);
	FLOATPOINT tmp_fdpx;
	
	if (!file.is_open())
	{
		throw(std::runtime_error("Could not open FDPX file!"));	
	}
	// read first pixel_shift_value value, but do not push it into list
	std::getline(file,line);
	this->pixel_shift_value = std::stod(line);
	
	while (std::getline(file,line))
	{
		tmp_fdpx = std::stod(line);
		//std::cout << tmp_fdpx << std::endl;
		list_fdpx.push_back(tmp_fdpx);
	}
	file.close();
	run();
}

FLOATPOINT ImageTool_externFDPX::get_next_pixelshift(cv::Mat1b& speckle_image)
{
	(void)speckle_image;
	// throw an error if list is empty
	if (list_fdpx.empty())
		throw(std::runtime_error("No more FDPX values!"));
	
	const FLOATPOINT ret_fdpx = list_fdpx.front();
	list_fdpx.pop_front();
		
	return ret_fdpx;
}

/* Constructor checks speckle area and inits the rect dimensions. Gets first speckle image for image shift calculation.
 * Gets next images in loop until range_speckle is reached. By that, constructor gets the next needed
 * spekl-image for pixel_shift_value calculation. All wli images are pushed into a buffer. Fdpx is calculated and an
 * iterator is set to end of buffer, for checking if recalculation of pixel_shift_value is needed. This is the case,
 * after class has received a specified number of images, from it's source. The number is specified by
 * range_spekl. Pixel_Shift object calculates the mean value of pixel_shift_value over range_spekl images, e.g. over 50 images,
 * range_spekl==50. The pixel_shift_value value is only valid for 50 images and must be recalculated, if class has
 * received the next 50 images from source. This is checked by iterators, in method "get_next()". */
//params.delta_shift, params.upsampling, params.area_wli, params.area_speckle, params.direction, params.debug_queue
ImageTool_WLI_SPEKL::ImageTool_WLI_SPEKL(Image_Source* source, user_parameter& params,
			const std::string filename_wli, const std::string subfolder_wli,
			const std::string filename_spekl, const std::string subfolder_spekl)
		// init pixel_shift_value with zero, because static pixel_shift_value not needed here
		: ImageTool(params, source, filename_wli, subfolder_wli), rect_speckle(params.area_speckle),
			delta(params.delta_shift), threshold(params.shift_threshold)
{
	check_area(rect_speckle);
	if (params.debug_queue)
	{
		speckle_image_logger = new ImageLogger(params.parent_folder / subfolder_spekl, filename_spekl);
	}
	pixel_shift_logger = new FileLogger(params.parent_folder, "FDPX_c.csv");
	pixel_shift = new Pixel_Shift_Generic(rect_speckle.width, rect_speckle.height,params);
	run();
}

void ImageTool_WLI_SPEKL::find_best_rotation(cv::Mat1b& img_first, cv::Mat1b& img_last)
{
	// rotate one time, to find out the direction of the input images
	img_first = rotate(img_first, 1);
	img_last = rotate(img_last, 1);

	// Second pixel_shift_value value, to check against first one. Use rotated images
	FLOATPOINT fdpx2 = pixel_shift->calc_fdpx(img_first, img_last);

	/* Check both pixel_shift_value values, use abs. A negativ value means, image flows from opposite direction.
	* Check negativ value later. You may use a vector instead of two variables and get back the indices.
	* See next comments. */
	FLOATPOINT fdpx_result = std::abs(pixel_shift_value) < std::abs(fdpx2) ? fdpx2 : pixel_shift_value;

	// check if value is valid
	if (std::abs(fdpx_result) < threshold)
		throw(std::runtime_error("Specified speckle area wrong or speckle pattern did not flow into a direction. "
			"Please check input images and specified area or use a larger distance!\nfdpx = " +
			std::to_string(fdpx_result)));

	/* do not know which pixel_shift_value value is abs max
	* start is 0 (no rotation) or 1 (rotation)
	* it might be better to use a vector for both pixel_shift_value values and get the index */
	uint8_t start_direction = 0;
	if (fdpx_result == fdpx2)
		start_direction = 1;

	// if result is negative, rotate image two times, opposite direction
	// set member pixel_shift_value for the first images, until next calculation, automatic via get_next()
	if (fdpx_result < 0)
	{
		pixel_shift_value = std::abs(fdpx_result);
		start_direction += 2;
	}
	else
		pixel_shift_value = fdpx_result;

	// set found direction
	this->direction = start_direction;

	// reload image into buffer of class Pixel_Shift. Only necessary if it is not loaded
	if (start_direction != 1)
	{
		// write first spekl image to hdd
		// first rotate to start position (3) plus direction
		// the first speckle image is only relevant for debugging here
		img_first = rotate(img_first, start_direction + 3);
		// take last not rotated image_speckle and rotate it, to direction you have found
		img_last = rotate(image_speckle, start_direction);
		// load it into buffer of your feed object
		pixel_shift->reload_image(img_last);
	}

	// rotate wli images in buffer
	for (auto& image_in_buf : buffer)
		image_in_buf.first = rotate(image_in_buf.first, start_direction);
}

FLOATPOINT ImageTool_WLI_SPEKL::get_next_pixelshift(cv::Mat1b& image)
{
	if (first_image)
	{
		first_image = false;
		cv::Mat1b speckle_image = rotate(image(rect_speckle), direction);
		if (speckle_image_logger != NULL)
			speckle_image_logger->log(speckle_image);

		pixel_shift->reload_image(speckle_image);
	}
	else if (++skipped_images >= delta)
	{
		cv::Mat1b speckle_image = rotate(image(rect_speckle), direction);
		if (speckle_image_logger != NULL)
			speckle_image_logger->log(speckle_image);

		const FLOATPOINT shift = pixel_shift->calc_fdpx(speckle_image, skipped_images);
		std::cout << "New Shift " << shift << " for elements in buffer of size " << buffer.size() << std::endl;
		auto it = buffer.rbegin();
		while (--skipped_images)
		{
			it->second = shift;
			++it;
		}
		cv_read.notify_one();
		return shift;
	}
	return std::numeric_limits<FLOATPOINT>::quiet_NaN();
}
