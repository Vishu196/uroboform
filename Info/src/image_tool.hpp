#ifndef IMAGE_TOOL_HPP
#define IMAGE_TOOL_HPP

#include <opencv2/opencv.hpp>
#include "image_src.hpp"
#include "pixel_shift.hpp"

class ImageTool
{
	public:
		virtual ~ImageTool(void) {delete wli_image_logger;}
		// ask for dimensions, values from Image_Source => not abstract
		//pic_dimension get_dimension(void){return {roi.width, roi.height};}
		uint32_t get_x(void) const {
			switch (direction)
			{
			case 1:
			case 3:
				return rect_wli.height;
				break;
			default:
				return rect_wli.width;
				break;
			}
		}
		uint32_t get_y(void) const {
			switch (direction)
			{
			case 1:
			case 3:
				return rect_wli.width;
				break;
			default:
				return rect_wli.height;
				break;
			}
		}
		virtual cv::Mat1b get_next(void);
		FLOATPOINT get_pixel_shift_value(void) const {return pixel_shift_value;}
		
	protected:
		ImageTool(user_parameter& params, Image_Source* source, const std::string filename_wli = "input_wli",
				  const std::string subfolder_wli = "Input/WLI");

		void run();

		Image_Source* source;
		user_parameter& params;
		uint8_t direction;
		FLOATPOINT pixel_shift_value;
		cv::Rect rect_wli;
		const unsigned max_buffered_images=50;
		std::deque<std::pair<cv::Mat1b, FLOATPOINT>> buffer;
		std::mutex buffer_mutex;
		std::condition_variable cv_read, cv_write;

		// debug output
		ImageLogger* wli_image_logger = nullptr;
		FileLogger* pixel_shift_logger = nullptr;

		virtual FLOATPOINT get_next_pixelshift(cv::Mat1b& speckle_image) = 0;
		cv::Mat1b rotate(const cv::Mat1b& image, uint8_t direction);
		void check_area(cv::Rect2i& area);
		bool source_is_empty = false;
};

/* Class ImageTool can modify the image from "ImageSource", e.g. rotate, crop.
 * It saves rotated, cropped dimensions (x and y)
 * It wants an area of wli pattern. Use this class for input images without speckle pattern.
 * It also has the pixel_shift_value (feed/pixelstep) value and can return it.
 */
class ImageTool_constFDPX : public ImageTool
{
	public:
		ImageTool_constFDPX(Image_Source* source, const FLOATPOINT px_shft,
							user_parameter& params, const std::string filename_wli = "input_wli",
							const std::string subfolder_wli = "Input/WLI")
							: ImageTool(params, source, filename_wli, subfolder_wli)
		{
			pixel_shift_value = px_shft;
			run();
		}

		virtual ~ImageTool_constFDPX(void){}
		
	protected:
		// return the constant value
		FLOATPOINT get_next_pixelshift(cv::Mat1b& speckle_image) override
		{
			(void)speckle_image;
			return pixel_shift_value;
		}
};

class ImageTool_externFDPX : public ImageTool
{
	public:
		ImageTool_externFDPX(Image_Source* source, user_parameter& params,
							 const std::string filename_wli = "input_wli", const std::string subfolder_wli = "Input/WLI");

		virtual ~ImageTool_externFDPX(void){}
		
	private:
		std::list<FLOATPOINT> list_fdpx;
		FLOATPOINT get_next_pixelshift(cv::Mat1b& speckle_image) override;
};

/* Class ImageTool_WLI_SPEKL is derived from class above. It has a extension for spekl pattern =>
 * needs one more rect area for spekl. For calulating pixel_shift_value (pixelstep) class "Pixel_Shift", in file "feed.hpp",
 * is used. Class feed needs two images from stack for calculation. The images used for pixel_shift_value calculation must
 * be apart from each other, e.g. image #1 and #51. The param range_spekl specifies this difference/delta.
 * To fullfill this behaviour, the class needs a buffer for wli-images, cause source does not have a buffer,
 * source only returns current image from camera or newest from folder. */
class ImageTool_WLI_SPEKL : public ImageTool
{
	public:
		ImageTool_WLI_SPEKL(Image_Source* source, user_parameter& params,
					const std::string filename_wli = "input_wli", const std::string subfolder_wli = "Input/WLI",
					const std::string filename_spekl = "input_spekl", const std::string subfolder_spekl = std::string("Input/SPEKL"));
		void find_best_rotation(cv::Mat1b& img_first, cv::Mat1b& img_last);
		virtual ~ImageTool_WLI_SPEKL(void) {delete pixel_shift; delete speckle_image_logger;}

	private:
		// area for spekl-images
		cv::Rect rect_speckle;
		cv::Mat1b image_speckle;
		Pixel_Shift* pixel_shift;
		unsigned skipped_images=0;
		unsigned delta;
		FLOATPOINT threshold;
		ImageLogger* speckle_image_logger = nullptr;
		bool first_image = true;
		
		FLOATPOINT get_next_pixelshift(cv::Mat1b& speckle_image) override;
};

#endif
