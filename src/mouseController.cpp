#include "stdafx.h"
#include "mouseController.h"

#include <iostream>

MouseController::MouseController(Mode aInitialMode) :
	_mode(aInitialMode),
	_isInteracting(false)
{
	_screenHeight = GetSystemMetrics(SM_CYSCREEN);
}

MouseController::Mode MouseController::mode(Mode aNewMode)
{
	if (aNewMode != Mode::Undefined)
		_mode = aNewMode;

	return _mode;
}

void MouseController::feed(Point aPoint, double aDepth)
{
	_isInteracting = true;
	if (_mode == Mode::Tapping)
	{
		if (aPoint.Y > 50 && aPoint.Y < (_screenHeight - 50))
		{
			SetCursorPos(aPoint.X, aPoint.Y);

			_dx = aPoint.X;
			_dy = aPoint.Y;
			_dwStart = GetTickCount();
		}
	}
	else if (_mode == Mode::Touching)
	{
		SetCursorPos(aPoint.X, aPoint.Y);
	}
}

DWORD MouseController::pass(double aDepth)
{
	DWORD result = 0;
	if (_mode == Mode::Tapping)
	{
		if (_isInteracting && aDepth < -0.05)
		{
			_isInteracting = false;

			PlaySoundA("sound.wav", NULL, SND_ASYNC);
			printf("Click! \n");
			DWORD dwPeriod = GetTickCount();
			result = dwPeriod - _dwStart;

			mouse_event(MOUSEEVENTF_LEFTDOWN, _dx, _dy, 0, 0);
			mouse_event(MOUSEEVENTF_LEFTUP, _dx, _dy, 0, 0);

			printf("\nProgram takes %6.0f seconds.\n", result);
		}
	}
	else if (_mode == Mode::Touching)
	{
		_isInteracting = false;
	}

	return result;
}