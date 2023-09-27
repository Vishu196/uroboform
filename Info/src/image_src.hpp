#ifndef IMAGE_SRC_HPP
#define IMAGE_SRC_HPP

#include <stdexcept>
#include "config.hpp"
#include <mutex>
#include <condition_variable>
#include "write_file.hpp"
// for IOCTL
#ifdef CONFIG_CAMERA_IOCTL
#include <linux/videodev2.h>
#endif
//#define WITH_FLIR
#ifdef WITH_FLIR
#define NOMINMAX
#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"
#endif

class NoMorePictures : public std::runtime_error
{
	public:
		NoMorePictures(const char* err): runtime_error(err){}
		virtual ~NoMorePictures(void){}
};

class ImageSourceError : public std::exception
{
	public:
		/** Constructor (C strings).
		*  @param message C-style string error message.
		*                 The string contents are copied upon construction.
		*                 Hence, responsibility for deleting the char* lies
		*                 with the caller. 
		*/
		explicit ImageSourceError(const char* message)
			: msg_(message) {}

		/** Constructor (C++ STL strings).
		*  @param message The error message.
		*/
		explicit ImageSourceError(const std::string& message)
			: msg_(message) {}

		/** Destructor.
		* Virtual to allow for subclassing.
		*/
		virtual ~ImageSourceError() noexcept {}

		/** Returns a pointer to the (constant) error description.
		*  @return A pointer to a const char*. The underlying memory
		*          is in posession of the Exception object. Callers must
		*          not attempt to free the memory.
		*/
		virtual const char* what() const noexcept {
			return msg_.c_str();
			}
	protected:
		/** Error message.
		*/
		std::string msg_;
};

/* Class Image_Source delivers an image, without modifying or checking it.
 * Dimension given as initial parameter is just a suggestion, the actual size can differ.
 *  User can define a maximal number of pictures, default is 0 => unlimited. */
class Image_Source
{
	public:
		uint32_t get_x(void) const {return params.dim_img_x;}
		uint32_t get_y(void) const {return params.dim_img_y;}
		cv::Mat1b get_next();
		virtual ~Image_Source(void) {}

	protected:
		Image_Source(user_parameter& p, const int32_t coltype=cv::IMREAD_GRAYSCALE)
				 : params(p), colortype(coltype) {}
		user_parameter& params;
		std::deque<cv::Mat1b> image_buffer;
		std::mutex buffer_lock;
		std::condition_variable cv_read, cv_write;
		const int32_t colortype;
		ImageLogger* wli_image_logger=nullptr;
		uint32_t pic_counter=0;
		virtual void get_image()=0;
		bool source_is_empty = false;
};

/* Class Folder_Source has a folder with pictures, class "ReadFolder" in file "read_folder.hpp".
 * It gets a filename of the image from a list "ReadFolder::filenames" and uses OpenCV to load
 * the picture into a object "cv::Mat1b" (grayscale). It also deletes already loaded filenames
 * from "ReadFolder::filenames". */
class Folder_Source : public Image_Source
{
	public:
		Folder_Source(user_parameter& params, const int32_t coltype=cv::IMREAD_GRAYSCALE);
		void get_image();
		virtual ~Folder_Source(void){}
		
	private:
		// usually hdd is fast enough, so we do not need to buffer too many images ahead. This is the limit
		// at which the thread would sleep for some time before reading more.
		const uint32_t max_buffered_images = 100;
		std::list<std::string> filenames;
};

// abstract class for all camera modules
class Image_Source_Cam : public Image_Source
{
	public:
		virtual ~Image_Source_Cam(void) {}
		
	protected:
	// x and y inverted by most webcams!
		Image_Source_Cam(user_parameter& params, const uint32_t device_number=0, const int32_t coltype=cv::IMREAD_GRAYSCALE)
					 : Image_Source(params, coltype), device_number(device_number), fps(params.fps)
		{
			wli_image_logger = new ImageLogger(params.parent_folder/"Camera_Images", "cam");
		}
		
		uint16_t device_number;
		uint32_t fps;
		//void image_empty_handler(cv::Mat1b& output_image) override;
		virtual void close_cam(void) = 0;
		virtual bool open_cam(const uint32_t device_number) = 0;
		// set_cam methods should return the dimension the camera module was able to set
		// do not return the wanted value here, otherwise the wrong value is set as a member => unwanted behaviour or error
		virtual void set_dim(const uint32_t x, const uint32_t y) = 0;
		// init_cam initializes the camera, by calling the above pure virtual methods
		void init_cam();
		void get_image();
		virtual void grab_image()=0;
};

//#ifdef CONFIG_ENABLE_CAMERA
/* See camera manual for settings in constructors:
 * https://www.theimagingsource.com/products/industrial-cameras/usb-2.0-monochrome/dmk22buc03/ 
 * Source: https://answers.opencv.org/question/1/how-can-i-get-frames-from-my-webcam/
 * 			https://stackoverflow.com/questions/11420748/setting-camera-parameters-in-opencv-python	*/
class Camera_Source_OpenCV : public Image_Source_Cam
{
	public:
		// first constructor automatically changes dimension x and y for caller, e.g. main function
		Camera_Source_OpenCV(user_parameter& params, const uint32_t device_number=0, const int32_t coltype=cv::IMREAD_GRAYSCALE)
						 : Image_Source_Cam(params,device_number,coltype), capture_api(params.capture_api)
						 {init_cam();}
		virtual ~Camera_Source_OpenCV(void){close_cam();}
	
