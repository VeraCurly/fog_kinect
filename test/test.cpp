#include "stdafx.h"

#include <cmath>
#include <iostream>

#include <windows.h>

#include "..\src\calibrator.h"

double random(double aSize)
{
	return aSize * (double(rand() % 10001) / 5000.0 - 1.0);
}

void WaitKeyPress(char* aMessage)
{
	std::cout << "\n... Press spacebar to " << aMessage << " ...\n";
	while (!::GetKeyState(VK_SPACE))
	{
		Sleep(16);
	}
}

void TestCalibrator()
{
	std::cout << "\n -- TEST CALIBRATOR --\n";

	double distanceToScreen = 1.8;
	int gestureSampleCount = 30;
	int gestureCount = 12;

	Calibrator calibrator(distanceToScreen);

	int samplesCount = gestureSampleCount * gestureCount;
	CameraSpacePoint* samples = new CameraSpacePoint[samplesCount];

	int i = 0;
	int gestureIndex = 0;
	double noiseLevel = 0.0;

	Point finger[4] = {
		Point(-0.4, -0.6),
		Point(0.4, -0.6),
		Point(-0.4, 0.6),
		Point(0.4, 0.6)
	};

	// first point in 
	for (++gestureIndex; i < gestureSampleCount; i++) {
		samples[i] = { finger[0].X + random(noiseLevel), finger[0].Y + random(noiseLevel), distanceToScreen + 0.2 - 0.3 * i / gestureSampleCount };
	}
	// first point out
	for (++gestureIndex; i < gestureSampleCount * gestureIndex; i++) {
		samples[i] = { finger[0].X + random(noiseLevel), finger[0].Y + random(noiseLevel), distanceToScreen + 0.2 - 0.3 * (gestureSampleCount * gestureIndex - i) / gestureSampleCount };
	}
	// second point in 
	for (++gestureIndex; i < gestureSampleCount * gestureIndex; i++) {
		samples[i] = { finger[1].X + random(noiseLevel), finger[1].Y + random(noiseLevel), distanceToScreen + 0.2 - 0.35 * (i - gestureSampleCount * (gestureIndex - 1)) / gestureSampleCount };
	}
	// second point out
	for (++gestureIndex; i < gestureSampleCount * gestureIndex; i++) {
		samples[i] = { finger[1].X + random(noiseLevel), finger[1].Y + random(noiseLevel), distanceToScreen + 0.2 - 0.35 * (gestureSampleCount * gestureIndex - i) / gestureSampleCount };
	}
	// second point in again!
	for (++gestureIndex; i < gestureSampleCount * gestureIndex; i++) {
		samples[i] = { finger[1].X + random(noiseLevel), finger[1].Y + random(noiseLevel), distanceToScreen + 0.2 - 0.35 * (i - gestureSampleCount * (gestureIndex - 1)) / gestureSampleCount };
	}
	// second point out again!
	for (++gestureIndex; i < gestureSampleCount * gestureIndex; i++) {
		samples[i] = { finger[1].X + random(noiseLevel), finger[1].Y + random(noiseLevel), distanceToScreen + 0.2 - 0.35 * (gestureSampleCount * gestureIndex - i) / gestureSampleCount };
	}
	// third point in partially
	for (++gestureIndex; i < gestureSampleCount * gestureIndex; i++) {
		samples[i] = { finger[2].X + random(noiseLevel), finger[2].Y + random(noiseLevel), distanceToScreen + 0.27 - 0.3 * (i - gestureSampleCount * (gestureIndex - 1)) / gestureSampleCount };
	}
	// third point out, collecting only 6 samples
	for (++gestureIndex; i < gestureSampleCount * gestureIndex; i++) {
		samples[i] = { finger[2].X + random(noiseLevel), finger[2].Y + random(noiseLevel), distanceToScreen + 0.27 - 0.3 * (gestureSampleCount * gestureIndex - i) / gestureSampleCount };
	}
	// third point in again
	for (++gestureIndex; i < gestureSampleCount * gestureIndex; i++) {
		samples[i] = { finger[2].X + random(noiseLevel), finger[2].Y + random(noiseLevel), distanceToScreen + 0.27 - 0.4 * (i - gestureSampleCount * (gestureIndex - 1)) / gestureSampleCount };
	}
	// third point out again
	for (++gestureIndex; i < gestureSampleCount * gestureIndex; i++) {
		samples[i] = { finger[2].X + random(noiseLevel), finger[2].Y + random(noiseLevel), distanceToScreen + 0.27 - 0.4 * (gestureSampleCount * gestureIndex - i) / gestureSampleCount };
	}
	// forth point in
	for (++gestureIndex; i < gestureSampleCount * gestureIndex; i++) {
		samples[i] = { finger[3].X + random(noiseLevel), finger[3].Y + random(noiseLevel), distanceToScreen + 0.2 - 0.3 * (i - gestureSampleCount * (gestureIndex - 1)) / gestureSampleCount };
	}
	// forth point out
	for (++gestureIndex; i < gestureSampleCount * gestureIndex; i++) {
		samples[i] = { finger[3].X + random(noiseLevel), finger[3].Y + random(noiseLevel), distanceToScreen + 0.2 - 0.3 * (gestureSampleCount * gestureIndex - i) / gestureSampleCount };
	}

	for (i = 0; i < samplesCount; i++)
	{
		if (i % gestureSampleCount == 0)
			std::cout << " -- " << (i / gestureSampleCount) << " -- \n";

		CameraSpacePoint& sample = samples[i];
		Calibrator::Event e = calibrator.feed(sample);
		//std::cout << sample.X << " " << sample.Y << " " << sample.Z << "   event: " << (int)e << "   state: " << (int)calibrator.state() << "\n";
	}

	std::cout << " ------------------- \n";

	for (i = 0; i < 4; i++)
	{
		Point& f = finger[i];
		CameraSpacePoint sample;
		sample.X = f.X + random(0.0002);
		sample.Y = f.Y + random(0.0002);
		sample.Z = 1;
		Point sp;
		if (calibrator.map(sample, sp))
			printf("%.4f %.4f %.4f mapped to %.0f %.0f\n", sample.X, sample.Y, sample.Z, sp.X, sp.Y);
		else
			printf("Cannot map %.4f %.4f %.4f\n", sample.X, sample.Y, sample.Z, sp.X, sp.Y);
	}

	delete[] samples;

	WaitKeyPress("continue");
}

void TestCalibratorFromFile()
{
	std::cout << "\n -- TEST CALIBRATOR FROM FILE --\n";

	double distanceToScreen = 1.8;
	Calibrator calibrator(distanceToScreen);

	Point finger[4] = {
		Point(-0.4, -0.6),
		Point(0.4, -0.6),
		Point(-0.4, 0.6),
		Point(0.4, 0.6)
	};

	for (int i = 0; i < 4; i++)
	{
		Point& f = finger[i];

		CameraSpacePoint sample;
		sample.X = f.X + random(0.0002);
		sample.Y = f.Y + random(0.0002);
		sample.Z = 1;

		Point sp;
		if (calibrator.map(sample, sp))
			printf("%.4f %.4f %.4f mapped to %.0f %.0f\n", sample.X, sample.Y, sample.Z, sp.X, sp.Y);
		else
			printf("Cannot map %.4f %.4f %.4f\n", sample.X, sample.Y, sample.Z, sp.X, sp.Y);
	}

	WaitKeyPress("continue");
}


int main()
{
	TestCalibrator();
	TestCalibratorFromFile();

	WaitKeyPress("exit");

	return 0;
}
