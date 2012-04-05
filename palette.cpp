#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <numeric>
#include <math.h>
#include "lib/CImg-1.4.9/CImg.h"
#include <stdio.h>

#include "color.hpp"

using namespace std;
using namespace cimg_library;

typedef vector<pair<RGB, int> > colordist;
typedef map<RGB, int> colormap;
typedef CImg<unsigned char> Image;

const float MAX_DELTA_E = 2.0;
const int MAX_COLORS = 6;
const char* COLOR_DELIMITER = ",";

/**
 * Get the exact color count from the image.
 */
void count_point_colors(const Image& img, colordist& pixel_counts)
{
	colormap pixels;

	// Count colour occurences
	RGB rgb;
	cimg_forXY(img, x, y)
	{
		rgb = RGB(img(x, y, 0), img(x, y, 1), img(x, y, 2));
		pixels[rgb]++;
	}

	pixel_counts.clear();
	pixel_counts.resize(pixels.size());
	copy(pixels.begin(), pixels.end(), pixel_counts.begin());

	// Sort by popularity
	sort(pixel_counts.begin(), pixel_counts.end(), compare);
}

float color_distance(const RGB& lhs, const RGB& rhs)
{
	return lhs.toLab().distance(rhs.toLab());
}

void find_nearest_color(const RGB& c, const colordist& colors,
		pair<RGB, float>& match)
{
	assert(colors.size() > 0);
	// initialize match against first color
	match = make_pair(colors[0].first, color_distance(colors[0].first, c));

	float dist;
	RGB neighbour;
	for (int i = 1; i < colors.size(); i++) {
		neighbour = colors[i].first;
		dist = color_distance(neighbour, c);
		if (dist < match.second)
		{
			// found a closer color
			match.first = neighbour;
			match.second = dist;
		}
	}
}

/**
 * Group nearby colors together.
 */
void aggregate_colors(const colordist& point_colors,
		colordist& grouped_colors)
{
	colormap index;
	grouped_colors.clear();

	// add black and white to our list
	RGB black(0, 0, 0);
	RGB white(255, 255, 255);
	grouped_colors.push_back(make_pair(black, 0));
	index[black] = 0;
	grouped_colors.push_back(make_pair(white, 0));
	index[white] = 1;

	pair<RGB, float> nearest_color;
	int i;

	for (auto& pair : point_colors)
	{
		const RGB& c = pair.first;
		const int& count = pair.second;
		colormap::iterator it = index.find(c);

		if (it != index.end())
		{
			// seen already (probably black or white)
			grouped_colors[it->second].second += count;
			continue;
		}

		find_nearest_color(c, grouped_colors, nearest_color);
		if (nearest_color.second < MAX_DELTA_E)
		{
			// merge together
			it = index.find(nearest_color.first);
			grouped_colors[it->second].second += count;
		}
		else
		{
			// this color is new and different
			i = grouped_colors.size();
			grouped_colors.push_back(pair);
			index[c] = i;
		}
	}
	sort(grouped_colors.begin(), grouped_colors.end(), compare);
}

/**
 * Reduce colors to our final set.
 */
void reduce_colors(const colordist& grouped_colors, vector<RGB>& reduced_colors)
{
	int top_count = grouped_colors[0].second;
	int min_count = top_count / 10; // need 10% of the top count

	for (auto& colorcount: grouped_colors)
	{
		if (colorcount.second >= min_count)
		{
			reduced_colors.push_back(colorcount.first);
		}

		if (reduced_colors.size() >= MAX_COLORS)
		{
			break;
		}
	}
}

/**
 * Find the important colors in an image.
 * XXX What about background detection.
 */
void get_palette(const char* filename, vector<RGB>& colors)
{
	colors.clear();

	CImg<unsigned char> img(filename);

	colordist point_colors;
	count_point_colors(img, point_colors);

	colordist grouped_colors;
	aggregate_colors(point_colors, grouped_colors);

	reduce_colors(grouped_colors, colors);
}

string to_str(const RGB& color)
{
	return color.to_str();
	//return string("<div style=\"width: 30px; height: 30px; background-color: ") + string(color.to_str()) + string("\"></div>");
}

const string join(const string a, const string b)
{
	if (a.empty()) return b;
	return a + COLOR_DELIMITER + b;
}

int main(int argc, char* argv[])
{
	vector<RGB> colors;
	vector<string> str_colors;

	string filename;
	while (getline(cin, filename))
	{
		get_palette(filename.c_str(), colors);

		// Convert RGB colors to hex strings
		str_colors.resize(colors.size());
		transform(colors.begin(), colors.end(), str_colors.begin(), to_str);

		cout
			<< filename
			<< "\t"
			<< accumulate(str_colors.begin(), str_colors.end(), string(""), join)
			<< "\n";
	}

}
