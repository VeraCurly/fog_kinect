// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Kinect.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <Windows.h>
#include <iostream>
#include <time.h> 
#include <algorithm>
#include <fstream>

#include "lowPassFilter.h"
#include "calibrator.h"

#pragma comment(lib, "Winmm.lib")

using namespace std;

//safe way of deleting a COM object
template<typename T>
void SafeRelease(T& ptr) { if (ptr) { ptr->Release(); ptr = nullptr; } }

// Kinect depth data source
IKinectSensor* sensor = nullptr;
IBodyFrameReader* bodyReader;

time_t t0;

int main()
{
	double distanceToScreen = 1.97;

	Calibrator calibrator(distanceToScreen);
	LowPassFilter pointSmoother(300, 33);
	LowPassFilter depthSmoother(100, 33);

	ofstream logFile;
	logFile.open("log.txt");
	if (!logFile.is_open())
	{
		cout << "Can't open a file\n"; 
		return 1;
	}
	
	HRESULT hr = S_OK;

	if (FAILED(GetDefaultKinectSensor(&sensor))) return 0;
	if (!sensor)
		return 1;

	sensor->Open();
	IBodyFrameSource* bodysource = nullptr;
	hr = sensor->get_BodyFrameSource(&bodysource);
	if (FAILED(hr)) 
		return 1;

	hr = bodysource->OpenReader(&bodyReader);
	if (FAILED(hr)) 
		return 1;

	while (1) {
		IBodyFrame* frame = nullptr;
		if (FAILED(bodyReader->AcquireLatestFrame(&frame)))
			continue;

		IBody* Body[BODY_COUNT] = { 0 };
		hr = frame->GetAndRefreshBodyData(BODY_COUNT, Body);

		SafeRelease(frame);

		if (FAILED(hr))
			continue;

		for (int count = 0; count < BODY_COUNT; count++)
		{
			BOOLEAN isTracking = false;
			hr = Body[count]->get_IsTracked(&isTracking);
			if (FAILED(hr) || !isTracking)
				continue;

			Joint joint[JointType::JointType_Count];
			hr = Body[count]->GetJoints(JointType::JointType_Count, joint);
			if (FAILED(hr))
				continue;

			if (joint[JointType_HandTipRight].TrackingState != TrackingState::TrackingState_NotTracked)
			{
				// this is the finger we need
				ICoordinateMapper* coordinateMapper;
									
				hr = sensor->get_CoordinateMapper(&coordinateMapper);
				CameraSpacePoint point = joint[JointType_HandTipRight].Position;
				ColorSpacePoint colorpoint;
				coordinateMapper->MapCameraPointToColorSpace(point, &colorpoint);

				double depth = point.Z;
				//depthSmoother.filter(depth);

				if (calibrator.state() != Calibrator::State::Completed)
				{
					Calibrator::Event e = calibrator.feed(point);
					switch (e) {
					case Calibrator::Event::Started:
						printf("Calibration started\n");
						break;
					case Calibrator::Event::PointEnd:
						PlaySoundA("sound.wav", NULL, SND_ASYNC);
						printf("Point calibrated\n");
						break;
					case Calibrator::Event::Finished:
						printf("Calibration completed\n");
						break;
					}
				}
				else if (depth < distanceToScreen)
				{
					Point sp;
					if (calibrator.map(point, sp))
					{
						pointSmoother.filter(sp.X, sp.Y);
						SetCursorPos(sp.X, sp.Y);

						logFile << sp.X << " " << sp.Y << " " << point.Z << " " << endl;
					}
				}
				else {
					pointSmoother.reset();
				}
			}
		}

		SafeRelease(Body[count]);
	}

	logFile.close();
	return 0;
}
