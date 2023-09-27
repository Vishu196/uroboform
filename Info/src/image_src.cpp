#include "image_src.hpp"
#include "helper.hpp"
#include <thread>

using namespace std::chrono_literals;

// includes for IOCTL
#ifdef CONFIG_CAMERA_IOCTL
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/v4l2-common.h>
#include <linux/v4l2-controls.h>
#include <linux/videodev2.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <string.h>
#include <fstream>
#include <string>
#endif

/* Only for less copying, if a class method wants to get an image with checks enabled.
 * Keep in mind, that the method get_image()
 * is only for getting an image, without any checks. Even get call could fail => image.empty(). */
cv::Mat1b Image_Source::get_next()
{
	// check if no more images to come, but handle all still buffered ones
	if (!image_buffer.size() && source_is_empty)
	{
		std::cout << std::endl;
		throw(NoMorePictures("Source for pictures is empty!"));
	}

	cv::Mat1b output_image;
	// wait until image delivered from underlying source implementation
	{
		std::unique_lock<std::mutex> guard(buffer_lock);
		cv_read.wait(guard, [&] {return (image_buffer.size() != 0) || (!image_buffer.size() && source_is_empty); });
		// check if no more images to come
		if (!image_buffer.size() && source_is_empty)
		{
			std::cout << std::endl;
			throw(NoMorePictures("Source for pictures is empty!"));
		}

		// get and remove from queue
		output_image = image_buffer.front();
		image_buffer.pop_front();
		// may put a new one in
		cv_write.notify_one();
		// global counter increment
		++pic_counter;
		// true if maximum is set and if enough pictures handled
		if (params.max_num_images && (pic_counter >= params.max_num_images))
			source_is_empty = true;
		// we log here camera images
		if (wli_image_logger)
			wli_image_logger->log(output_image);
		std::cout << "Source buffer " << image_buffer.size() << std::endl;
//		std::cout << " Image# delivered from source" << pic_counter << std::endl;
	}
	
	// if the image is empty, and it should not be empty, as there is no limit set => bad_exception e.g. camera driver error
	if (output_image.empty())
		throw(ImageSourceError("CRITICAL: Could not get new image from source!"));

	if (uint32_t(output_image.cols) != params.dim_img_x || uint32_t(output_image.rows) != params.dim_img_y)
	{
		std::cout << "Expected: " << params.dim_img_x << "x" << params.dim_img_y << " and got " << output_image.cols << "x" << output_image.rows << std::endl;
		throw(std::runtime_error("Wrong dimensions or picture corrupted"));	
	}
	return output_image;
}

Folder_Source::Folder_Source(user_parameter& params, const int32_t coltype)
						: Image_Source(params, coltype)
{
	std::cout << "Reading from " << params.input_folder.string() << std::endl;
	for (auto& p : std::filesystem::directory_iterator(params.input_folder))
	{
		// skip hidden files and special directory entries
		if (p.path().c_str()[0] == '.')
			continue;
		if (p.is_regular_file())
			filenames.push_back(p.path().string());
	}

	// close directory and sort list of filenames by name
	filenames.sort();

	// skip files
	if (params.skip_initial_images > filenames.size())
		filenames.clear();
	else
	{
		auto it = filenames.begin();
		std::advance(it, params.skip_initial_images);
		filenames.erase(filenames.begin(), it);
	}

	/* resize member variable max_number_of_pictures or list of filenames
	 * This might not be necessary, alternatively override is_empty(), check
	 * for filenames.size() in it
	 */
	if (params.max_num_images  > filenames.size() || params.max_num_images == 0)
		params.max_num_images = filenames.size();
	else
		filenames.resize(params.max_num_images);
	
	/* Throw an error, if there are no images in folder.
	 * The error must be thrown, because the member variables are in an
	 * invalid state otherwise. Also the next class, using this as
	 * the image source, does not check for valid dimension => would lead to another
	 * error, without informing that it comes from this class. */
	if (!filenames.size())
	{
		source_is_empty = true;
		throw(std::runtime_error("The folder you have selected is empty or does not contain enough images to start"));
	}

	get_image();
	if (image_buffer.size())
	{
		params.dim_img_x = image_buffer.front().cols;
		params.dim_img_y = image_buffer.front().rows;
	}
	std::thread t1([&] {
		while (!source_is_empty)
		{
			get_image();
		}
	});
	t1.detach();
}

void Folder_Source::get_image()
{
	const std::string front = filenames.front();
	filenames.pop_front();
	//	std::cout << "Next image: " << front << std::endl;
	std::cout << "\rimages left in src: " << std::setw(6) << filenames.size() << std::endl;
	if (filenames.size() == 0)
		source_is_empty = true;

	if (!params.stop_at_stage || (params.stop_at_stage> user_parameter::E_STAGE_SOURCE))
	{
		std::unique_lock<std::mutex> guard(buffer_lock);
		cv_write.wait(guard, [&] {return image_buffer.size() < max_buffered_images; });
		image_buffer.push_back(cv::imread(front, cv::IMREAD_GRAYSCALE));
	}
	cv_read.notify_one();
}

