#include <iostream>
#include <string>
#include <map>
#include <filesystem>
#include "algorithm.hpp"
#include "image_tool.hpp"
#include "debugger.hpp"
#include "image_collector.hpp"
#include "config.hpp"

#include <csignal>
#include <cstdlib>
#include <cstdio>

static void sigHandler(int sig) {
	std::cout << "Signal " << sig << " received, exiting" << std::endl;
	std::exit(0);
}

static const char* ver = "0.2.2";

int main(int argc, const char *argv[])
{
	std::cout << "SITOH LSWLI Software ver. " << ver << std::endl << std::endl;
	std::cout << "Starting software." << std::endl;

	signal(SIGINT, sigHandler);

	user_parameter params;
	if (0 > params.read_config(argc, argv))
	{
		std::cout << "Please check command line parameters" << std::endl;
		return -1;
	}

	Image_Source* source = nullptr;
	try
	{
		// create image source
		source = create_ImageSrc(params);
		std::cout << "X = " << params.dim_img_x << " | Y = " << params.dim_img_y << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return -1;
	}
	std::cout << "Creating image source complete" << std::endl;
	ImageTool* image_tool=nullptr;
	try
	{
		if (!params.area_speckle.empty())
			image_tool = new ImageTool_WLI_SPEKL(source, params);
		else
		{
			try
			{
				FLOATPOINT pxstep = std::stod(params.pixel_step);
				image_tool = new ImageTool_constFDPX(source, pxstep, params);
			}
			catch (const std::invalid_argument& e)
			{
				image_tool = new ImageTool_externFDPX(source, params);
			}
		}
	}
	catch (const NoMorePictures& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
		delete source;
		return -3;
	}
	catch(const std::runtime_error& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
		delete source;
		return -4;
	}
	std::cout << "Creating ImageToolBox complete!" << std::endl;

	ImageCollector* image_collector = nullptr;
	try
	{
		image_collector = new ImageCollector(image_tool, params);
	}
	catch (const std::exception& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
		delete image_tool;
		delete source;
		return -1;
	}
	std::cout << "Creating image_collector complete!" << std::endl;

	CoreAlgorithm_Debug* debugger=nullptr;
	if (params.debug_queue)
	{
		debugger = new CoreAlgorithm_Debug(params, image_tool->get_x());
	}

	// which signal to use? (padding)
	Signal_Complex* signal_complex=nullptr;
	switch (params.signal)
	{
		case 1:
			signal_complex = new Signal_const(512);
			break;

		// identical to matlab file (old algorithm)
		case 2:
			signal_complex = new Signal_static_pad(0);
			break;

		// identical to matlab file (a030)
		case 3:
			signal_complex = new Signal_static_pad(64);
			break;

		default:
			signal_complex = new Signal_Complex_auto_pad();
			break;
	}
	std::cout << "Creating signal_complex complete!" << std::endl;

	FreqBandFilter filter(signal_complex, params.radius, debugger);
	std::cout << "Creating filter complete!" << std::endl;

	Angle* angle_obj = nullptr;
	try
	{
		angle_obj = new Angle(params);
	}
	catch(const std::exception& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
		delete image_collector;
		delete signal_complex;
		delete debugger;
		delete image_tool;
		delete source;
		
		return -2;
	}
	std::cout << "Creating angle complete!" << std::endl;

	MoveRMS<std::vector<FLOATPOINT>>* rms_obj = nullptr;
	switch (params.algorithm)
	{
	case 1:
		rms_obj = new MoveRMS_Sliding<std::vector<FLOATPOINT>, sqrtdiv>(params.rfac);
		break;
	case 2:
		rms_obj = new MoveRMS_Sliding<std::vector<FLOATPOINT>, divide>(params.rfac);
		break;
	case 3:
	default:
		rms_obj = new MoveRMS_Sliding<std::vector<FLOATPOINT>, id>(params.rfac);
		break;
	}
	std::cout << "Creating Sliding algorithm complete!" << std::endl;

	CoreAlgorithm* kernel=nullptr;
	try
	{
		if (params.algorithm > 0)
			kernel = new CoreAlgorithm_RMS(params, &filter, image_collector, angle_obj, rms_obj, debugger);
		else
			kernel = new CoreAlgorithm_FFT(params, &filter, image_collector, angle_obj, signal_complex, debugger);
		std::cout << "Creating kernel complete! Will start RUN!" << std::endl;
		kernel->run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
	}

	delete kernel;
	delete signal_complex;
	delete debugger;
	delete rms_obj;
	delete image_tool;
	delete source;

	return 0;
}
