#pragma once
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <string>
#include "cqueue.h"
#include "utility.h"

#define WITH_THREADING
//#define WITH_DEBUGGING


class Source
{
	const std::string Path;
	cqueue<cv::Mat> Fifo;
	std::queue<std::string> Names;
	std::thread Task;
public:
	void fill()
	{
		for (const auto& entry : std::filesystem::directory_iterator(Path))
		{
#ifdef WITH_DEBUGGING
			std::cout << entry << std::endl;
#endif	
			if (entry.is_regular_file())
			{
				auto t00 = std::chrono::high_resolution_clock::now();
				cv::Mat image = cv::imread(entry.path().string(), cv::IMREAD_GRAYSCALE);
				if (image.data != NULL)
				{
					Fifo.push(image);
					std::cout << "Image name is: " << entry;
				}
#ifdef WITH_DEBUGGING
				utility::display_time(t00, std::chrono::high_resolution_clock::now());
#endif	
			}
		}
		Fifo.push(cv::Mat());

	}
	cv::Mat getNext()
	{
		cv::Mat img;
		Fifo.pop(img);
		return img;
	}
	Source(const std::string& path, int size = 5) : Path(path), Fifo(size)
	{
#ifdef WITH_THREADING
		Task = std::thread([&] {
#endif
			fill();
#ifdef WITH_THREADING
			});
		Task.detach();
#endif
	}
};