void Image_Source_Cam::init_cam()
{
	if (!open_cam(device_number))
		throw(std::runtime_error("Camera not available, check connection and device number!"));

	// before setting the dimension, try to ask for possible configurations!
	set_dim(params.dim_img_x, params.dim_img_y);

	get_image();
	cv::Mat1b image = image_buffer.front();
	// may inform the user, that the choosen dimensions do not work
	if (params.dim_img_x != static_cast<uint32_t> (image.cols))
	{
		std::cerr << "WARNING: Dimension x = " << params.dim_img_x << " not accepted by camera. Changed x to " << image.cols << std::endl;
		params.dim_img_x = image.cols;
	}
	if (params.dim_img_y != static_cast<uint32_t> (image.rows))
	{
		std::cerr << "WARNING: Dimension y = " << params.dim_img_y << " not accepted by camera. Changed y to " << image.rows << std::endl;
		params.dim_img_y = image.rows;
	}
	std::thread t1([&] {
		while (!source_is_empty)
		{
			const auto start = std::chrono::high_resolution_clock::now();
			try
			{
				get_image();
				cv_read.notify_one();
			}
			catch (const std::bad_alloc& e)
			{
				// if memory is full we simply stop the thread and don't collect images anymore
				std::cerr << e.what() << std::endl;
				std::cerr << "Stopping image aqcuisition due to full memory" << std::endl;
				break;
			}
			const auto end = std::chrono::high_resolution_clock::now();
			const std::chrono::duration<double, std::milli> elapsed = end - start;
			if (elapsed > (std::chrono::duration < double, std::milli>)0)
				std::this_thread::sleep_for(1000ms / fps-elapsed);
		}
	});
	t1.detach();
}

void Image_Source_Cam::get_image()
{
	uint8_t num_of_tries = 0;
	uint8_t cam_open_tries = 0;
	bool camera_working  = false;
	
	do
	{
		cam_open_tries = 0;
		try
		{
			grab_image();
			camera_working = true;
		}
		catch (const ImageSourceError& e)
		{
			close_cam();
			camera_working  = false;
			std::cerr << "ERROR: " << e.what() << ". Try number " << (uint16_t)num_of_tries << std::endl;
			while( !open_cam(device_number) && cam_open_tries++ < 9)
				close_cam();
		}
	}
	while (num_of_tries++ < 9 && !camera_working && cam_open_tries < 9);
	
	if ( !open_cam(device_number) )
		throw(std::runtime_error("Could not re-open camera, please try another camera configuration!"));
	// one last try, but without exception catching
	if (!camera_working)
		grab_image();
}

inline bool Camera_Source_OpenCV::open_cam(const uint32_t device_number)
{
	(void)device_number;

	if (cam.isOpened())
		return true;
	cam.open(params.input_folder);
	if (cam.isOpened())
	{
		cam.set(cv::CAP_PROP_FORMAT, -1);
		cam.set(cv::CAP_PROP_FPS, fps);
		std::cout << cam.isOpened() << ":";
		std::cout << cam.getBackendName();
		std::cout << " FPS: " << cam.get(cv::CAP_PROP_FPS);
		std::cout << std::endl;
	}
	return cam.isOpened();
}

#ifdef CONFIG_CAMERA_IOCTL
/* Source: 	http://jwhsmith.net/2014/12/capturing-a-webcam-stream-using-v4l2/
 * 			https://jayrambhia.com/blog/capture-v4l2	*/
void Camera_Source_IOCTL::check_cap(void)
{
	//struct v4l2_capability cap;
	// get cam capabilities
	if (ioctl(cam, VIDIOC_QUERYCAP, &cap) < 0)
		throw(std::runtime_error("Cam does not answer to capability request!"));
	
	// check if camera can capture a single frame
	if (!(cap.capabilities & cap_type))
		throw(std::runtime_error("Cam does not handle single-planar video capture!"));
	std::cout << "Cam Cap OK" << std::endl;
}

void Camera_Source_IOCTL::init_mmap(void)
{
	// buffer request
	requestBuffer			= {0};
	requestBuffer.count		= 1;
	requestBuffer.type 		= buf_type;
	requestBuffer.memory	= mem_type;
	
	if(ioctl(cam, VIDIOC_REQBUFS, &requestBuffer) < 0)
		throw(std::runtime_error("Buffer request failed!"));
	std::cout << "Init request buffer OK" << std::endl;
	
	memset(&queryBuffer, 0, sizeof(queryBuffer));
	// allocate buffer
	queryBuffer 		= {0};
	queryBuffer.type	= buf_type;
	queryBuffer.memory	= mem_type;
	queryBuffer.index 	= 0;
 
	if(ioctl(cam, VIDIOC_QUERYBUF, &queryBuffer) < 0)
		throw(std::runtime_error("Buffer allocation failed!"));
	std::cout << "Init query buffer OK" << std::endl;	
	buffer = static_cast<uint8_t*>(mmap(NULL, queryBuffer.length, PROT_READ | PROT_WRITE, MAP_SHARED, cam, queryBuffer.m.offset));
	// clear memory, just to be sure
	memset(buffer, 0, queryBuffer.length);
}

