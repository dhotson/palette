/*
 *
 *  color.hpp
 *  palette-cpp
 *
 */

#include <cmath>
#include <cstdio>
#include <string>
#include <utility>

#include "conversions.hpp"

using namespace std;

class LAB
{
	public:
	unsigned char l;
	unsigned char a;
	unsigned char b;

	LAB(unsigned char l, unsigned char a, unsigned char b)
		: l(l), a(a), b(b) {}

	float distance(LAB b) const
	{
		int dl = this->l - b.l;
		int da = this->a - b.a;
		int db = this->b - b.b;
		return std::sqrt(dl * dl + da * da + db * db);
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
	RGB(unsigned char r, unsigned char g, unsigned char b)
		: r(r), g(g), b(b) {}

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

