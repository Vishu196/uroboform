#include <thread>
#include "image_collector.hpp"
#include "write_file.hpp"

/* Constructor checks for valid user input and inits the image buffer, via add_handler().
 * User can force the constructor to load black images into buffer. This way, bases of pixel
 * columns are decreased. This feature should only be used to check result (against matlab). */
ImageCollector::ImageCollector(ImageTool* tool, user_parameter& p)
	: img_tool(tool), params(p)
{
	// more than y preload images would make no sense
	p.preload_black = std::min(p.preload_black, img_tool->get_y());

	// Init counter and offset for first image 
	counter = std::floor(img_tool->get_pixel_shift_value());
	if (counter == 0)
		counter++;
	// offset is the shift (pixel_shift_value), relativ to the previous image.
	// for the first image, this shift is only an adoption
	offset = counter;
	std::thread thr([&] {
		{
			std::unique_lock<std::mutex> guard(buffer_mutex);
			init();
		}
		while (1)
		{
			{
				std::unique_lock<std::mutex> guard(buffer_mutex);
				cv_write.wait(guard, [&] {return counter_x == img_tool->get_x(); });
				try
				{
					counter_x = 0;
					delete_handler();
				}
				catch (const std::exception&)
				{
					end_calculation = true;
					break;
				}
			}
			cv_read.notify_one();
		}
		cv_read.notify_one();
	});
	thr.detach();
}

void ImageCollector::push_image(const cv::Mat& img)
{
	if (!params.stop_at_stage || (params.stop_at_stage > user_parameter::E_STAGE_COLLECTOR))
	{
		buffer.push_back(img);
		std::cout << "Collector added " << buffer.size() << std::endl;
	}
}

void ImageCollector::init()
{
	// preload buffer with black images, only for testing
	if (params.preload_black)
	{
		const cv::Mat default_pic = cv::Mat::zeros(img_tool->get_y(), img_tool->get_x(), OCV_MATTYPE);

		for (uint32_t i=0; i< params.preload_black; i++)
		{
			push_image(default_pic);
			offset_array.push_back(1);
		}
		// each preload image has distance 1
		sum_offset = params.preload_black;
		// store pixel shift of black images
		fdpx_vec.push_back(std::make_pair(1, params.preload_black));
	}
	
	// add as many images as needed
	add_handler();
}

/* store value as next pixel shift in buffer */
void ImageCollector::store_pixel_shift(const FLOATPOINT value)
{
	// only increase the count value (second value of vector)
	if (fdpx_vec.size() && (fdpx_vec.back().first == value))
		fdpx_vec.back().second++;
	else
		fdpx_vec.push_back(std::make_pair(value, 1));
}

/* Sum all pixel shift and return average value for one image */
FLOATPOINT ImageCollector::get_avg_pixel_shift(void)
{
	FLOATPOINT fdpx_sum = 0;
	for (const auto& pixel_shift_value : fdpx_vec)
		fdpx_sum += pixel_shift_value.first * pixel_shift_value.second;
	
	return fdpx_sum / buffer.size();
}

/* add_handler() pushes as many images as needed to buffer. How many
 * images are needed depends on the shift (pixel_shift_value) and the y size of the
 * images, e.g. y_max = 640, pixel_shift_value = 2 => buffer must contain 320 images.
 * In real, pixel_shift_value is not a const value. The member offset contains
 * the shift for the image, relative to the previous image. Method checks,
 * if the sum of all offset (shift) values plus the next shift (offset) is
 * less than or equal y_max. If yes, image and it's offset is pushed
 * into buffer. Offset is added to "sum_offset". "counter" and "offset"
 * for the next image are calculated. Example for this calculation:
 * pixel_shift_value = 1,5
 * counter	| offset
 * 1.0		| 1
 * 1,5		| 2
 * 1.0		| 1
 * 1.5		| 2
 * 1.0		| 1
 * etc.					*/
void ImageCollector::add_handler(void)
{
	// get y dimension of the images (all images have same size)
	const uint32_t y = get_y();
	// check if the buffer can contain the next image
	while ((sum_offset+offset)<=y)
	{
		// first get the pixelstep for the image you want to get
		const FLOATPOINT tmp_fdpx = img_tool->get_pixel_shift_value();
		store_pixel_shift(tmp_fdpx);
		// get the next image from source (ImageTool)
		try
		{
			const cv::Mat img = img_tool->get_next();
			push_image(img);
		}
		catch (const std::exception&)
		{
			end_calculation = true;
			break;
		}
		// add the offset of this image to sum
		sum_offset += offset;
		// push back the offset
		offset_array.push_back(offset);
		/* calculate new counter and offset value, by getting the pixelstep to the next image
		 * ATTENTION: the next pixelstep might not be the same as above, it may change in get_next() call
		 * when you get the image */
		counter = counter - offset + img_tool->get_pixel_shift_value();
		offset = round(counter);
	}
}