void Camera_Source_IOCTL::start_cam(void)
{
	//type = queryBuffer.type;
	// Activate streaming
	//int type = bufferinfo.type;
	if (ioctl(cam, VIDIOC_STREAMON, &buf_type) < 0)
		throw(std::runtime_error("Unable to start stream!"));
	std::cout << "Starting cam OK" << std::endl;
}

bool Camera_Source_IOCTL::open_cam(const uint32_t device_number)
{
	// if open fails or the device is not able to capture frames return false, otherwise return true
	if ( (cam = open((dev_path+std::to_string(device_number)).c_str(), O_RDWR)) < 0 )
		return false;

	std::cout << "Opening cam OK" << std::endl;
	check_cap();
    
	return true;
}

void Camera_Source_IOCTL::close_cam(void)
{
	if(ioctl(cam, VIDIOC_STREAMOFF, &buf_type) < 0)
		throw(std::runtime_error("Could not end streaming, VIDIOC_STREAMOFF"));
	
	close(cam);
}

void Camera_Source_IOCTL::set_dim(const uint32_t x, const uint32_t y)
{
		// cam settings
	// try "$ v4l2-ctl -d /dev/video0 --list-formats-ext"
	imageFormat.type 				= buf_type;
	imageFormat.fmt.pix.pixelformat = pixelformat;
	imageFormat.fmt.pix.width 		= x;
	imageFormat.fmt.pix.height 		= y;
//	imageFormat.fmt.pix.field 		= V4L2_FIELD_NONE;
    
    if (ioctl(cam, VIDIOC_S_FMT, &imageFormat) < 0)
		// throw an error dimension not supported!
		throw(std::runtime_error("Device could not set format, VIDIOC_S_FMT"));
	std::cout << "Set image format OK" << std::endl;
}

void Camera_Source_IOCTL::grab_image()
{
	if (ioctl(cam, VIDIOC_QBUF, &queryBuffer) < 0)
		throw(std::runtime_error("Could not queue buffer, VIDIOC_QBUF"));
    
    // THIS CALL DOES NOT WORK! => endless waiting for an image (never get one)
    if (ioctl(cam, VIDIOC_DQBUF, &queryBuffer) < 0)
		throw(std::runtime_error("Could not dequeue the buffer, VIDIOC_DQBUF"));
    
    std::cout << "Buffer has: " << (double)queryBuffer.bytesused / 1024 << " KBytes of data" << std::endl;
    
	if (!params.stop_at_stage || (params.stop_at_stage > user_parameter::E_STAGE_SOURCE))
		image_buffer.push_back(cv::Mat1b(dim_img_y, dim_img_x, buffer));
}

void Camera_Source_IOCTL::skip_images(const int32_t skip)
{
	// init mmap before you start the skip
	init_mmap();
	// start camera
	start_cam();
	Image_Source_Cam::skip_images(skip);
}
#endif

#ifdef WITH_FLIR

void Camera_Source_Spinnaker::grab_image()
{
	if (!pCam || !pCam->IsInitialized())
		throw std::runtime_error("Camera failed while grabbing image.");
	if (!is_acquiring)
	{
		pCam->BeginAcquisition();
		is_acquiring = true;
	}

	// Retrieve next received image
	//
	// *** NOTES ***
	// Capturing an image houses images on the camera buffer. Trying
	// to capture an image that does not exist will hang the camera.
	//
	// *** LATER ***
	// Once an image from the buffer is saved and/or no longer
	// needed, the image must be released in order to keep the
	// buffer from filling up.
	//
	Spinnaker::ImagePtr pResultImage = pCam->GetNextImage();

	//
	// Ensure image completion
	//
	// *** NOTES ***
	// Images can easily be checked for completion. This should be
	// done whenever a complete image is expected or required.
	// Further, check image status for a little more insight into
	// why an image is incomplete.
	//
	if (pResultImage->IsIncomplete())
	{
		// Retrieve and print the image status description
		std::cout << "Image incomplete: " << Spinnaker::Image::GetImageStatusDescription(pResultImage->GetImageStatus())
			<< "..." << std::endl
			<< std::endl;
	}
	else
	{
		//
		// Print image information; height and width recorded in pixels
		//
		// *** NOTES ***
		// Images have quite a bit of available metadata including
		// things such as CRC, image status, and offset values, to
		// name a few.
		//
		const size_t width = pResultImage->GetWidth();
		const size_t height = pResultImage->GetHeight();

		if (!params.stop_at_stage || (params.stop_at_stage > user_parameter::E_STAGE_SOURCE))
		{
			std::lock_guard<std::mutex> guard(buffer_lock);
			image_buffer.push_back(cv::Mat1b(height,width, (uint8_t*)pResultImage->GetData()).clone());
		}
		// Release image
		//
		// *** NOTES ***
		// Images retrieved directly from the camera (i.e. non-converted
		// images) need to be released in order to keep from filling the
		// buffer.
		//
		pResultImage->Release();
	}
}

