#pragma once
#ifndef KEYFRAME
#define KEYFRAME
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#include "glew.h"
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"
#endif
class Keyframe
{
public:
	struct keyframe* Frames;
	float X, Y, Z;
	float Ax, Ay, Az;
	int BaseTime;
	Keyframe(float);
	~Keyframe();
	void keyframeData(float[][7],int);
	void Update();
	void Reset();
private:
	float dx0dt, dy0dt, dz0dt; // slopes for the first point
	float dax0dt, day0dt, daz0dt; //slopes for the first angle
	float dx1dt, dy1dt, dz1dt; // slopes for the second point
	float dax1dt, day1dt, daz1dt; //slopes for the second angle
	float ax, bx, cx, dx; //x coefficients
	float ay, by, cy, dy; //y coefficients
	float az, bz, cz, dz; //z coefficients
	float aax, bax, cax, dax; //x  angle coefficients
	float aay, bay, cay, day; //y angle coefficients
	float aaz, baz, caz, daz; //z angle coefficients
	float t;
	int init;
	int first;
	int currentTime;
	int maxframes; //number of frames
	void initializeKeyframeData(int);
	float NowFrame;
	bool finished;
	int NowKeyFrame;
};