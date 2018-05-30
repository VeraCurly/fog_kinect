#pragma once
#include "stdafx.h"

#include "point.h"

#include <Kinect.h>

/* Moves the mouse pointer */
class MouseController
{
public:
	/* Controller mode */
	enum class Mode : int {
		Undefined,
		Tapping,
		Touching
	};

	/* Constructor
	* @param("aDistanceToScreen") Distance to screen in meters
	*/
	MouseController(Mode aInitialMode);

	/* Sets and gets the mode
	* @param("aNewMode")? new mode; does nothing if missing or "Mode::Undefined"
	* @returns Current mode 
	*/
	Mode mode(Mode aNewMode = Mode::Undefined);

	/* Feed screen points to move the mouse
	* @param("aPoint") Screen point
	*/
	void feed(Point aPoint, double aDepth);

	/* Call this function to indicate that the finger is not in the interaction space
	*/
	DWORD pass(double aDepth);

private:
	Mode _mode;

	bool _isInteracting;

	int _screenHeight;
	double _dx, _dy;
	DWORD _dwStart;
};