bool Camera_Source_Spinnaker::open_cam(const uint32_t device_number)
{
	if (pCam && pCam->IsInitialized())
		return true;
	system = Spinnaker::System::GetInstance();
	// Print out current library version
	const Spinnaker::LibraryVersion spinnakerLibraryVersion = system->GetLibraryVersion();
	std::cout << "Spinnaker library version: " << spinnakerLibraryVersion.major << "." << spinnakerLibraryVersion.minor
		<< "." << spinnakerLibraryVersion.type << "." << spinnakerLibraryVersion.build << std::endl
		<< std::endl;

	Spinnaker::CameraList camList = system->GetCameras();
	const unsigned int numCameras = camList.GetSize();
	std::cout << "Number of cameras detected: " << numCameras << std::endl << std::endl;
	if (numCameras <= device_number)
		return false;
	pCam = camList.GetByIndex(device_number);
	camList.Clear();
	Spinnaker::GenApi::INodeMap& nodeMapTLDevice = pCam->GetTLDeviceNodeMap();
	print_device_info(nodeMapTLDevice);

	pCam->Init();
	Spinnaker::GenApi::INodeMap& nodeMap = pCam->GetNodeMap();

	printDeviceInfo(nodeMap);

	// Set acquisition mode to continuous
	//
	// *** NOTES ***
	// Because the example acquires and saves 10 images, setting acquisition
	// mode to continuous lets the example finish. If set to single frame
	// or multiframe (at a lower number of images), the example would just
	// hang. This would happen because the example has been written to
	// acquire 10 images while the camera would have been programmed to
	// retrieve less than that.
	//
	// Setting the value of an enumeration node is slightly more complicated
	// than other node types. Two nodes must be retrieved: first, the
	// enumeration node is retrieved from the nodemap; and second, the entry
	// node is retrieved from the enumeration node. The integer value of the
	// entry node is then set as the new value of the enumeration node.
	//
	// Notice that both the enumeration and the entry nodes are checked for
	// availability and readability/writability. Enumeration nodes are
	// generally readable and writable whereas their entry nodes are only
	// ever readable.
	//
	// Retrieve enumeration node from nodemap
	Spinnaker::GenApi::CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
	if (!IsAvailable(ptrAcquisitionMode) || !IsWritable(ptrAcquisitionMode))
	{
		std::cout << "Unable to set acquisition mode to continuous (enum retrieval). Aborting..." << std::endl << std::endl;
		return false;
	}

	// Retrieve entry node from enumeration node
	Spinnaker::GenApi::CEnumEntryPtr ptrAcquisitionModeContinuous = ptrAcquisitionMode->GetEntryByName("Continuous");
	if (!IsAvailable(ptrAcquisitionModeContinuous) || !IsReadable(ptrAcquisitionModeContinuous))
	{
		std::cout << "Unable to set acquisition mode to continuous (entry retrieval). Aborting..." << std::endl << std::endl;
		return false;
	}

	// Retrieve integer value from entry node
	const int64_t acquisitionModeContinuous = ptrAcquisitionModeContinuous->GetValue();

	// Set integer value from entry node as new value of enumeration node
	ptrAcquisitionMode->SetIntValue(acquisitionModeContinuous);

	std::cout << "Acquisition mode set to continuous..." << std::endl;

	Spinnaker::GenApi::CFloatPtr framerate = nodeMap.GetNode("AcquisitionFrameRate");
	if (!IsAvailable(framerate) || !IsWritable(framerate))
	{
		std::cout << "Unable to set frame rate. Ignoring..." << std::endl << std::endl;
	}
	else
	{
		framerate->SetValue(fps);
		std::cout << "Framerate set to " << framerate->GetValue() << std::endl;
	}
	configure_exposure(nodeMap, 19);
	configureTrigger(nodeMap, true);

	return true;
}

