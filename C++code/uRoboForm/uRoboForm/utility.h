#pragma once
#include <iostream>
#include <chrono>
#include <fstream>

class utility
{
public:
	void static display_time(const std::chrono::high_resolution_clock::time_point& t01, const std::chrono::high_resolution_clock::time_point& t02);
	double static get_time(const std::chrono::high_resolution_clock::time_point& t01, const std::chrono::high_resolution_clock::time_point& t02);

};
