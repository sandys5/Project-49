#include "Keyframe.h"

struct keyframe {
	int f;				// frame #
	float x, y, z;			// x, y, and z locations
	float ax, ay, az;		// angles in degrees
	float dxdf, dydf, dzdf;		// derivatives (compute in InitGraphics)
	float daxdf, daydf, dazdf; //
};

Keyframe::Keyframe(float Base)
{
	NowKeyFrame = 0;
	init = 0;
	BaseTime =  Base* 1000;
	Frames = NULL;
	first = 1;
	currentTime = 0;
	Repeat = 1;
}

void Keyframe::keyframeData(float listData[][7],int size) {
	//Check if the there is right amount of arguments, if so calloc memory for the keyframe array, otherwise return an error
	Frames = (struct keyframe*)calloc(size, sizeof(struct keyframe));
	for (int i = 0; i < size; i++) {
		Frames[i].f = listData[i][0];
		Frames[i].x = listData[i][1];
		Frames[i].y = listData[i][2];
		Frames[i].z = listData[i][3];
		Frames[i].ax = listData[i][4];
		Frames[i].ay = listData[i][5];
		Frames[i].az = listData[i][6];
	}
	initializeKeyframeData(Frames[size-1].f);
}

void Keyframe::initializeKeyframeData(int size){
	for (int i = 0; Frames[i].f >= 0; i++)
	{
		if (i == 0)				// first keyframe
		{
			Frames[i].dxdf = Frames[i].dydf = Frames[i].dzdf = 0.;
			Frames[i].daxdf = Frames[i].daydf = Frames[i].dazdf = 0.;
		}
		else
		{
			if (Frames[i + 1].f < 0)		// last keyframe
			{
				Frames[i].dxdf = Frames[i].dydf = Frames[i].dzdf = 0.;
				Frames[i].daxdf = Frames[i].daydf = Frames[i].dazdf = 0.;
			}
			else				// keyframes in the middle
			{
				Frames[i].dxdf = (Frames[i + 1].x - Frames[i - 1].x) / (Frames[i + 1].f - Frames[i - 1].f);
				Frames[i].dydf = (Frames[i + 1].y - Frames[i - 1].y) / (Frames[i + 1].f - Frames[i - 1].f);
				Frames[i].dzdf = (Frames[i + 1].z - Frames[i - 1].z) / (Frames[i + 1].f - Frames[i - 1].f);
				Frames[i].daxdf = (Frames[i + 1].ax - Frames[i - 1].ax) / (Frames[i + 1].f - Frames[i - 1].f);
				Frames[i].daydf = (Frames[i + 1].ay - Frames[i - 1].ay) / (Frames[i + 1].f - Frames[i - 1].f);
				Frames[i].dazdf = (Frames[i + 1].az - Frames[i - 1].az) / (Frames[i + 1].f - Frames[i - 1].f);
			}
		}
		X = Frames[0].x;
		Y = Frames[0].y;
		Z = Frames[0].z;
		Ax = Frames[0].ax;
		Ay = Frames[0].ay;
		Az = Frames[0].az;
		init = 1;
		maxframes = size;
	}
}