// This function prints the device information of the camera from the transport
// layer; please see NodeMapInfo example for more in-depth comments on printing
// device information from the nodemap.
int Camera_Source_Spinnaker::printDeviceInfo(Spinnaker::GenApi::INodeMap& nodeMap)
{
	int result = 0;
	std::cout << std::endl << "*** DEVICE INFORMATION ***" << std::endl << std::endl;
	try
	{
		Spinnaker::GenApi::FeatureList_t features;
		const Spinnaker::GenApi::CCategoryPtr category = nodeMap.GetNode("DeviceInformation");
		if (IsAvailable(category) && IsReadable(category))
		{
			category->GetFeatures(features);
			for (auto it = features.begin(); it != features.end(); ++it)
			{
				const Spinnaker::GenApi::CNodePtr pfeatureNode = *it;
				std::cout << pfeatureNode->GetName() << " : ";
				Spinnaker::GenApi::CValuePtr pValue = static_cast<Spinnaker::GenApi::CValuePtr>(pfeatureNode);
				std::cout << (IsReadable(pValue) ? pValue->ToString() : "Node not readable");
				std::cout << std::endl;
			}
		}
		else
		{
			std::cout << "Device control information not available." << std::endl;
		}
	}
	catch (Spinnaker::Exception& e)
	{
		std::cout << "Error: " << e.what() << std::endl;
		result = -1;
	}
	return result;
}

// This function configures the camera to use a trigger. First, trigger mode is
// set to off in order to select the trigger source. Once the trigger source
// has been selected, trigger mode is then enabled, which has the camera
// capture only a single image upon the execution of the chosen trigger.
int Camera_Source_Spinnaker::configureTrigger(Spinnaker::GenApi::INodeMap& nodeMap, bool hardwareTrigger)
{
	int result = 0;
	std::cout << std::endl << "*** CONFIGURING TRIGGER ***" << std::endl;
	std::cout << "Note that if the application / user software triggers faster than frame time, the trigger may be dropped "
		"/ skipped by the camera."
		<< std::endl
		<< "If several frames are needed per trigger, a more reliable alternative for such case, is to use the "
		"multi-frame mode."
		<< std::endl;
	if (!hardwareTrigger)
	{
		std::cout << "Software trigger chosen..." << std::endl;
	}
	else
	{
		std::cout << "Hardware trigger chosen..." << std::endl;
	}
	try
	{
		//
		// Ensure trigger mode off
		//
		// *** NOTES ***
		// The trigger must be disabled in order to configure whether the source
		// is software or hardware.
		//
		Spinnaker::GenApi::CEnumerationPtr ptrTriggerMode = nodeMap.GetNode("TriggerMode");
		if (!IsAvailable(ptrTriggerMode) || !IsReadable(ptrTriggerMode))
		{
			std::cout << "Unable to disable trigger mode (node retrieval). Aborting..." << std::endl;
			return -1;
		}
		Spinnaker::GenApi::CEnumEntryPtr ptrTriggerModeOff = ptrTriggerMode->GetEntryByName("Off");
		if (!IsAvailable(ptrTriggerModeOff) || !IsReadable(ptrTriggerModeOff))
		{
			std::cout << "Unable to disable trigger mode (enum entry retrieval). Aborting..." << std::endl;
			return -1;
		}
		ptrTriggerMode->SetIntValue(ptrTriggerModeOff->GetValue());
		std::cout << "Trigger mode disabled..." << std::endl;
		//
		// Set TriggerSelector to FrameStart
		//
		// *** NOTES ***
		// For this example, the trigger selector should be set to frame start.
		// This is the default for most cameras.
		//
		Spinnaker::GenApi::CEnumerationPtr ptrTriggerSelector = nodeMap.GetNode("TriggerSelector");
		if (!IsAvailable(ptrTriggerSelector) || !IsWritable(ptrTriggerSelector))
		{
			std::cout << "Unable to set trigger selector (node retrieval). Aborting..." << std::endl;
			return -1;
		}
		Spinnaker::GenApi::CEnumEntryPtr ptrTriggerSelectorFrameStart = ptrTriggerSelector->GetEntryByName("FrameStart");
		if (!IsAvailable(ptrTriggerSelectorFrameStart) || !IsReadable(ptrTriggerSelectorFrameStart))
		{
			std::cout << "Unable to set trigger selector (enum entry retrieval). Aborting..." << std::endl;
			return -1;
		}
		ptrTriggerSelector->SetIntValue(ptrTriggerSelectorFrameStart->GetValue());
		std::cout << "Trigger selector set to frame start..." << std::endl;
		//
		// Select trigger source
		//
		// *** NOTES ***
		// The trigger source must be set to hardware or software while trigger
		// mode is off.
		//
		Spinnaker::GenApi::CEnumerationPtr ptrTriggerSource = nodeMap.GetNode("TriggerSource");
		if (!IsAvailable(ptrTriggerSource) || !IsWritable(ptrTriggerSource))
		{
			std::cout << "Unable to set trigger mode (node retrieval). Aborting..." << std::endl;
			return -1;
		}
		if (!hardwareTrigger)
		{
			// Set trigger mode to software
			Spinnaker::GenApi::CEnumEntryPtr ptrTriggerSourceSoftware = ptrTriggerSource->GetEntryByName("Software");
			if (!IsAvailable(ptrTriggerSourceSoftware) || !IsReadable(ptrTriggerSourceSoftware))
			{
				std::cout << "Unable to set trigger mode (enum entry retrieval). Aborting..." << std::endl;
				return -1;
			}
			ptrTriggerSource->SetIntValue(ptrTriggerSourceSoftware->GetValue());
			std::cout << "Trigger source set to software..." << std::endl;
		}
		else
		{
			// Set trigger mode to hardware ('Line0')
			Spinnaker::GenApi::CEnumEntryPtr ptrTriggerSourceHardware = ptrTriggerSource->GetEntryByName("Line0");
			if (!IsAvailable(ptrTriggerSourceHardware) || !IsReadable(ptrTriggerSourceHardware))
			{
				std::cout << "Unable to set trigger mode (enum entry retrieval). Aborting..." << std::endl;
				return -1;
			}
			ptrTriggerSource->SetIntValue(ptrTriggerSourceHardware->GetValue());
			std::cout << "Trigger source set to hardware..." << std::endl;
		}
		isHardwareTrigger = hardwareTrigger;
		//
		// Turn trigger mode on
		//
		// *** LATER ***
		// Once the appropriate trigger source has been set, turn trigger mode
		// on in order to retrieve images using the trigger.
		//
		Spinnaker::GenApi::CEnumEntryPtr ptrTriggerModeOn = ptrTriggerMode->GetEntryByName("On");
		if (!IsAvailable(ptrTriggerModeOn) || !IsReadable(ptrTriggerModeOn))
		{
			std::cout << "Unable to enable trigger mode (enum entry retrieval). Aborting..." << std::endl;
			return -1;
		}
		ptrTriggerMode->SetIntValue(ptrTriggerModeOn->GetValue());
		// NOTE: Blackfly and Flea3 GEV cameras need 1 second delay after trigger mode is turned on
		std::cout << "Trigger mode turned back on..." << std::endl;
	}
	catch (Spinnaker::Exception& e)
	{
		std::cout << "Error: " << e.what() << std::endl;
		result = -1;
	}
	return result;
}

