#pragma once
#include "stdafx.h"

#include "point.h"

#include <Kinect.h>

#define CALIBRATOR_POINT_COUNT 4
#define CALIBRATOR_SAMPLES_PER_POINT 11

/* Calibrates the Kinect on a fog screen */
class Calibrator
{
public:
	/* Calibrator states */
	enum class State : int {
		Initialized = 0,	/* Not yes started */
		Off = 1,			/* Last point was not in the calibration space */
		Calibrating = 2,	/* Calibrating a point */
		Completed = 4,		/* The calibration is completed */
		LoadedFromFile = 8, /* The calibration is loaded from file */
		Ready = Completed | LoadedFromFile
	};

	/* Calibrator events */
	enum class Event : int {
		Invalid = -1,	/* Calibration is completed, samples should not be fed in anymore */
		None,			/* No event */
		Started,		/* Calibration started */
		PointStart,		/* Started to calibrate a point */
		PointAbort,		/* Point calibration was aborted */
		PointEnd,		/* Finished point calibration */
		Finished,		/* Finished the calibration */
	};

	/* Constructor
	* @param("aDistanceToScreen") Distance to screen in meters
	*/
	Calibrator(double aDistanceToScreen);

	/* Gets the current state
	* @returns Calibrator state
	*/
	State state() { return _state; }

	/* Feed Kinect camera points to the calibrator until it gets calibrated
	* @param("aCameraPoint") Kinect camera point
	* @returns Processing event
	*/
	Event feed(CameraSpacePoint aCameraPoint);

	/* Maps Kinect camera point to the fog screen
	* @param("aCameraPoint") Kinect camera point
	* @param("aScreenPoint") Receiver for the screen point
	* @returns False if the calibrator was not configured yet (state & State::Ready == 0) or the point is too far; True otherwise
	*/
	bool map(CameraSpacePoint aCameraPoint, Point& aScreenPoint);
	
private:
	double _distanceToScreen;

	double _screenWidth;
	double _screenHeight;

	int _calibPointIndex;
	double _calibPoints[CALIBRATOR_POINT_COUNT][3];

	int _sampleIndex;
	double _samplesX[CALIBRATOR_SAMPLES_PER_POINT];
	double _samplesY[CALIBRATOR_SAMPLES_PER_POINT];
	double _samplesZ[CALIBRATOR_SAMPLES_PER_POINT];

	double _scaleX, _scaleY;
	double _offsetX, _offsetY;

	State _state;

	void Configure();
	void SaveToFile();
	void LoadFromFile();
};
