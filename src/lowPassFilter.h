#pragma once
#include "stdafx.h"

/* Removes high-frequency noise */
class LowPassFilter
{
public:
	/* Constructor
	* @param("aT") Filter strength
	* @param("aInterval") Data sampling interval in milliseconds
	*/
	LowPassFilter(double aT, double aInterval);

	/* Call this function every time when a click event starts or ends
	*/
	void reset();

	/* Applies filtering to a value. Note that the method modifies the original data.
	* @param("aX") a value to smooth
	*/
	void filter(double& aX);

	/* Applies filtering to X and Y. Note that the method modifies the original data.
	* @param("aX") point X coordinate
	* @param("aY") point Y coordinate
	*/
	void filter(double& aX, double& aY);

private:
	double t;
	double interval;

	double alpha;
	double x;
	double y;
};