// This function retrieves a single image using the trigger. In this example,
// only a single image is captured and made available for acquisition - as such,
// attempting to acquire two images for a single trigger execution would cause
// the example to hang. This is different from other examples, whereby a
// constant stream of images are being captured and made available for image
// acquisition.
int Camera_Source_Spinnaker::issueTrigger(Spinnaker::GenApi::INodeMap& nodeMap)
{
	int result = 0;
	try
	{
		//
		// Use trigger to capture image
		//
		// *** NOTES ***
		// The software trigger only feigns being executed by the Enter key;
		// what might not be immediately apparent is that there is not a
		// continuous stream of images being captured; in other examples that
		// acquire images, the camera captures a continuous stream of images.
		// When an image is retrieved, it is plucked from the stream.
		//
		if (!isHardwareTrigger)
		{
			// Execute software trigger
			Spinnaker::GenApi::CCommandPtr ptrSoftwareTriggerCommand = nodeMap.GetNode("TriggerSoftware");
			if (!IsAvailable(ptrSoftwareTriggerCommand) || !IsWritable(ptrSoftwareTriggerCommand))
			{
				std::cout << "Unable to execute trigger. Aborting..." << std::endl;
				return -1;
			}
			ptrSoftwareTriggerCommand->Execute();
			// NOTE: Blackfly and Flea3 GEV cameras need 2 second delay after software trigger
		}
		else
		{
			// Execute hardware trigger
			std::cout << "Use the hardware to trigger image acquisition." << std::endl;
		}
	}
	catch (Spinnaker::Exception& e)
	{
		std::cout << "Error: " << e.what() << std::endl;
		result = -1;
	}
	return result;
}

// This function returns the camera to a normal state by turning off trigger
// mode.
int Camera_Source_Spinnaker::resetTrigger(Spinnaker::GenApi::INodeMap& nodeMap)
{
	int result = 0;
	try
	{
		//
		// Turn trigger mode back off
		//
		// *** NOTES ***
		// Once all images have been captured, turn trigger mode back off to
		// restore the camera to a clean state.
		//
		Spinnaker::GenApi::CEnumerationPtr ptrTriggerMode = nodeMap.GetNode("TriggerMode");
		if (!IsAvailable(ptrTriggerMode) || !IsReadable(ptrTriggerMode))
		{
			std::cout << "Unable to disable trigger mode (node retrieval). Non-fatal error..." << std::endl;
			return -1;
		}
		Spinnaker::GenApi::CEnumEntryPtr ptrTriggerModeOff = ptrTriggerMode->GetEntryByName("Off");
		if (!IsAvailable(ptrTriggerModeOff) || !IsReadable(ptrTriggerModeOff))
		{
			std::cout << "Unable to disable trigger mode (enum entry retrieval). Non-fatal error..." << std::endl;
			return -1;
		}
		ptrTriggerMode->SetIntValue(ptrTriggerModeOff->GetValue());
		std::cout << "Trigger mode disabled..." << std::endl;
	}
	catch (Spinnaker::Exception& e)
	{
		std::cout << "Error: " << e.what() << std::endl;
		result = -1;
	}
	return result;
}

