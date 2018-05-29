#include "stdafx.h"

#include "calibrator.h"

#include <windows.h>

#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

const double SAFETY_ZONE_RADIUS = 0.15; // units of Kinect CameraPoint

Calibrator::Calibrator(double aDistanceToScreen) :
	_distanceToScreen(aDistanceToScreen),
	_calibPointIndex(-1),
	_sampleIndex(0),
	_state(State::Initialized)
{
	_screenWidth = GetSystemMetrics(SM_CXSCREEN);
	_screenHeight = GetSystemMetrics(SM_CYSCREEN);

	LoadFromFile();
}

Calibrator::Event Calibrator::feed(CameraSpacePoint aCameraPoint)
{
	// quit if calibraiton is done already
	if (_state == State::Completed)
		return Event::Invalid;

	// ignore the very first sample and report that the calibration has started
	if (_state == State::Initialized || _state == State::LoadedFromFile)
	{
		_calibPointIndex = 0;
		_state = State::Off;
		return Event::Started;
	}

	Event result = Event::None;

	// stop if the distance exceed the threshold
	if (aCameraPoint.Z > _distanceToScreen)
	{
		if (_calibPointIndex == CALIBRATOR_POINT_COUNT)	// done
		{
			Configure();
			SaveToFile();

			_state = State::Completed;
			result = Event::Finished;
		}
		else 	// we are still calibrating, the current calibration point buffer must be empty, 
				// i.e. we reset the buffer if the finger is out while having too few points in the buffer
		{
			if (_state == State::Calibrating)
			{
				_state = State::Off;
				result = Event::PointAbort;
			}
			_sampleIndex = 0;
		}

		return result;
	}

	// stop if the too close to the last calibrated point
	if (_state == State::Off && _calibPointIndex > 0)
	{
		double prevX = _calibPoints[_calibPointIndex - 1][0];
		double prevY = _calibPoints[_calibPointIndex - 1][1];
		double dx = aCameraPoint.X - prevX;
		double dy = aCameraPoint.Y - prevY;
		double distToPrevPoint = sqrt(dx*dx + dy*dy);
		if (distToPrevPoint < SAFETY_ZONE_RADIUS)
			return result;
	}

	// we are in the point calibration zone
	if (_state != State::Calibrating)
		result = Event::PointStart;

	_state = State::Calibrating;

	// add the sample
	_samplesX[_sampleIndex] = aCameraPoint.X;
	_samplesY[_sampleIndex] = aCameraPoint.Y;
	_samplesZ[_sampleIndex] = aCameraPoint.Z;
	_sampleIndex++;

	if (_sampleIndex == CALIBRATOR_SAMPLES_PER_POINT)
	{
		// finilize the calibration point
		std::sort(_samplesX, _samplesX + CALIBRATOR_SAMPLES_PER_POINT);
		std::sort(_samplesY, _samplesY + CALIBRATOR_SAMPLES_PER_POINT);
		std::sort(_samplesZ, _samplesZ + CALIBRATOR_SAMPLES_PER_POINT);

		/*
		for (int i = 0; i < CALIBRATOR_SAMPLES_PER_POINT; i++)
		{
			printf("Point! %f %f %f\n", samplesX[i], samplesY[i], samplesZ[i]);
		}*/

		int middleIndex = CALIBRATOR_SAMPLES_PER_POINT / 2;
		_calibPoints[_calibPointIndex][0] = _samplesX[middleIndex];
		_calibPoints[_calibPointIndex][1] = _samplesY[middleIndex];
		_calibPoints[_calibPointIndex][2] = _samplesZ[middleIndex];

		std::cout << "-- Calib point at " << _calibPoints[_calibPointIndex][0] << " " << _calibPoints[_calibPointIndex][1] << " " << _calibPoints[_calibPointIndex][2] << "\n";

		// get ready for the next point
		_sampleIndex = 0;
		_calibPointIndex++;
		_state = State::Off;
		result = Event::PointEnd;
	}

	return result;
}

bool Calibrator::map(CameraSpacePoint aCameraPoint, Point& aScreenPoint)
{
	if (_state != State::Completed && _state != State::LoadedFromFile)
		return false;

	if (aCameraPoint.Z > _distanceToScreen)
		return false;

	aScreenPoint.X = (aCameraPoint.X - _offsetX) * _scaleX;
	aScreenPoint.Y = (aCameraPoint.Y - _offsetY) * _scaleY;

	return true;
}

void Calibrator::Configure()
{
	std::cout << "\n -- Calib points -- \n";
	for (int i = 0; i < CALIBRATOR_POINT_COUNT; i++)
	{
		std::cout << _calibPoints[i][0] << " " << _calibPoints[i][1] << " " << _calibPoints[i][2] << "\n";
	}
	std::cout << "\n";

	_offsetX = _calibPoints[0][0];
	_offsetY = _calibPoints[0][1];

	_scaleX = _screenWidth / (((_calibPoints[1][0] - _calibPoints[0][0]) + (_calibPoints[3][0] - _calibPoints[2][0])) / 2);
	_scaleY = _screenHeight / (((_calibPoints[2][1] - _calibPoints[0][1]) + (_calibPoints[3][1] - _calibPoints[1][1])) / 2);
}

void Calibrator::SaveToFile()
{
	std::ofstream calibFile("last-calib.txt");
	if (!calibFile.is_open())
	{
		std::cout << "[ERROR] Cannot open calibration file for writing.\n";
		return;
	}

	for (int i = 0; i < CALIBRATOR_POINT_COUNT; i++)
	{
		calibFile << _calibPoints[i][0] << " " << _calibPoints[i][1] << " " << _calibPoints[i][2] << "\n";
	}

	std::cout << "Saved to file " << "\n";
	calibFile.close();
}


void Calibrator::LoadFromFile()
{
	std::ifstream calibFile("last-calib.txt");
	if (!calibFile.is_open())
	{
		std::cout << "[ERROR] Cannot open calibration file for reading.\n";
		return;
	}

	char buffer[256];
	double x, y, z;
	int pointIndex = 0;
	try
	{
		while (!calibFile.eof() && pointIndex < CALIBRATOR_POINT_COUNT)
		{
			calibFile.getline(buffer, 256);
			std::istringstream iss(buffer);
			iss >> x >> y >> z;

			_calibPoints[pointIndex][0] = x;
			_calibPoints[pointIndex][1] = y;
			_calibPoints[pointIndex][2] = z;

			pointIndex++;
		}

		if (pointIndex == CALIBRATOR_POINT_COUNT)
		{
			_state = State::LoadedFromFile;
			Configure();
		}
	}
	catch (...)
	{
		std::cout << "[ERROR] Calibration file format is invalid.\n";
	}

	calibFile.close();
}