	private:
		cv::VideoCaptureAPIs capture_api;
		cv::VideoCapture cam;
		
		void grab_image()
		{
			static int i = 0;
			cv::Mat1b img;
			cam >> img;
			if (!params.stop_at_stage || (params.stop_at_stage > user_parameter::E_STAGE_SOURCE))
			{
				std::lock_guard<std::mutex> guard(buffer_lock);
				image_buffer.push_back(img);
			}
			std::cout << "img#" << i++ << std::endl;
		}
		void close_cam(void) override {cam.release();}
		bool open_cam(const uint32_t device_number) override;
		void set_x(const uint32_t x) {cam.set(cv::CAP_PROP_FRAME_WIDTH, x);}
		void set_y(const uint32_t y) {cam.set(cv::CAP_PROP_FRAME_HEIGHT,y);}
		void set_dim(const uint32_t x, const uint32_t y) override { set_x(x); set_y(y); }
};

#ifdef CONFIG_CAMERA_IOCTL
class Camera_Source_IOCTL : public Image_Source_Cam
{
	public:
		// first constructor automatically changes dimension x and y for caller, e.g. main function
		Camera_Source_IOCTL(user_parameter& params, const uint32_t device_number=0,
						const int32_t coltype=cv::IMREAD_GRAYSCALE, const std::string dev_path="/dev/video")
						 : Image_Source_Cam(params, device_number,coltype), dev_path(dev_path)
						 {init_cam(); *dim_img_x=this->dim_img_x; *dim_img_y=this->dim_img_y; /*init_mmap();*/}
		virtual ~Camera_Source_IOCTL(void){close_cam();}
		
		private:
			std::string dev_path;
			int cam;
			// not sure if following members are needed as a permanent object, but function ioctl() uses a pointer as third paramter
			v4l2_capability 	cap;
			v4l2_format 		imageFormat;
			v4l2_requestbuffers requestBuffer;
			v4l2_buffer 		queryBuffer;
			uint8_t* 			buffer;
			//int 				type;
			
			// just for testing, use set_format() for all settings in future
			const uint32_t cap_type		= V4L2_CAP_VIDEO_CAPTURE;
			const uint32_t buf_type		= V4L2_BUF_TYPE_VIDEO_CAPTURE;
			const uint32_t pixelformat	= V4L2_PIX_FMT_MJPEG;
			const uint32_t mem_type 	= V4L2_MEMORY_MMAP;
			
			//cv::Mat1b image;
			void close_cam(void) override;
			bool open_cam(const uint32_t device_number) override;
			void set_dim(const uint32_t dim_img_x, const uint32_t dim_img_y) override;
			void init_buffers(void);
			void get_image();
			void check_cap(void);
			void init_mmap(void);
			void start_cam(void);
};
#endif

#ifdef WITH_FLIR
class Camera_Source_Spinnaker : public Image_Source_Cam
{
public:
	// first constructor automatically changes dimension x and y for caller, e.g. main function
	Camera_Source_Spinnaker(user_parameter& params, const uint32_t device_number = 0, const int32_t coltype = cv::IMREAD_GRAYSCALE)
		: Image_Source_Cam(params, device_number, coltype), is_acquiring(false)
	{
		init_cam();
	}
	virtual ~Camera_Source_Spinnaker(void) { close_cam(); }

private:
	Spinnaker::SystemPtr system=nullptr;
	Spinnaker::CameraPtr pCam=nullptr;
	bool is_acquiring;
	bool isHardwareTrigger = false;

	void grab_image() override;

	void close_cam(void) override
	{
		if (pCam)
		{
			pCam->EndAcquisition();
			pCam->DeInit();
		}
		pCam = nullptr;
		system->ReleaseInstance();
	}
	bool open_cam(const uint32_t device_number) override;
	int configureTrigger(Spinnaker::GenApi::INodeMap& nodeMap, bool hardwareTrigger=false);
	int issueTrigger(Spinnaker::GenApi::INodeMap& nodeMap);
	int resetTrigger(Spinnaker::GenApi::INodeMap& nodeMap);
	int printDeviceInfo(Spinnaker::GenApi::INodeMap& nodeMap);

	// This function configures a custom exposure time. Automatic exposure is turned
	// off in order to allow for the customization, and then the custom setting is
	// applied.
	int configure_exposure(Spinnaker::GenApi::INodeMap& nodeMap, double exposureTimeToSet);

	// This function returns the camera to its default state by re-enabling automatic
	// exposure.
	int reset_exposure(Spinnaker::GenApi::INodeMap& nodeMap);

	// This function prints the device information of the camera from the transport
	// layer; please see NodeMapInfo example for more in-depth comments on printing
	// device information from the nodemap.
	int print_device_info(Spinnaker::GenApi::INodeMap& nodeMap);
	void set_x(const uint32_t x);
	void set_y(const uint32_t y);
	void set_dim(const uint32_t x, const uint32_t y) override { set_x(x); set_y(y); }
};

#endif

/* Function to create the right source out of an input path, /dev/videoX or folder with pictures. */
Image_Source* create_ImageSrc(user_parameter& params, const int32_t coltype=cv::IMREAD_GRAYSCALE);

#endif