// This function configures a custom exposure time. Automatic exposure is turned
// off in order to allow for the customization, and then the custom setting is
// applied.
int Camera_Source_Spinnaker::configure_exposure(Spinnaker::GenApi::INodeMap& nodeMap, double exposureTimeToSet)
{
	int result = 0;

	std::cout << std::endl << std::endl << "*** CONFIGURING EXPOSURE ***" << std::endl << std::endl;

	try
	{
		//
		// Turn off automatic exposure mode
		//
		// *** NOTES ***
		// Automatic exposure prevents the manual configuration of exposure
		// time and needs to be turned off.
		//
		// *** LATER ***
		// Exposure time can be set automatically or manually as needed. This
		// example turns automatic exposure off to set it manually and back
		// on in order to return the camera to its default state.
		//
		Spinnaker::GenApi::CEnumerationPtr ptrExposureAuto = nodeMap.GetNode("ExposureAuto");
		if (!IsAvailable(ptrExposureAuto) || !IsWritable(ptrExposureAuto))
		{
			std::cout << "Unable to disable automatic exposure (node retrieval). Aborting..." << std::endl << std::endl;
			return -1;
		}

		Spinnaker::GenApi::CEnumEntryPtr ptrExposureAutoOff = ptrExposureAuto->GetEntryByName("Off");
		if (!IsAvailable(ptrExposureAutoOff) || !IsReadable(ptrExposureAutoOff))
		{
			std::cout << "Unable to disable automatic exposure (enum entry retrieval). Aborting..." << std::endl << std::endl;
			return -1;
		}

		ptrExposureAuto->SetIntValue(ptrExposureAutoOff->GetValue());

		std::cout << "Automatic exposure disabled..." << std::endl;

		//
		// Set exposure time manually; exposure time recorded in microseconds
		//
		// *** NOTES ***
		// The node is checked for availability and writability prior to the
		// setting of the node. Further, it is ensured that the desired exposure
		// time does not exceed the maximum. Exposure time is counted in
		// microseconds. This information can be found out either by
		// retrieving the unit with the GetUnit() method or by checking SpinView.
		//
		Spinnaker::GenApi::CFloatPtr ptrExposureTime = nodeMap.GetNode("ExposureTime");
		if (!IsAvailable(ptrExposureTime) || !IsWritable(ptrExposureTime))
		{
			std::cout << "Unable to set exposure time. Aborting..." << std::endl << std::endl;
			return -1;
		}

		// Ensure desired exposure time does not exceed the maximum
		const double exposureTimeMax = ptrExposureTime->GetMax();

		if (exposureTimeToSet > exposureTimeMax)
		{
			exposureTimeToSet = exposureTimeMax;
		}
		ptrExposureTime->SetValue(exposureTimeToSet);

		std::cout << std::fixed << "Exposure time set to " << exposureTimeToSet << " us..." << std::endl << std::endl;
	}
	catch (Spinnaker::Exception& e)
	{
		std::cout << "Error: " << e.what() << std::endl;
		result = -1;
	}

	return result;
}

// This function returns the camera to its default state by re-enabling automatic
// exposure.

int Camera_Source_Spinnaker::reset_exposure(Spinnaker::GenApi::INodeMap& nodeMap)
{
	int result = 0;

	try
	{
		//
		// Turn automatic exposure back on
		//
		// *** NOTES ***
		// Automatic exposure is turned on in order to return the camera to its
		// default state.
		//
		Spinnaker::GenApi::CEnumerationPtr ptrExposureAuto = nodeMap.GetNode("ExposureAuto");
		if (!IsAvailable(ptrExposureAuto) || !IsWritable(ptrExposureAuto))
		{
			std::cout << "Unable to enable automatic exposure (node retrieval). Non-fatal error..." << std::endl << std::endl;
			return -1;
		}

		Spinnaker::GenApi::CEnumEntryPtr ptrExposureAutoContinuous = ptrExposureAuto->GetEntryByName("Continuous");
		if (!IsAvailable(ptrExposureAutoContinuous) || !IsReadable(ptrExposureAutoContinuous))
		{
			std::cout << "Unable to enable automatic exposure (enum entry retrieval). Non-fatal error..." << std::endl << std::endl;
			return -1;
		}

		ptrExposureAuto->SetIntValue(ptrExposureAutoContinuous->GetValue());

		std::cout << "Automatic exposure enabled..." << std::endl << std::endl;
	}
	catch (Spinnaker::Exception& e)
	{
		std::cout << "Error: " << e.what() << std::endl;
		result = -1;
	}

	return result;
}

