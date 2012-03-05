#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include "lib/CImg-1.4.9/CImg.h"

using namespace std;
using namespace cimg_library;

class RGB
{
	public:
	int r;
	int g;
	int b;

	const unsigned int value() const
	{
		return (r << 16) + (g << 8) + b;
	}

	const bool operator<(const RGB b) const
	{
		return this->value() < b.value();
	}
};

bool compare(std::pair<RGB, int> a, std::pair<RGB, int> b)
{
	return a.second < b.second;
}

int main()
{
	std::map<RGB, int> pixels;

	CImg<unsigned char> img("99.png");

	RGB rgb;
	cimg_forXY(img, x, y)
	{
		rgb.r = img(x, y, 0);
		rgb.g = img(x, y, 1);
		rgb.b = img(x, y, 2);
		pixels[rgb]++;
	}

	std::vector<std::pair<RGB, int> > v;
	std::pair<RGB, int> p;

	std::map<RGB, int>::iterator it;
	for (it = pixels.begin(); it != pixels.end(); ++it)
	{
		std::cout << ((RGB)it->first).value() << "-" << it->second << std::endl;
		p = std::pair<RGB, int>((RGB)it->first, it->second);
		v.push_back(p);
	}

	sort(v.begin(), v.end(), compare);

	std::vector<std::pair<RGB, int> >::iterator itv;
	for (itv = v.begin(); itv != v.end(); ++itv)
	{
		std::cout << (itv->first).value() << "-" << itv->second << std::endl;
	}

	return 0;
}
