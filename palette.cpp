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

#include "palette.h"

using namespace std;
using namespace cimg_library;

class LAB
{
	public:
	unsigned char l;
	unsigned char a;
	unsigned char b;

	LAB(unsigned char l, unsigned char a, unsigned char b) : l(l), a(a), b(b) {}

	const float distance(LAB b)
	{
		int dl = this->l - b.l;
		int da = this->a - b.a;
		int db = this->b - b.b;
		return sqrt(dl * dl + da * da + db * db);
	}
};

class RGB
{
	public:
	unsigned char r;
	unsigned char g;
	unsigned char b;
	char str[7 + 1];

	RGB() {}
	RGB(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {}

	const unsigned int value() const
	{
		return (r << 16) + (g << 8) + b;
	}

	const bool operator<(const RGB b) const
	{
		return this->value() < b.value();
	}

	const LAB toLab() const
	{
		int b = this->b, g = this->g, r = this->r;
		int x, y, z, f;
		int L, a;

		x = b*labXb + g*labXg + r*labXr;
		y = b*labYb + g*labYg + r*labYr;
		z = b*labZb + g*labZg + r*labZr;

		f = x > labT;
		x = CV_DESCALE( x, lab_shift );

		if( f )
				assert( (unsigned)x < 512 ), x = icvLabCubeRootTab[x];
		else
				x = CV_DESCALE(x*labSmallScale + labSmallShift,lab_shift);

		f = z > labT;
		z = CV_DESCALE( z, lab_shift );

		if( f )
				assert( (unsigned)z < 512 ), z = icvLabCubeRootTab[z];
		else
				z = CV_DESCALE(z*labSmallScale + labSmallShift,lab_shift);

		f = y > labT;
		y = CV_DESCALE( y, lab_shift );

		if( f )
		{
				assert( (unsigned)y < 512 ), y = icvLabCubeRootTab[y];
				L = CV_DESCALE(y*labLScale - labLShift, 2*lab_shift );
		}
		else
		{
				L = CV_DESCALE(y*labLScale2,lab_shift);
				y = CV_DESCALE(y*labSmallScale + labSmallShift,lab_shift);
		}

		a = CV_DESCALE( 500*(x - y), lab_shift ) + 128;
		b = CV_DESCALE( 200*(y - z), lab_shift ) + 128;

		return LAB(CV_CAST_8U(L), CV_CAST_8U(a), CV_CAST_8U(b));
	}

	const string to_str() const
	{
		snprintf((char*)this->str, 7 + 1, "#%02X%02X%02X", this->r, this->g, this->b);
		return string(str);
	}
};

bool compare(pair<RGB, int> a, pair<RGB, int> b)
{
	return a.second > b.second;
}

void get_colors(const char* filename, vector<RGB>& colors)
{
	colors.clear();

	map<RGB, int> pixels;
	CImg<unsigned char> img(filename);

	// Count colour occurences
	RGB rgb;
	cimg_forXY(img, x, y)
	{
		rgb = RGB(img(x, y, 0), img(x, y, 1), img(x, y, 2));
		pixels[rgb]++;
	}

	vector<pair<RGB, int>> pixel_counts;
	pixel_counts.resize(pixels.size());
	copy(pixels.begin(), pixels.end(), pixel_counts.begin());

	/*
	for (const auto& kv : pixels)
	{
		pair<RGB, int> p = pair<RGB, int>(kv.first, kv.second);
		pixel_counts.push_back(p);
	}
	*/

	// Sort by popularity
	sort(pixel_counts.begin(), pixel_counts.end(), compare);


	vector<pair<RGB, int>> grouped_pixel_counts;
	for (auto& pair1 : pixel_counts)
	{
		// TODO: group similar colors..
	}

	for (auto& pair : pixel_counts)
	{
		// Discard if not many occurences
		if (pair.second < 50)
			continue;

		cout << (pair.first).to_str() << "-" << pair.second << endl;

		RGB candidate = pair.first;

		// Discard if similar to colors already chosen
		bool similar = false;
		for (RGB& color : colors)
		{
			LAB l = color.toLab();

			cout << "  " << color.to_str() << " - " << l.distance(candidate.toLab()) << "\n";

			if (l.distance(candidate.toLab()) < 10)
			{
				similar = true;
				break;
			}
		}

		if (!similar)
			colors.push_back(candidate);

	}
}

const string to_str(RGB color)
{
	return string("<div style=\"width: 30px; height: 30px; background-color: ") + string(color.to_str()) + string("\"></div>");
}

const string join(const string a, const string b)
{
	if (a.empty()) return b;
	return a + " " + b;
}

int main(int argc, char* argv[])
{
	vector<RGB> colors;
	vector<string> str_colors;

	string filename;
	while (getline(cin, filename))
	{
		get_colors(filename.c_str(), colors);

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
