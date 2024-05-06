#pragma once
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <string>
#include "cqueue.h"

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
			std::cout << entry << std::endl;
			if (entry.is_regular_file())
			{
				cv::Mat image = cv::imread(entry.path().string(), cv::IMREAD_GRAYSCALE);
				if (image.data != NULL)
				{
					Fifo.push(image);
				}
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
	Source(const std::string& path, int size = 20) : Path(path), Fifo(size)
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
