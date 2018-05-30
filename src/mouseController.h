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
		Dwelling,
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
	* @param("aScreenPoint") Kinect camera point
	* @returns Processing event
	*/
	void feed(CameraSpacePoint aCameraPoint);

	/* Call this function to indicate that the finger is not in the interaction space
	*/
	void pass();

private:
	Mode _mode;
};
