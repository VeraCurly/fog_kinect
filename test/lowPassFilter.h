// lowPassFilter.h file
#pragma once
#include "stdafx.h"

class LowPassFilter
{
private:
	double t;
	double interval;

	double alpha;
	double x;
	double y;

public:
	/* @description Constructor
	* @param("aT") Filter strength
	* @param("aInterval") Data sampling interval in milliseconds
	*/
	LowPassFilter(double aT, double aInterval);

	/* @description Call this function every time when a click event starts or ends
	*/
	void reset();

	/* @description Applies filtering to X and Y. Note that the method modifies the original data.
	* @param("aX") point X coordinate
	* @param("aY") point Y coordinate
	*/
	void filter(double& aX, double& aY);
}; 