void Keyframe::Update(){
	// # msec into the cycle ( 0 - MSEC-1 ):
	if (init) {
		if (first)
			currentTime = glutGet(GLUT_ELAPSED_TIME);
		int msec = (glutGet(GLUT_ELAPSED_TIME) - currentTime) % BaseTime;	// 0 - (MSEC-1)
		first = 0;
		// turn that into the current frame number:

		if (NowFrame <= maxframes - (maxframes*.05)) { //This basically allows us to stop at the last frame.
			float time01 = (float)msec / (float)(BaseTime - 1);		// 0. - 1.
			NowFrame = (float)maxframes * time01;
		}
		else {
			NowFrame = NowFrame + ((maxframes - NowFrame)*.9);
			if (Repeat) {
				Reset();
			}
		}
		if (Frames[NowKeyFrame].f <= NowFrame && NowFrame <= Frames[NowKeyFrame + 1].f)
		{
			dx0dt = Frames[NowKeyFrame].dxdf * (Frames[NowKeyFrame + 1].f - Frames[NowKeyFrame].f);
			dy0dt = Frames[NowKeyFrame].dydf * (Frames[NowKeyFrame + 1].f - Frames[NowKeyFrame].f);
			dz0dt = Frames[NowKeyFrame].dzdf * (Frames[NowKeyFrame + 1].f - Frames[NowKeyFrame].f);
			dax0dt = Frames[NowKeyFrame].daxdf * (Frames[NowKeyFrame + 1].f - Frames[NowKeyFrame].f);
			day0dt = Frames[NowKeyFrame].daydf * (Frames[NowKeyFrame + 1].f - Frames[NowKeyFrame].f);
			daz0dt = Frames[NowKeyFrame].dazdf * (Frames[NowKeyFrame + 1].f - Frames[NowKeyFrame].f);

			dx1dt = Frames[NowKeyFrame + 1].dxdf * (Frames[NowKeyFrame + 1].f - Frames[NowKeyFrame].f);
			dy1dt = Frames[NowKeyFrame + 1].dydf * (Frames[NowKeyFrame + 1].f - Frames[NowKeyFrame].f);
			dz1dt = Frames[NowKeyFrame + 1].dzdf * (Frames[NowKeyFrame + 1].f - Frames[NowKeyFrame].f);
			dax1dt = Frames[NowKeyFrame + 1].daxdf * (Frames[NowKeyFrame + 1].f - Frames[NowKeyFrame].f);
			day1dt = Frames[NowKeyFrame + 1].daydf * (Frames[NowKeyFrame + 1].f - Frames[NowKeyFrame].f);
			daz1dt = Frames[NowKeyFrame + 1].dazdf * (Frames[NowKeyFrame + 1].f - Frames[NowKeyFrame].f);

			ax = 2.*Frames[NowKeyFrame].x - 2.*Frames[NowKeyFrame + 1].x + dx0dt + dx1dt;
			ay = 2.*Frames[NowKeyFrame].y - 2.*Frames[NowKeyFrame + 1].y + dy0dt + dy1dt;
			az = 2.*Frames[NowKeyFrame].z - 2.*Frames[NowKeyFrame + 1].z + dz0dt + dz1dt;
			aax = 2.*Frames[NowKeyFrame].ax - 2.*Frames[NowKeyFrame + 1].ax + dax0dt + dax1dt;
			aay = 2.*Frames[NowKeyFrame].ay - 2.*Frames[NowKeyFrame + 1].ay + day0dt + day1dt;
			aaz = 2.*Frames[NowKeyFrame].az - 2.*Frames[NowKeyFrame + 1].az + daz0dt + daz1dt;

			bx = -3.*Frames[NowKeyFrame].x + 3.*Frames[NowKeyFrame + 1].x - 2.*dx0dt - dx1dt;
			by = -3.*Frames[NowKeyFrame].y + 3.*Frames[NowKeyFrame + 1].y - 2.*dy0dt - dy1dt;
			bz = -3.*Frames[NowKeyFrame].z + 3.*Frames[NowKeyFrame + 1].z - 2.*dz0dt - dz1dt;
			bax = -3.*Frames[NowKeyFrame].ax + 3.*Frames[NowKeyFrame + 1].ax - 2.*dax0dt - dax1dt;
			bay = -3.*Frames[NowKeyFrame].ay + 3.*Frames[NowKeyFrame + 1].ay - 2.*day0dt - day1dt;
			baz = -3.*Frames[NowKeyFrame].az + 3.*Frames[NowKeyFrame + 1].az - 2.*daz0dt - daz1dt;

			cx = dx0dt;
			cy = dy0dt;
			cz = dz0dt;
			cax = dax0dt;
			cay = day0dt;
			caz = daz0dt;

			dx = Frames[NowKeyFrame].x;
			dy = Frames[NowKeyFrame].y;
			dz = Frames[NowKeyFrame].z;
			dax = Frames[NowKeyFrame].ax;
			day = Frames[NowKeyFrame].ay;
			daz = Frames[NowKeyFrame].az;

			t = (float)(NowFrame - Frames[NowKeyFrame].f) / (float)(Frames[NowKeyFrame + 1].f - Frames[NowKeyFrame].f);
			X = dx + t * (cx + t * (bx + t * ax));
			Y = dy + t * (cy + t * (by + t * ay));
			Z = dz + t * (cz + t * (bz + t * az));
			Ax = dax + t * (cax + t * (bax + t * aax));
			Ay = day + t * (cay + t * (bay + t * aay));
			Az = daz + t * (caz + t * (baz + t * aaz));
		}
		else {
			NowKeyFrame++;
		}
	}
}

void Keyframe::Reset() {
	NowKeyFrame = 0;
	NowFrame = 0;
	first = 1;
	currentTime = 0;
	dx0dt = NULL;
	dy0dt = NULL;
	dz0dt = NULL;
	dax0dt = NULL;
	day0dt = NULL;
	daz0dt = NULL;

	dx1dt = NULL;
	dy1dt = NULL;

	dz1dt = NULL;
	dax1dt = NULL;
	day1dt = NULL;
	daz1dt = NULL;

	ax = NULL;
	ay = NULL;
	az = NULL;
	aax = NULL;
	aay = NULL;
	aaz = NULL;

	bx = NULL;
	by = NULL;
	bz = NULL;
	bax = NULL;
	bay = NULL; 
	baz = NULL;

	cx = dx0dt;
	cy = dy0dt;
	cz = dz0dt;
	cax = dax0dt;
	cay = day0dt;
	caz = daz0dt;

	dx = NULL;
	dy = NULL;
	dz = NULL;
	dax = NULL;
	day = NULL;
	daz = 

	t = NULL;
	X = Frames[0].x;
	Y = Frames[0].y;
	Z = Frames[0].z;
	Ax = Frames[0].ax;
	Ay = Frames[0].ay;
	Az = Frames[0].az;
}

Keyframe::~Keyframe()
{
	free(Frames);
}
