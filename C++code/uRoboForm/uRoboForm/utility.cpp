#include"utility.h"

using namespace std;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

void utility::display_time(const chrono::high_resolution_clock::time_point& t01, const chrono::high_resolution_clock::time_point& t02)
{
	duration<double, milli> ms_double = t02 - t01;
	cout << "Time: " << ms_double.count() << "ms" << endl;
	cout << endl;
}

double utility::get_time(const chrono::high_resolution_clock::time_point& t01, const chrono::high_resolution_clock::time_point& t02)
{
	duration<double, milli> ms_double = t02 - t01;
	double duration = chrono::duration<double>(ms_double).count();
	
	return duration*1000;
}
