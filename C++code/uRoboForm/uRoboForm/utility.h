#pragma once
#include <iostream>
#include <chrono>
#include <fstream>

class utility
{
public:
	void static display_time(const std::chrono::steady_clock::time_point& t01, const std::chrono::steady_clock::time_point& t02);

};
