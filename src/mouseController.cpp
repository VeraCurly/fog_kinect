#include "mouseController.h"

MouseController::MouseController(Mode aInitialMode) :
	_mode(aInitialMode)
{

}

MouseController::Mode MouseController::mode(Mode aNewMode)
{
	if (aNewMode != Mode::Undefined)
		_mode = aNewMode;

	return _mode;
}

void MouseController::feed(CameraSpacePoint aCameraPoint)
{
	// do something
}

void MouseController::pass()
{
	// do something
}