// This function prints the device information of the camera from the transport
// layer; please see NodeMapInfo example for more in-depth comments on printing
// device information from the nodemap.

int Camera_Source_Spinnaker::print_device_info(Spinnaker::GenApi::INodeMap& nodeMap)
{
	int result = 0;

	std::cout << std::endl << "*** DEVICE INFORMATION ***" << std::endl << std::endl;

	try
	{
		Spinnaker::GenApi::FeatureList_t features;
		Spinnaker::GenApi::CCategoryPtr category = nodeMap.GetNode("DeviceInformation");
		if (IsAvailable(category) && IsReadable(category))
		{
			category->GetFeatures(features);

			Spinnaker::GenApi::FeatureList_t::const_iterator it;
			for (it = features.begin(); it != features.end(); ++it)
			{
				Spinnaker::GenApi::CNodePtr pfeatureNode = *it;
				std::cout << pfeatureNode->GetName() << " : ";
				Spinnaker::GenApi::CValuePtr pValue = (Spinnaker::GenApi::CValuePtr)pfeatureNode;
				std::cout << (IsReadable(pValue) ? pValue->ToString() : "Node not readable");
				std::cout << std::endl;
			}
		}
		else
		{
			std::cout << "Device control information not available." << std::endl;
		}
	}
	catch (Spinnaker::Exception& e)
	{
		std::cout << "Error: " << e.what() << std::endl;
		result = -1;
	}

	return result;
}
void Camera_Source_Spinnaker::set_x(const uint32_t x)
{
	// Set maximum width
	//
	// *** NOTES ***
	// Other nodes, such as those corresponding to image width and height,
	// might have an increment other than 1. In these cases, it can be
	// important to check that the desired value is a multiple of the
	// increment. However, as these values are being set to the maximum,
	// there is no reason to check against the increment.
	//
	Spinnaker::GenApi::INodeMap& nodeMap = pCam->GetNodeMap();
	Spinnaker::GenApi::CIntegerPtr ptrWidth = nodeMap.GetNode("Width");
	if (IsAvailable(ptrWidth) && IsWritable(ptrWidth))
	{
		int64_t widthToSet = x;// ptrWidth->GetMax();
		ptrWidth->SetValue(widthToSet);
		std::cout << "Width set to " << ptrWidth->GetValue() << "..." << std::endl;
	}
	else
	{
		std::cout << "Width not available..." << std::endl;
	}
}
void Camera_Source_Spinnaker::set_y(const uint32_t y)
{
	// Set maximum height
	//
	// *** NOTES ***
	// Other nodes, such as those corresponding to image width and height,
	// might have an increment other than 1. In these cases, it can be
	// important to check that the desired value is a multiple of the
	// increment. However, as these values are being set to the maximum,
	// there is no reason to check against the increment.
	//
	Spinnaker::GenApi::INodeMap& nodeMap = pCam->GetNodeMap();
	Spinnaker::GenApi::CIntegerPtr ptrHeight = nodeMap.GetNode("Height");
	if (IsAvailable(ptrHeight) && IsWritable(ptrHeight))
	{
		int64_t heightToSet = y;// ptrHeight->GetMax();
		ptrHeight->SetValue(heightToSet);
		std::cout << "Height set to " << ptrHeight->GetValue() << "..." << std::endl;
	}
	else
	{
		std::cout << "Height not available..." << std::endl;
	}
}
#endif

/* Function to create the right source out of an input path, /dev/videoX or folder with pictures.
 * Can only handle /dev/videoX at the moment, please do not use links, e.g. /dev/front_cam */
 //params.input_folder, & params.x, & params.y, params.num_of_pic, params.skip
Image_Source* create_ImageSrc(user_parameter& params, const int32_t coltype)
{
	const std::vector<std::string> dev_path{ "/dev/video","/dev/flir" };

	for (std::size_t i = 0; i < dev_path.size(); ++i)
	{
		if (params.input_folder.string().compare(0, dev_path[i].size(), dev_path[i]) == 0)
		{
			int32_t dev_number = -1;
			try
			{
				dev_number = std::stoi(params.input_folder.string().substr(dev_path[i].size()));
			}
			catch (const std::exception&)
			{
				// could not find number
				throw(std::runtime_error("You selected a camera device without a device number. Notice that it is not possible to use links, always use the full path, e.g. /dev/video0"));
			}
			switch (i)
			{
#ifdef WITH_FLIR
			case 1:
				return new Camera_Source_Spinnaker(params, dev_number, coltype);
#endif
			default:
				return new Camera_Source_OpenCV(params, dev_number, coltype);
			}
		}
	}
	// source seems to be a folder containing images
	return new Folder_Source(params, coltype);
}
