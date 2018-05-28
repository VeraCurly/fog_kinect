#include "stdafx.h"
#include "lowPassFilter.h"

LowPassFilter::LowPassFilter(double aT, double aInterval) :
	t(aT),
	interval(aInterval)
{
	if (t <= 0 || interval <= 0)
		throw "invalid parameters for LowPassFilter";

	alpha = t / interval;
	reset();
}

void LowPassFilter::reset()
{
	x = 0.0;
	y = 0.0;
}

void LowPassFilter::filter(double& aX, double& aY)
{
	if (x == 0.0 && y == 0.0)
	{
		x = aX;
		y = aY;
		return;
	}

	x = (aX + alpha * x) / (1.0 + alpha);
	y = (aY + alpha * y) / (1.0 + alpha);

	aX = x;
	aY = y;
}