/* delete_handler() may delete images from buffer. This depends on the
 * front of "offset_array". A offset of '0' or '1' at front means:
 * image has no more information, "get_pixel_column(...)" has returned all
 * information => pop. If there are more images in buffer and the offset of
 * the next image is '0', this image also does not contain new information
 * => delete it.
 * If front of offset_array is greater than '1' (else case), front is decremented
 * by '1'. Front image still contains more information, do not delete.
 * The sum of offsets is decremented at each call => new image could be added by
 * calling "add_handler()". */ 
void ImageCollector::delete_handler(void)
{
	// always decrement sum of offset, cause offset_array.front is decremented or deleted each time
	sum_offset--;

	/* If queue is empty => throw an error. Only possible, because add_handler
	 * fills buffer to limit (y), not possible for only one single image
	 */
	if (offset_array.size()==0 || buffer.size()==0)
		throw(std::runtime_error("No more images in delete!"));
	// look if there is something to delete
	if (offset_array.front() < 2)
	{
		do
		{
			// image might not be buffered, so check
			if (buffer.size())
				buffer.pop_front();
			offset_array.pop_front();
			
			/* if still black images available reduce counter
			   used for offsetting the pixel column start */
			if (params.preload_black)
				--params.preload_black;
				
			// delete pixel_shift_value value
			fdpx_vec.front().second--;
			if (fdpx_vec.front().second == 0)
				fdpx_vec.pop_front();
		} while (buffer.size() > 0 && offset_array.front() == 0);
	}
	else
		offset_array.front() -= 1;
	add_handler();
}

/* Public method "get_pixel_column(...)" writes a pixel column to it's
 * param. vector "output". It returns a bool value, to inform the calling
 * function, if a row is processed. The row to start with is always at
 * bottom of the first image in buffer. This is defined by the direction of
 * the shift, new pixels appears at the bottom of the next image (rotate it
 * to this direction in class "ImageTool").
 * Which row to choose, for each image, is defined by the offset (shift)
 * of the image, e.g. y_max = 640, offset = 2 => row 639
 * (index 638). It is the same for the next image, but offset values in array
 * are relativ to the previous image => add the offset to the offset of the 
 * previous image. Sum up is done in var. "total_offset". 
 * Example:
 * image#	| offset	| total_offset	| row
 * 1		| 2			| 2				| 639
 * 2		| 2			| 4				| 637
 * 3		| 2			| 6				| 635
 * etc.
 * For choosing a pixel, the x position (column) is needed. The x position is
 * incremented at the end of each call, "counter_x". If the counter has reached
 * the x dimension of the images, it will be rest (to zero) => row is processed
 * => call delete and add (in this order). */
uint32_t ImageCollector::get_pixel_column(std::vector<FLOATPOINT>& output)
{
	// dimension cannot change during runtime
	const uint32_t x = img_tool->get_x();

	cv_write.notify_one();

	// clear the output
	output.clear();
	output.reserve(buffer.size());

	const uint32_t y = img_tool->get_y();
	// init total_offset to preload_black, which is zero by default
	uint32_t total_offset = params.preload_black;
	{
		std::unique_lock<std::mutex> guard(buffer_mutex);
		// we wait until something in buffer and not fully processed line
		// end handling is a little more complicated as we need to notify from thread, so go via variable
		cv_read.wait(guard, [&] {return (((buffer.size() != 0)&&(counter_x<x))||(end_calculation)); });
		if (end_calculation)
			throw(NoMorePictures("End of calculation"));
		for (uint32_t i = params.preload_black; i < buffer.size(); i++)
		{
			// add offset of image to total_offset, offsets are relative and must be summed up	
			total_offset += offset_array[i];
			// calculate the row to use (for this image)
			const uint32_t tmp_y = y - total_offset;
			//std::cout << "tmp_y = " << tmp_y << " | total_offset = " << total_offset << "y Px = " << y << std::endl;
			// get the pixel value
			const FLOATPOINT tmp_value = buffer[i].at<uchar>(tmp_y, counter_x);
			// and push to output vector
			output.push_back(tmp_value);
		}

		// increment counter_x for next call
		++counter_x;

		if (offset_array.size() == 0 || ((!params.stop_at_stage || params.stop_at_stage > user_parameter::E_STAGE_COLLECTOR) && buffer.size() == 0))
			throw(std::runtime_error("No more images in get_pixel_col!"));
	}

	return counter_x;
}

// Instead of this method use a get method. Get a vector and sum it in caller
uint32_t ImageCollector::get_offset_sum(const uint32_t index)
{
	if (index > offset_array.size())
	{
		throw(std::runtime_error("Wrong index for offset_array. Array may smaller than expected!"));
	}
	//return std::accumulate(std::get<0>(offset_array.begin()), std::get<0>(offset_array.begin())+index, 0);
	uint32_t sum=0;
	for (uint32_t i=0; i<index; i++)
		sum += offset_array[i];

	return sum;
}
