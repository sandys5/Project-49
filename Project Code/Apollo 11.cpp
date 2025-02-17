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

#include "newloadobjfile.h"
#include "newloadobjfile.cpp"
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"
#include "glslprogram.cpp"
//#include "newloadobjfile.cpp"
//#include "mtl.cpp"
#include "Keyframe.cpp"
//#include "bmptotexture.cpp"


// title of the window:
const char *WINDOWTITLE = { "Apollo 11 Animation" };

// what the glui package defines as true and false:
const int GLUITRUE = { true };
const int GLUIFALSE = { false };

// delimiters for parsing the obj file:
#define OBJDELIMS		" \t"
//Animation Constant
#define MS_IN_THE_ANIMATION_CYCLE	15000
// the escape key:
#define ESCAPE		0x1b

// initial window size:
const int INIT_WINDOW_SIZE = { 800 };

//Textures
GLuint TexE, TexM, TexS, TexSt, AFlag, DayEarthTex, NightEarthTex, CloudEarthTex;

// Dimentions
// Earth to Moon ratio is realistic, Sun ratio had to be fudged
float MoonDiameter = 400;
float EarthDiameter = MoonDiameter * 4;
float SunDiameter = MoonDiameter * .75;

// Important positions
float BaseXYZ[] = { 175, 3, -88 };
float LaunchXYZ[] = { 6710, 385, -705 };
float LM_XYZ[] = { 20., 11.5, 15 };
float SaturnXYZ[] = { 900, 0, -100 };
float AstroXYZ[] = { 14 , 12, 9 };
float FlagXYZ[] = { 15, 11, 11 };
float MoonXYZ[] = { 0, 0, 0 };
float StarMapAnchor[] = { 0., 0., 0. };
float EarthXYZ[] = { MoonDiameter * 17, 0, 0 };
float SunXYZ[] = { MoonDiameter * 10 , MoonDiameter * 4,  -MoonDiameter * 10 };
float LightXYZ[] = { 220., 5, -220. };

//Animation First Frames 
float LM_Animate[] = { 20., 16., 15. };

//Animate Variables
const float TurnFactor = (.75 * M_PI) / 180;
float currentFactor = 0;
float rAngle = 0;
int FPath = 1;
int text = 1;

// Lights
int Light1On = 1;
int Light2On = 1;
int Light3On = 1;

//Materials
Mtls LunarMat; 

//Default Materials
float dissolve = 1.;
float SpecularExponant = 0;

// View variables
int loadMoon = 1;
int stars = 1;
int View = 1;

//Bezier curves
int numPoints = 25;

//point testing
float dotPosX = MoonDiameter / 2;
float dotPosY = 0;
float dotPosZ = 0;

//Shaders
GLSLProgram *FragmentLight;
GLSLProgram *EarthShadeModel;
GLSLProgram *MoonShadeModel;
GLSLProgram *LunarMask;

//Animation objects
Keyframe Test = Keyframe(5.);
Keyframe EarthWalk = Keyframe(27);
Keyframe Flight = Keyframe(30.);
Keyframe LMFirstStep = Keyframe(36.);

// Moon positioning for flightpath
float rotPos;
float MOrbit = (MoonDiameter / 2) + 100;
float EOrbit = (EarthDiameter / 2) + 150;
float XMove = -3400;
std::vector<std::vector<float> > pathPoints;

//Animation Parameters
float test[][7] = {
	{0, AstroXYZ[0], AstroXYZ[1], AstroXYZ[2] - 5, 0., 0., 0.},
	{150, AstroXYZ[0], AstroXYZ[1] + 5, AstroXYZ[2] - 5, 0., 0., 0.},
	{300, AstroXYZ[0], AstroXYZ[1] + 5, AstroXYZ[2], 0., 0., 0.},
	{450, AstroXYZ[0], AstroXYZ[1], AstroXYZ[2], 0., 0., 0.},
	{600, AstroXYZ[0], AstroXYZ[1], AstroXYZ[2] - 5, 0., 0., 0.}
};

float EyeEarth[][7] = {
	{0, LM_XYZ[0] + 5, LM_XYZ[1]+1.5, LM_XYZ[2], 0, 0, 0},
	{30, LM_XYZ[0] + 5, LM_XYZ[1] + 1.5, LM_XYZ[2],90, 12, 0},
	{60, LM_XYZ[0] + 5, LM_XYZ[1]+1.5, LM_XYZ[2], 160, 0, 0},
	{240, LM_XYZ[0] + 5, LM_XYZ[1] + 1.5, LM_XYZ[2], 160, 0, 0},
	{270, LM_XYZ[0] + 5, LM_XYZ[1] + 1.5, LM_XYZ[2],90, 12, 0},
	{300, LM_XYZ[0] + 5, LM_XYZ[1] + 1.5, LM_XYZ[2], 0, 0, 0},
	{315, LM_XYZ[0] + 6.25, LM_XYZ[1] + 1.6, LM_XYZ[2], 0, 0, 0},
	{330, LM_XYZ[0] + 7.5, LM_XYZ[1] + 1.5, LM_XYZ[2], 0, 0, 0},
	{430, LM_XYZ[0] + 7.5, LM_XYZ[1] + 1.5, LM_XYZ[2], 0, 0, 0},
	{445, LM_XYZ[0] + 8.75, LM_XYZ[1] + 1.6, LM_XYZ[2], 0, 0, 0},
	{460, LM_XYZ[0] + 10., LM_XYZ[1] + 1.5, LM_XYZ[2], 0, 0, 0},
	{560, LM_XYZ[0] + 10., LM_XYZ[1] + 1.5, LM_XYZ[2], 0, 0, 0},
	{575, LM_XYZ[0] + 11.25, LM_XYZ[1] + 1.6, LM_XYZ[2], 0, 0, 0},
	{590, LM_XYZ[0] + 12.5, LM_XYZ[1] + 1.5, LM_XYZ[2], 0, 0, 0},
	{690, LM_XYZ[0] + 12.5, LM_XYZ[1] + 1.5, LM_XYZ[2], 0, 0, 0},
	{705, LM_XYZ[0] + 13.75, LM_XYZ[1] + 1.6, LM_XYZ[2], 0, 0, 0},
	{720, LM_XYZ[0] + 15, LM_XYZ[1] + 1.5, LM_XYZ[2], 0, 0, 0},
	{750, LM_XYZ[0] + 15, LM_XYZ[1] + 1.5, LM_XYZ[2], 45, 0, 0},
	{850, LM_XYZ[0] + 15, LM_XYZ[1] + 1.5, LM_XYZ[2], 45, 0, 0},
	{880, LM_XYZ[0] + 15, LM_XYZ[1] + 1.5, LM_XYZ[2], 55, -25, 0},
	{980, LM_XYZ[0] + 15, LM_XYZ[1] + 1.5, LM_XYZ[2], 55, -25, 0},
	{1020, LM_XYZ[0] + 15, LM_XYZ[1] + 1.5, LM_XYZ[2], 35, -25, 0},
	{1120, LM_XYZ[0] + 15, LM_XYZ[1] + 1.5, LM_XYZ[2], 35, -25, 0},
	{1150, LM_XYZ[0] + 15, LM_XYZ[1] + 1.5, LM_XYZ[2], -20, 0, 0},
	{1250, LM_XYZ[0] + 15, LM_XYZ[1] + 1.5, LM_XYZ[2], -20, 0, 0},
	{1280, LM_XYZ[0] + 15, LM_XYZ[1] + 1.5, LM_XYZ[2], 0, 0, 0}
};

float SmallStepForMan[][7] = {
	{0, 20, 14, 13.5, 0, 0, 0},
	{600, 20, 14, 13.25, 0, 45, 0},
	{900, 20, 14, 13.25, 0, 45, 0},
	{1500, 20, 14, 13.25, 180, 0, 0},
	{1800, 20, 13.885, 13., 180, 0, 0},
	{2100, 20, 13.885, 13., 180, 0, 0},
	{2400, 20, 13.77, 12.9, 180, 0, 0},
	{2700, 20, 13.77, 12.9, 180, 0, 0},
	{3000, 20, 13.655, 12.8, 180, 0, 0},
	{3300, 20, 13.655, 12.8, 180, 0, 0},
	{3600, 20, 13.54, 12.7, 180, 0, 0},
	{3900, 20, 13.54, 12.7, 180, 0, 0},
	{4200, 20, 13.425, 12.6, 180, 0, 0},
	{4500, 20, 13.425, 12.6, 180, 0, 0},
	{4800, 20, 13.31, 12.5, 180, 0, 0},
	{5200, 20, 13.31, 12.5, 180, 0, 0},
	{5500, 20, 12.5, 12.5, 0, 0, 0},
	{5800, 20, 12.5, 12.0, 0, 0, 0},
	{5950, 20, 12.5, 12.0, 0, 0, 0},
	{6100, 20, 12.5, 11.5, 0, 0, 0},
	{6250, 20, 12.5, 11.5, 0., 0., 0.},
	{6350, 20, 12.5, 11.0, 20., 0., 0.},
	{6650, 20, 12.5, 11.0, 20., 0., 0.},
	{6850, 20, 12.5, 11.0, -20., 0., 0.},
	{7150, 20, 12.5, 11.0, -20., 0., 0.},
	{7250, 20, 12.5, 11.0, 0., 0., 0.},
	{9050, 20, 12.5, 11.0, 0., 0., 0.},
	{9150, 20, 12.5, 10.5, 0., 0., 0.},
	{9250, 20, 12.5, 10.5, 0., 0., 0.},
	{9350, 20, 12.5, 10.0, 0., 0., 0.},
	{9450, 20, 12.5, 10.0, 0., 0., 0.},
	{9550, 20, 12.5, 9.5, 0., 0., 0.},
	{9650, 20, 12.5, 9.5, 0., 0., 0.},
	{9750, 20, 12.5, 9.0, 0., 0., 0.},
	{9950, 20, 12.5, 9.0, 0., 0., 0.},
	{11000, 20, 12.5, 9.0, 180., 0., 0.},
	{13000, 20, 12.5, 9.0, 180., 0., 0.}
};

//LookAt Point
//{Starting X, Starting Y, Starting Z, Original X, Original Y, Original Z}
//Note that Starting XYZ are the parameters that will change as the eye position changes, the original
//position is to help with this process.
float LookAtAnimEarth[6] = {EarthXYZ[0],EarthXYZ[1],EarthXYZ[2],EarthXYZ[0],EarthXYZ[1],EarthXYZ[2]};
float LookAtAnimSSFM[6] = {LM_XYZ[0], 14, LM_XYZ[2]-7, LM_XYZ[0], 14, LM_XYZ[2]-7 };
float path[][7] = {

	//Launch and first orbit around Earth
	{0, LaunchXYZ[0] + XMove, LaunchXYZ[1], LaunchXYZ[2], 0., 0., 0.},
	{40, LaunchXYZ[0] - 250 + XMove, LaunchXYZ[1], LaunchXYZ[2] + 10, 0., 0., 0.},
	{60, 6800 - EOrbit + XMove, 0, 0, 0., 0., 0.},
	{80, 6800 + XMove, 0, EOrbit, 0., 0., 0.},
	{100, 6800 + EOrbit + XMove, 0, 0, 0., 0.},
	{120, 6800 + XMove, 0, -EOrbit, 0., 0., 0.},
	{140, 6800 - EOrbit + XMove, 0, 0 , 0, 0, 0},
	//Second orbit around Earth
	{160, 6800 + XMove, 0, EOrbit, 0., 0., 0.},
	{180, 6800 + EOrbit + XMove, 0, 0, 0., 0.},
	{200, 6800 + XMove, 0, -EOrbit, 0., 0., 0.},
	//To the Moon
	{235, MoonXYZ[0] + XMove + 250, MoonXYZ[1],  MoonXYZ[2] + MOrbit, 0., 0., 0.},
	//First Moon Orbit
	{260, MoonXYZ[0] - MOrbit + XMove, MoonXYZ[1], MoonXYZ[2], 0., 0., 0.},
	{280, MoonXYZ[0] + XMove, MoonXYZ[1],  MoonXYZ[2] - MOrbit, 0., 0., 0.},
	{300, MoonXYZ[0] + MOrbit + XMove, MoonXYZ[1], MoonXYZ[2], 0, 0., 0.},
	//Second Moon Orbit to landing
	{320, MoonXYZ[0] + XMove, MoonXYZ[1],  MoonXYZ[2] + MOrbit, 0., 0., 0.},
	{340, MoonXYZ[0] - MOrbit + XMove, MoonXYZ[1], MoonXYZ[2], 0., 0., 0.},
	{360, MoonXYZ[0] + XMove, MoonXYZ[1],  MoonXYZ[2] - MOrbit, 0., 0., 0.},
	{375, MoonXYZ[0] + BaseXYZ[0] + XMove, MoonXYZ[1] + BaseXYZ[1], MoonXYZ[2] + BaseXYZ[2], 0, 0, 0},
	{377, MoonXYZ[0] + BaseXYZ[0] + XMove, MoonXYZ[1] + BaseXYZ[1], MoonXYZ[2] + BaseXYZ[2], 0, 0, 0},
	{380, MoonXYZ[0] + BaseXYZ[0] + XMove, MoonXYZ[1] + BaseXYZ[1], MoonXYZ[2] + BaseXYZ[2], 0, 0, 0},
	//////////
	//Second half//
	//////////
	//Takeoff to first Moon orbit
	{440, MoonXYZ[0] + BaseXYZ[0] + XMove, MoonXYZ[1] + BaseXYZ[1], MoonXYZ[2] + BaseXYZ[2], 0, 0, 0},
	{460, MoonXYZ[0] + MOrbit * .7 + XMove, MoonXYZ[1] + 10, MoonXYZ[2], 0, 0, 0},
	{480, MoonXYZ[0] + XMove, MoonXYZ[1] + 10, MoonXYZ[2] + MOrbit * .8, 0, 0, 0},
	{500, MoonXYZ[0] - MOrbit * .8 + XMove, MoonXYZ[1] + 10, MoonXYZ[2], 0, 0, 0},
	//Second Moon orbit
	{520, MoonXYZ[0] + XMove + 100, MoonXYZ[1] + 10, MoonXYZ[2] - MOrbit * .8, 0, 0, 0},
	{540, MoonXYZ[0] + MOrbit + XMove - 10, MoonXYZ[1] + 10, MoonXYZ[2], 0, 0, 0},
	{560, MoonXYZ[0] + XMove, MoonXYZ[1] + 10, MoonXYZ[2] + MOrbit, 0, 0, 0},
	{580, MoonXYZ[0] - MOrbit + XMove, MoonXYZ[1] + 10, MoonXYZ[2], 0, 0, 0},
	{600, MoonXYZ[0] + XMove, MoonXYZ[1] + 10, MoonXYZ[2] - MOrbit, 0, 0, 0},
	{610, MoonXYZ[0] + 200 + XMove , MoonXYZ[1] + 10, MoonXYZ[2] - MOrbit + 100, 0, 0, 0},

	//Path to Earth, 'bot' of Moon to 'top' of Earth
	{675, 6800 + XMove, 0, EOrbit, 0, 0, 0},
	//Earth orbit to splashdown
	{690, 6800 + EOrbit * .7 + XMove, 10, 0, 0, 0, 0},
	{710, 6800 + EarthDiameter / 2 + XMove, 10, 0, 0, 0, 0},
	{715, 6800 + EarthDiameter / 2 + XMove, 10, 0, 0, 0, 0},
	{720, 6800 + EarthDiameter / 2 + XMove, 10, 0, 0, 0, 0},
	{-1}
};

//Test 
//To load in .obj
/////////////
/*
struct Vertex {
	float x, y, z;
};

struct Normal {
	float nx, ny, nz;
};

struct TextureCoord {
	float s, t, p;
};

struct face {
	int v, n, t;
};*/

void	Cross(float[3], float[3], float[3]);
char *	ReadRestOfLine(FILE *);
void	ReadObjVTN(char *, int *, int *, int *);
float	Unit(float[3]);
float	Unit(float[3], float[3]);
////////////

// multiplication factors for input interaction:
//  (these are known from previous experience)
const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };

// minimum allowable scale factor:
const float MINSCALE = { 0.01f };

// active mouse buttons (or them together):
const int LEFT = { 4 };
const int MIDDLE = { 2 };
const int RIGHT = { 1 };

// which projection:
enum Projections
{
	ORTHO,
	PERSP
};

// which button:
enum ButtonVals
{
	RESET,
	QUIT
};

// window background color (rgba):
const GLfloat BACKCOLOR[] = { 0., 0., 0., 1. };

// the color numbers:
// this order must match the radio button order
enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE,
	BLACK
};

char * ColorNames[] =
{
	"Red",
	"Yellow",
	"Green",
	"Cyan",
	"Blue",
	"Magenta",
	"White",
	"Black"
};


// the color definitions:
// this order must match the menu order

const GLfloat Colors[][3] =
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
	{ 1., 1., 1. },		// white
	{ 0., 0., 0. },		// black
};

// fog parameters:
const GLfloat FOGCOLOR[4] = { .0, .0, .0, 1. };
const GLenum  FOGMODE = { GL_LINEAR };
const GLfloat FOGDENSITY = { 0.30f };
const GLfloat FOGSTART = { 1.5 };
const GLfloat FOGEND = { 4. };


// non-constant global variables:
int		ActiveButton;			// current button that is down
GLuint	LandingSite;			// list to load in lunar surface obj
GLuint	LunarModule;			// list to load in lunar module obj
GLuint	SaturnV;				// list to load Saturn V rocket
GLuint	Astronaut;				// list to load Astronaut
GLuint	FlagList;				// list to load Flag
GLuint	FlagPoleList;				// list to load Flag
int		MainWindow;				// window id for main graphics window
float	Scale;					// scaling factor
int		WhichColor;				// index into Colors[ ]
int		WhichProjection;		// ORTHO or PERSP
int		Xmouse, Ymouse;			// mouse valuesl
float	Xrot, Yrot;				// rotation angles in degrees
bool	Zoom;

//bool	View3; //if this is true, lunar surface will be loaded in its actual position on the moon
float Time;

// function prototypes:
void	Display();
void	DoColorMenu(int);
void	DoMainMenu(int);
void	DoProjectMenu(int);
void	DoRasterString(float, float, float, char *);
void	DoStrokeString(float, float, float, float, char *);
float	ElapsedSeconds();
void	InitGraphics();
void	Animate();
void	InitLists();
void	InitMenus();
void	Keyboard(unsigned char, int, int);
void	MouseButton(int, int, int, int);
void	MouseMotion(int, int);
void	Reset();
void	Resize(int, int);
void	Visibility(int);
void	freeMem();
void	LookAtRot(float *,float *,float *);
void	MulMatrices(float [4][4], float [4][1]);

////////////////////////////////////////////////////////////
//Functions to load in .obj files
//Credit to Mike Bailey, Oregon State University
////////////////////////////////////////////////////////////

void freeMem() {
	delete FragmentLight;
	delete EarthShadeModel;
	delete MoonShadeModel;
	delete LunarMask;
}

int
LoadObjFileO(char *name)
{
	char *cmd;		// the command string
	char *str;		// argument string

	std::vector <struct Vertex> Vertices(10000);
	std::vector <struct Normal> Normals(10000);
	std::vector <struct TextureCoord> TextureCoords(10000);

	Vertices.clear();
	Normals.clear();
	TextureCoords.clear();

	struct Vertex sv;
	struct Normal sn;
	struct TextureCoord st;


	// open the input file:

	FILE *fp = fopen(name, "r");
	if (fp == NULL)
	{
		fprintf(stderr, "Cannot open .obj file '%s'\n", name);
		return 1;
	}


	float xmin = 1.e+37f;
	float ymin = 1.e+37f;
	float zmin = 1.e+37f;
	float xmax = -xmin;
	float ymax = -ymin;
	float zmax = -zmin;

	glBegin(GL_TRIANGLES);

	for (; ; )
	{
		char *line = ReadRestOfLine(fp);
		if (line == NULL)
			break;


		// skip this line if it is a comment:

		if (line[0] == '#')
			continue;


		// skip this line if it is something we don't feel like handling today:

		if (line[0] == 'g')
			continue;

		if (line[0] == 'm')
			continue;

		if (line[0] == 's')
			continue;

		if (line[0] == 'u')
			continue;


		// get the command string:

		cmd = strtok(line, OBJDELIMS);


		// skip this line if it is empty:

		if (cmd == NULL)
			continue;


		if (strcmp(cmd, "v") == 0)
		{
			str = strtok(NULL, OBJDELIMS);
			sv.x = atof(str);

			str = strtok(NULL, OBJDELIMS);
			sv.y = atof(str);

			str = strtok(NULL, OBJDELIMS);
			sv.z = atof(str);

			Vertices.push_back(sv);

			if (sv.x < xmin)	xmin = sv.x;
			if (sv.x > xmax)	xmax = sv.x;
			if (sv.y < ymin)	ymin = sv.y;
			if (sv.y > ymax)	ymax = sv.y;
			if (sv.z < zmin)	zmin = sv.z;
			if (sv.z > zmax)	zmax = sv.z;

			continue;
		}


		if (strcmp(cmd, "vn") == 0)
		{
			str = strtok(NULL, OBJDELIMS);
			sn.nx = atof(str);

			str = strtok(NULL, OBJDELIMS);
			sn.ny = atof(str);

			str = strtok(NULL, OBJDELIMS);
			sn.nz = atof(str);

			Normals.push_back(sn);

			continue;
		}


		if (strcmp(cmd, "vt") == 0)
		{
			st.s = st.t = st.p = 0.;

			str = strtok(NULL, OBJDELIMS);
			st.s = atof(str);

			str = strtok(NULL, OBJDELIMS);
			if (str != NULL)
				st.t = atof(str);

			str = strtok(NULL, OBJDELIMS);
			if (str != NULL)
				st.p = atof(str);

			TextureCoords.push_back(st);

			continue;
		}


		if (strcmp(cmd, "f") == 0)
		{
			struct face vertices[10];
			for (int i = 0; i < 10; i++)
			{
				vertices[i].v = 0;
				vertices[i].n = 0;
				vertices[i].t = 0;
			}

			int sizev = (int)Vertices.size();
			int sizen = (int)Normals.size();
			int sizet = (int)TextureCoords.size();

			int numVertices = 0;
			bool valid = true;
			int vtx = 0;
			char *str;
			while ((str = strtok(NULL, OBJDELIMS)) != NULL)
			{
				int v, n, t;
				ReadObjVTN(str, &v, &t, &n);

				// if v, n, or t are negative, they are wrt the end of their respective list:

				if (v < 0)
					v += (sizev + 1);

				if (n < 0)
					n += (sizen + 1);

				if (t < 0)
					t += (sizet + 1);


				// be sure we are not out-of-bounds (<vector> will abort):

				if (t > sizet)
				{
					if (t != 0)
						fprintf(stderr, "Read texture coord %d, but only have %d so far\n", t, sizet);
					t = 0;
				}

				if (n > sizen)
				{
					if (n != 0)
						fprintf(stderr, "Read normal %d, but only have %d so far\n", n, sizen);
					n = 0;
				}

				if (v > sizev)
				{
					if (v != 0)
						fprintf(stderr, "Read vertex coord %d, but only have %d so far\n", v, sizev);
					v = 0;
					valid = false;
				}

				vertices[vtx].v = v;
				vertices[vtx].n = n;
				vertices[vtx].t = t;
				vtx++;

				if (vtx >= 10)
					break;

				numVertices++;
			}


			// if vertices are invalid, don't draw anything this time:

			if (!valid)
				continue;

			if (numVertices < 3)
				continue;


			// list the vertices:

			int numTriangles = numVertices - 2;

			for (int it = 0; it < numTriangles; it++)
			{
				int vv[3];
				vv[0] = 0;
				vv[1] = it + 1;
				vv[2] = it + 2;

				// get the planar normal, in case vertex normals are not defined:

				struct Vertex *v0 = &Vertices[vertices[vv[0]].v - 1];
				struct Vertex *v1 = &Vertices[vertices[vv[1]].v - 1];
				struct Vertex *v2 = &Vertices[vertices[vv[2]].v - 1];

				float v01[3], v02[3], norm[3];
				v01[0] = v1->x - v0->x;
				v01[1] = v1->y - v0->y;
				v01[2] = v1->z - v0->z;
				v02[0] = v2->x - v0->x;
				v02[1] = v2->y - v0->y;
				v02[2] = v2->z - v0->z;
				Cross(v01, v02, norm);
				Unit(norm, norm);
				glNormal3fv(norm);

				for (int vtx = 0; vtx < 3; vtx++)
				{
					if (vertices[vv[vtx]].t != 0)
					{
						struct TextureCoord *tp = &TextureCoords[vertices[vv[vtx]].t - 1];
						glTexCoord2f(tp->s, tp->t);
					}

					if (vertices[vv[vtx]].n != 0)
					{
						struct Normal *np = &Normals[vertices[vv[vtx]].n - 1];
						glNormal3f(np->nx, np->ny, np->nz);
					}

					struct Vertex *vp = &Vertices[vertices[vv[vtx]].v - 1];
					glVertex3f(vp->x, vp->y, vp->z);
				}
			}
			continue;
		}


		if (strcmp(cmd, "s") == 0)
		{
			continue;
		}

	}

	glEnd();
	fclose(fp);

	fprintf(stderr, "Obj file range: [%8.3f,%8.3f,%8.3f] -> [%8.3f,%8.3f,%8.3f]\n",
		xmin, ymin, zmin, xmax, ymax, zmax);
	fprintf(stderr, "Obj file center = (%8.3f,%8.3f,%8.3f)\n",
		(xmin + xmax) / 2., (ymin + ymax) / 2., (zmin + zmax) / 2.);
	fprintf(stderr, "Obj file  span = (%8.3f,%8.3f,%8.3f)\n",
		xmax - xmin, ymax - ymin, zmax - zmin);

	return 0;
}


/*
void
Cross(float v1[3], float v2[3], float vout[3])
{
	float tmp[3];

	tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
	tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
	tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];

	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}



float
Unit(float v[3])
{
	float dist;

	dist = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];

	if (dist > 0.0)
	{
		dist = sqrt(dist);
		v[0] /= dist;
		v[1] /= dist;
		v[2] /= dist;
	}

	return dist;
}



float
Unit(float vin[3], float vout[3])
{
	float dist;

	dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];

	if (dist > 0.0)
	{
		dist = sqrt(dist);
		vout[0] = vin[0] / dist;
		vout[1] = vin[1] / dist;
		vout[2] = vin[2] / dist;
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}

	return dist;
}


char *
ReadRestOfLine(FILE *fp)
{
	static char *line;
	std::vector<char> tmp(1000);
	tmp.clear();

	for (; ; )
	{
		int c = getc(fp);

		if (c == EOF && tmp.size() == 0)
		{
			return NULL;
		}

		if (c == EOF || c == '\n')
		{
			delete[] line;
			line = new char[tmp.size() + 1];
			for (int i = 0; i < (int)tmp.size(); i++)
			{
				line[i] = tmp[i];
			}
			line[tmp.size()] = '\0';	// terminating null
			return line;
		}
		else
		{
			tmp.push_back(c);
		}
	}

	return "";
}


void
ReadObjVTN(char *str, int *v, int *t, int *n)
{
	// can be one of v, v//n, v/t, v/t/n:

	if (strstr(str, "//"))				// v//n
	{
		*t = 0;
		sscanf(str, "%d//%d", v, n);
		return;
	}
	else if (sscanf(str, "%d/%d/%d", v, t, n) == 3)	// v/t/n
	{
		return;
	}
	else
	{
		*n = 0;
		if (sscanf(str, "%d/%d", v, t) == 2)		// v/t
		{
			return;
		}
		else						// v
		{
			*n = *t = 0;
			sscanf(str, "%d", v);
		}
	}
}
*/
/////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

int
main(int argc, char *argv[])
{
	// turn on the glut package:
	// (do this before checking argc and argv since it might
	// pull some command line arguments out)
	glutInit(&argc, argv);

	// setup all the graphics stuff:
	InitGraphics();

	// create the display structures that will not change:
	InitLists();

	// init all the global variables used by Display( ):
	// this will also post a redisplay
	Reset();

	// setup all the user interface stuff:
	InitMenus();

	// draw the scene once and wait for some interaction:
	// (this will never return)
	glutSetWindow(MainWindow);
	glutMainLoop();

	// this is here to make the compiler happy:
	//Deallocate memory so no memory leaks happen.
	freeMem();
	return 0;
}

void
Animate()
{
	// put animation stuff in here -- change some global variables
	// for Display( ) to find
	int ms = glutGet(GLUT_ELAPSED_TIME);
	ms %= MS_IN_THE_ANIMATION_CYCLE;
	Time = (float)ms / (float)MS_IN_THE_ANIMATION_CYCLE;
	rAngle = Time * 360;
	rotPos = (((180 - rAngle) / 180) * .154);
	if (View == 1) {
		Flight.Update();
	}
	if (View == 7)
		currentFactor += TurnFactor;
	else
		currentFactor = 0.;
	// force a call to Display( ) next time it is convenient:
	if (View == 10)
	{
		/* Always set your points this way
		float SomeVariable[][7] = {
		{Frame #, X, Y, Z, AngleX, AngleY,AngleZ},
		...
		{Frame #, ...},
		{-1] Ending Frame
		};

		Also, set Keyframe object like so:
		Keyframe SomeVariableName = Keyframe(TotalTime);

		At the end of the program, free the mem of any allocated memory using new operator:
		void freeMem(){
			delete SomeVariableName;
		}
		*/
		//Call this whenever we need to update XYZ
		EarthWalk.Update();
		//printf("X: %f, Y: %f, Z: %f\n", EarthWalk.X, EarthWalk.Y, EarthWalk.Z);
	}
	if (View != 1) {
		Flight.Reset();
	}
	if (View != 10) {
		//This resets it back to original frame.
		EarthWalk.Reset();
	}
	if (View == 11) {
		LMFirstStep.Update();
	}
	if (View != 11) {
		LMFirstStep.Reset();
	}
	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

/*// utility to create an array from 3 separate values:
float *
Array3(float a, float b, float c)
{
	static float array[4];
	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = 0; //Sets all lights to infinity for sun. Need to change if we add more lights
	return array;
}*/

// utility to create an array from a multiplier and an array:
void MulMatrices(float A[4][4], float B[4][1]) {
	float C[4][1];
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 1; j++)
		{
			C[i][j] = 0.;
			for (int k = 0; k < 4; k++)
			{
				C[i][j] += A[i][k] * B[k][j];
				B[i][j] = C[i][j];
			}
		}
	}
}

float *
MulArray3(float factor, float array0[3])
{
	static float array[4];
	array[0] = factor * array0[0];
	array[1] = factor * array0[1];
	array[2] = factor * array0[2];
	array[3] = 1.;
	return array;
}

float White[] = { 1.,1.,1.,1. };

//Set material of object
void
SetMaterial(float amb, float diff, float spec, float r, float g, float b, float shininess)
{

	glMaterialfv(GL_BACK, GL_EMISSION, Array3(0., 0., 0.));
	glMaterialfv(GL_BACK, GL_AMBIENT, MulArray3(.4f, White));
	glMaterialfv(GL_BACK, GL_DIFFUSE, MulArray3(1., White));
	glMaterialfv(GL_BACK, GL_SPECULAR, Array3(0., 0., 0.));
	glMaterialf(GL_BACK, GL_SHININESS, 2.f);
	glMaterialfv(GL_FRONT, GL_EMISSION, Array3(0., 0., 0.));
	glMaterialfv(GL_FRONT, GL_AMBIENT, MulArray3(amb, Array3(r, g, b)));
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MulArray3(diff, Array3(r, g, b)));
	glMaterialfv(GL_FRONT, GL_SPECULAR, MulArray3(spec, White));
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

//Set up a point light
void
SetPointLight(int ilight, float x, float y, float z, float r, float g, float b)
{
	glLightfv(ilight, GL_POSITION, Array3(x, y, z));
	glLightfv(ilight, GL_AMBIENT, Array3(0., 0., 0.));
	glLightfv(ilight, GL_DIFFUSE, Array3(r, g, b));
	glLightfv(ilight, GL_SPECULAR, Array3(r, g, b));
	glLightf(ilight, GL_CONSTANT_ATTENUATION, 1.);
	glLightf(ilight, GL_LINEAR_ATTENUATION, 0.);
	glLightf(ilight, GL_QUADRATIC_ATTENUATION, 0.);
	glEnable(ilight);
}

struct point {
	float x, y, z;		// coordinates
	float nx, ny, nz;	// surface normal
	float s, t;		// texture coords
};

int	ReadInt(FILE *);
short	ReadShort(FILE *);

struct bmfh
{
	short bfType;
	int bfSize;
	short bfReserved1;
	short bfReserved2;
	int bfOffBits;
} FileHeader;

struct bmih
{
	int biSize;
	int biWidth;
	int biHeight;
	short biPlanes;
	short biBitCount;
	int biCompression;
	int biSizeImage;
	int biXPelsPerMeter;
	int biYPelsPerMeter;
	int biClrUsed;
	int biClrImportant;
} InfoHeader;

const int birgb = { 0 };

//Read a BMP file into a Texture:
unsigned char *
BmpToTexture(char *filename, int *width, int *height)
{

	int s, t, e;		// counters
	int numextra;		// # extra bytes each line in the file is padded with
	FILE *fp;
	unsigned char *texture;
	int nums, numt;
	unsigned char *tp;

	fp = fopen(filename, "rb");
	if (fp == NULL)
	{
		fprintf(stderr, "Cannot open Bmp file '%s'\n", filename);
		return NULL;
	}

	FileHeader.bfType = ReadShort(fp);

	// if bfType is not 0x4d42, the file is not a bmp:
	if (FileHeader.bfType != 0x4d42)
	{
		fprintf(stderr, "Wrong type of file: 0x%0x\n", FileHeader.bfType);
		fclose(fp);
		return NULL;
	}


	FileHeader.bfSize = ReadInt(fp);
	FileHeader.bfReserved1 = ReadShort(fp);
	FileHeader.bfReserved2 = ReadShort(fp);
	FileHeader.bfOffBits = ReadInt(fp);


	InfoHeader.biSize = ReadInt(fp);
	InfoHeader.biWidth = ReadInt(fp);
	InfoHeader.biHeight = ReadInt(fp);

	nums = InfoHeader.biWidth;
	numt = InfoHeader.biHeight;

	InfoHeader.biPlanes = ReadShort(fp);
	InfoHeader.biBitCount = ReadShort(fp);
	InfoHeader.biCompression = ReadInt(fp);
	InfoHeader.biSizeImage = ReadInt(fp);
	InfoHeader.biXPelsPerMeter = ReadInt(fp);
	InfoHeader.biYPelsPerMeter = ReadInt(fp);
	InfoHeader.biClrUsed = ReadInt(fp);
	InfoHeader.biClrImportant = ReadInt(fp);

	texture = new unsigned char[3 * nums * numt];
	if (texture == NULL)
	{
		fprintf(stderr, "Cannot allocate the texture array!\b");
		return NULL;
	}

	// extra padding bytes:
	numextra = 4 * (((3 * InfoHeader.biWidth) + 3) / 4) - 3 * InfoHeader.biWidth;

	// we do not support compression:
	if (InfoHeader.biCompression != birgb)
	{
		fprintf(stderr, "Wrong type of image compression: %d\n", InfoHeader.biCompression);
		fclose(fp);
		return NULL;
	}

	rewind(fp);
	fseek(fp, 14 + 40, SEEK_SET);

	if (InfoHeader.biBitCount == 24)
	{
		for (t = 0, tp = texture; t < numt; t++)
		{
			for (s = 0; s < nums; s++, tp += 3)
			{
				*(tp + 2) = fgetc(fp);		// b
				*(tp + 1) = fgetc(fp);		// g
				*(tp + 0) = fgetc(fp);		// r
			}

			for (e = 0; e < numextra; e++)
			{
				fgetc(fp);
			}
		}
	}

	fclose(fp);

	*width = nums;
	*height = numt;
	return texture;
}

int
ReadInt(FILE *fp)
{
	unsigned char b3, b2, b1, b0;
	b0 = fgetc(fp);
	b1 = fgetc(fp);
	b2 = fgetc(fp);
	b3 = fgetc(fp);
	return (b3 << 24) | (b2 << 16) | (b1 << 8) | b0;
}

short
ReadShort(FILE *fp)
{
	unsigned char b1, b0;
	b0 = fgetc(fp);
	b1 = fgetc(fp);
	return (b1 << 8) | b0;
}


static int		NumLngs, NumLats;
static struct point *	Pts;

inline struct point *
PtsPointer(int lat, int lng)
{
	if (lat < 0)	lat += (NumLats - 1);
	if (lng < 0)	lng += (NumLngs - 1);
	if (lat > NumLats - 1)	lat -= (NumLats - 1);
	if (lng > NumLngs - 1)	lng -= (NumLngs - 1);
	return &Pts[NumLngs*lat + lng];
}

inline void
DrawPoint(struct point *p)
{
	glNormal3f(p->nx, p->ny, p->nz);
	glTexCoord2f(p->s, p->t);
	glVertex3f(p->x, p->y, p->z);
}

void LookAtRot(float *Lx, float *Ly, float *Lz) {
	//We must first do two series of rotations, one horizontal, and one vertical. 
	float xyz[4][1] = {
		{*(Lx)},
		{*(Ly)},
		{*(Lz)},
		{1}
	};
	float Yrotate[4][4] = {
		{cos((EarthWalk.Ax*M_PI) / 180), 0, sin((EarthWalk.Ax*M_PI) / 180), 0 },
		{0, 1, 0, 0},
		{-sin((EarthWalk.Ax*M_PI) / 180), 0, cos((EarthWalk.Ax*M_PI) / 180)},
		{0, 0, 0, 1}
	};
	float Zrotate[4][4] = {
		{cos((EarthWalk.Az * M_PI) / 180), -sin((EarthWalk.Az * M_PI) / 180), 0, 0},
		{sin((EarthWalk.Az * M_PI) / 180), cos((EarthWalk.Az * M_PI) / 180), 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1}
	};
	//!!!!!!!!!IT IS IMPORTANT HOW YOU PASS IN THE MATRICES!!!!!!!!
	MulMatrices(Yrotate, xyz);
	//printf("xyz[0][0]: %f, xyz[1][0]: %f, xyz[2][0]: %f\n", xyz[0][0], xyz[1][0], xyz[2][0]);
	*(Lx) = xyz[0][0];
	*(Ly) = xyz[1][0];
	*(Lz) = xyz[2][0];
}

void
MjbSphere(float radius, int slices, int stacks)
{
	struct point top, bot;		// top, bottom points
	struct point *p;

	// set the globals:

	NumLngs = slices;
	NumLats = stacks;

	if (NumLngs < 3)
		NumLngs = 3;

	if (NumLats < 3)
		NumLats = 3;


	// allocate the point data structure:

	Pts = new struct point[NumLngs * NumLats];


	// fill the Pts structure:

	for (int ilat = 0; ilat < NumLats; ilat++)
	{
		float lat = -M_PI / 2. + M_PI * (float)ilat / (float)(NumLats - 1);
		float xz = cos(lat);
		float y = sin(lat);
		for (int ilng = 0; ilng < NumLngs; ilng++)
		{
			float lng = -M_PI + 2. * M_PI * (float)ilng / (float)(NumLngs - 1);
			float x = xz * cos(lng);
			float z = -xz * sin(lng);
			p = PtsPointer(ilat, ilng);
			p->x = radius * x;
			p->y = radius * y;
			p->z = radius * z;
			p->nx = x;
			p->ny = y;
			p->nz = z;

			p->s = (lng + M_PI) / (2.*M_PI);
			p->t = (lat + M_PI / 2.) / M_PI;

		}
	}

	top.x = 0.;		top.y = radius;	top.z = 0.;
	top.nx = 0.;		top.ny = 1.;		top.nz = 0.;
	top.s = 0.;		top.t = 1.;

	bot.x = 0.;		bot.y = -radius;	bot.z = 0.;
	bot.nx = 0.;		bot.ny = -1.;		bot.nz = 0.;
	bot.s = 0.;		bot.t = 0.;


	// connect the north pole to the latitude NumLats-2:

	glBegin(GL_QUADS);
	for (int ilng = 0; ilng < NumLngs - 1; ilng++)
	{
		p = PtsPointer(NumLats - 1, ilng);
		DrawPoint(p);

		p = PtsPointer(NumLats - 2, ilng);
		DrawPoint(p);

		p = PtsPointer(NumLats - 2, ilng + 1);
		DrawPoint(p);

		p = PtsPointer(NumLats - 1, ilng + 1);
		DrawPoint(p);
	}
	glEnd();

	// connect the south pole to the latitude 1:

	glBegin(GL_QUADS);
	for (int ilng = 0; ilng < NumLngs - 1; ilng++)
	{
		p = PtsPointer(0, ilng);
		DrawPoint(p);

		p = PtsPointer(0, ilng + 1);
		DrawPoint(p);

		p = PtsPointer(1, ilng + 1);
		DrawPoint(p);

		p = PtsPointer(1, ilng);
		DrawPoint(p);
	}
	glEnd();


	// connect the other 4-sided polygons:

	glBegin(GL_QUADS);
	for (int ilat = 2; ilat < NumLats - 1; ilat++)
	{
		for (int ilng = 0; ilng < NumLngs - 1; ilng++)
		{
			p = PtsPointer(ilat - 1, ilng);
			DrawPoint(p);

			p = PtsPointer(ilat - 1, ilng + 1);
			DrawPoint(p);

			p = PtsPointer(ilat, ilng + 1);
			DrawPoint(p);

			p = PtsPointer(ilat, ilng);
			DrawPoint(p);
		}
	}
	glEnd();

	delete[] Pts;
	Pts = NULL;
}


struct Point
{
	double x, y, z;
};

struct Curve
{
	float r, g, b;
	Point p0, p1, p2, p3;
};

void DrawCurve()
{
	//Jonathan Ropp's
	glLineWidth(3.);

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < pathPoints.size(); i++) {
		glColor3f(1, 1, 1);
		glVertex3f(pathPoints[i][0], pathPoints[i][1], pathPoints[i][2]);
	}
	glEnd();
	glLineWidth(1);
}

/*
void DrawCurve(struct Curve *curve)
	{
	//Mike Bailey's
	glLineWidth(3.);
	glColor3f(curve->r, curve->g, curve->b);

	struct Point p0 = curve->p0;
	struct Point p1 = curve->p1;
	struct Point p2 = curve->p2;
	struct Point p3 = curve->p3;

	glBegin(GL_LINE_STRIP);
	for (int it = 0; it <= numPoints; it++)
	{
		float t = (float)it / (float)numPoints;
		float omt = 1.f - t;
		float x = omt * omt*omt*p0.x + 3.f*t*omt*omt*p1.x + 3.f*t*t*omt*p2.x + t * t*t*p3.x;
		float y = omt * omt*omt*p0.y + 3.f*t*omt*omt*p1.y + 3.f*t*t*omt*p2.y + t * t*t*p3.y;
		float z = omt * omt*omt*p0.z + 3.f*t*omt*omt*p1.z + 3.f*t*t*omt*p2.z + t * t*t*p3.z;
		glVertex3f(x, y, z);
	}
	glEnd();
	glLineWidth(1);

	glColor3f(1, 1, 1);

	// Draw control points
	/*glPointSize(5.);
	glBegin(GL_POINTS);
	glVertex3f(p0.x, p0.y, p0.z);
	glVertex3f(p1.x, p1.y, p1.z);
	glVertex3f(p2.x, p2.y, p2.z);
	glVertex3f(p3.x, p3.y, p3.z);
	glEnd();

}*/

void storePoints(struct Curve *curve) {
	struct Point p0 = curve->p0;
	struct Point p1 = curve->p1;
	struct Point p2 = curve->p2;
	struct Point p3 = curve->p3;

	for (int it = 0; it <= numPoints; it++)
	{
		float t = (float)it / (float)numPoints;
		float omt = 1.f - t;
		float x = omt * omt*omt*p0.x + 3.f*t*omt*omt*p1.x + 3.f*t*t*omt*p2.x + t * t*t*p3.x;
		float y = omt * omt*omt*p0.y + 3.f*t*omt*omt*p1.y + 3.f*t*t*omt*p2.y + t * t*t*p3.y;
		float z = omt * omt*omt*p0.z + 3.f*t*omt*omt*p1.z + 3.f*t*t*omt*p2.z + t * t*t*p3.z;
		pathPoints.push_back({ x, y, z });
		it++; //We dont need a lot of accuracy
	}
}

// draw the complete scene:
void
Display()
{
	// set which window we want to do the graphics into:
	glutSetWindow(MainWindow);

	// erase the background:
	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	// specify shading to be flat:
	glShadeModel(GL_SMOOTH);

	// set the viewport to a square centered in the window:
	GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
	GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = (vx - v) / 2;
	GLint yb = (vy - v) / 2;
	glViewport(xl, yb, v, v);

	//Perspective	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90., 1., 0.1, 16000.);

	//Views
	// Sun Light
	glPushMatrix();
	SetPointLight(GL_LIGHT1, SunXYZ[0], 5, SunXYZ[2], 1, 1., 1.);
	glPopMatrix();
	float EyeEarthPosX, EyeEarthPosY, EyeEarthPosZ, LookAtX, LookAtY, LookAtZ, UpVecX, UpVecY, UpVecZ;

	//Flight path
	if (View == 1) {
		//Default view of flight path
		EyeEarthPosX = 0; EyeEarthPosY = 5500; EyeEarthPosZ = 0;
		LookAtX = 0; LookAtY = 0; LookAtZ = 0;
		UpVecX = 0; UpVecY = 0; UpVecZ = 1;
	}

	//View from Earth
	if (View == 2) {
		EyeEarthPosX = EarthXYZ[0] - (EarthDiameter / 2) + 650;
		EyeEarthPosY = EarthXYZ[1] - (EarthDiameter / 2) - 50;
		EyeEarthPosZ = EarthXYZ[2] - (EarthDiameter / 2) - 50;
		LookAtX = BaseXYZ[0]; LookAtY = BaseXYZ[1]; LookAtZ = BaseXYZ[2];
		UpVecX = 0; UpVecY = 0; UpVecZ = 1;
	}

	//View from above moon base
	if (View == 3) {
		EyeEarthPosX = 250; EyeEarthPosY = 0; EyeEarthPosZ = -250;
		LookAtX = 0; LookAtY = 0; LookAtZ = 0;
		UpVecX = 0; UpVecY = 1; UpVecZ = 0;
		if (Zoom) { //if this is the animation
			EyeEarthPosX = 250; EyeEarthPosY = 0; EyeEarthPosZ = -250 / (2 * Time);
			LookAtX = 0; LookAtY = 0; LookAtZ = 0;
			UpVecX = 0; UpVecY = 1; UpVecZ = 0;
		}

		else {
			EyeEarthPosX = 275; EyeEarthPosY = 0; EyeEarthPosZ = -150;
			LookAtX = 0; LookAtY = 0; LookAtZ = 0;
			UpVecX = 0; UpVecY = 1; UpVecZ = 0;
		}
	}

	//viewpoint from lunar module
	if (View == 4) {
		EyeEarthPosX = LM_XYZ[0]; EyeEarthPosY = LM_XYZ[1] + 5; EyeEarthPosZ = LM_XYZ[2] + 2;
		LookAtX = EarthXYZ[0]; LookAtY = EarthXYZ[1]; LookAtZ = EarthXYZ[2];
		UpVecX = 0;	UpVecY = 1;	UpVecZ = 0;
	}

	//Other view of landing site
	if (View == 5) {
		EyeEarthPosX = 20; EyeEarthPosY = 20; EyeEarthPosZ = -10;
		LookAtX = LM_XYZ[0]; LookAtY = LM_XYZ[1]; LookAtZ = LM_XYZ[2];
		UpVecX = 0; UpVecY = 1; UpVecZ = 0;
	}

	//View point Lander/Neil
	if (View == 6) {
		EyeEarthPosX = LM_XYZ[0] + 2; EyeEarthPosY = LM_XYZ[1] + 3; EyeEarthPosZ = LM_XYZ[2] - 7;
		LookAtX = FlagXYZ[0] + 3; LookAtY = FlagXYZ[1]; LookAtZ = FlagXYZ[2];
		UpVecX = 0; UpVecY = 1; UpVecZ = 0;
	}

	//Pan of lunar landscape
	if (View == 7)
	{
		EyeEarthPosX = LM_Animate[0]; EyeEarthPosY = LM_Animate[1]; EyeEarthPosZ = LM_Animate[2];
		LookAtX = (EarthXYZ[0] * cos(currentFactor) - EarthXYZ[2] * sin(currentFactor)); LookAtY = EarthXYZ[1]; LookAtZ = (EarthXYZ[0] * sin(currentFactor) + EarthXYZ[2] * cos(currentFactor));
		UpVecX = 0; UpVecY = 1; UpVecZ = 0;
	}

	//View of lunar module landing
	if (View == 8) {
		EyeEarthPosX = 20; EyeEarthPosY = 13; EyeEarthPosZ = 15;
		LookAtX = 7; LookAtY = 250; LookAtZ = 0;
		UpVecX = 0; UpVecY = 1; UpVecZ = 0;
	}

	//alt view of module landing
	if (View == 9) {
		EyeEarthPosX = 0; EyeEarthPosY = 50; EyeEarthPosZ = 10;
		LookAtX = 0; LookAtY = 0; LookAtZ = 0;
		UpVecX = 0; UpVecY = 1; UpVecZ = 0;
	}

	//alt view of module landing
	if (View == 0) {
		EyeEarthPosX = 0; EyeEarthPosY = 15; EyeEarthPosZ = -10;
		LookAtX = 40; LookAtY = 35; LookAtZ = 30;
		UpVecX = 0; UpVecY = 1; UpVecZ = 0;
	}
	if (View == 10) {
		LookAtAnimEarth[0] = LookAtAnimEarth[3] + (EarthWalk.X - EarthWalk.Frames[0].x);
		LookAtAnimEarth[1] = LookAtAnimEarth[4] + ((EarthWalk.Y - EarthWalk.Frames[0].y));
		LookAtAnimEarth[2] = LookAtAnimEarth[5] + ((EarthWalk.Z - EarthWalk.Frames[0].z));
		glRotatef(EarthWalk.Ax, 0, 1., 0);
		glRotatef(EarthWalk.Ay, 1, 0, 0);
		EyeEarthPosX = EarthWalk.X; EyeEarthPosY = EarthWalk.Y; EyeEarthPosZ = EarthWalk.Z;
		LookAtX = LookAtAnimEarth[0]; LookAtY = LookAtAnimEarth[1]; LookAtZ = LookAtAnimEarth[2];
		UpVecX = 0; UpVecY = 1; UpVecZ = 0;

	}
	if (View == 11) {
		LookAtAnimSSFM[0] = LookAtAnimSSFM[3] + (LMFirstStep.X - LMFirstStep.Frames[0].x);
		LookAtAnimSSFM[1] = LookAtAnimSSFM[4] + ((LMFirstStep.Y - LMFirstStep.Frames[0].y));
		LookAtAnimSSFM[2] = LookAtAnimSSFM[5] + ((LMFirstStep.Z - LMFirstStep.Frames[0].z));
		glRotatef(LMFirstStep.Ax, 0, 1., 0);
		glRotatef(LMFirstStep.Ay, 1, 0, 0);
		EyeEarthPosX = LMFirstStep.X; EyeEarthPosY = LMFirstStep.Y; EyeEarthPosZ = LMFirstStep.Z;
		LookAtX = LookAtAnimSSFM[0]; LookAtY = LookAtAnimSSFM[1]; LookAtZ = LookAtAnimSSFM[2];
		UpVecX = 0; UpVecY = 1; UpVecZ = 0;
	}
	// set the EyeEarth position, look-at position, and up-vector:
	gluLookAt(EyeEarthPosX, EyeEarthPosY, EyeEarthPosZ, LookAtX, LookAtY, LookAtZ, UpVecX, UpVecY, UpVecZ);

	// rotate the scene based on each view:
	if (View == 1) {
		glRotatef((GLfloat)Yrot, 0., 0., 1.);
		glRotatef((GLfloat)Xrot, 1., 0., 0.);
	}
	if (View == 2) {
		glRotatef((GLfloat)Yrot, 0., 0., 1.);
		glRotatef((GLfloat)Xrot, 0., 1., 0.);
	}
	else {
		glRotatef((GLfloat)Yrot, 0., 1., 0.);
		glRotatef((GLfloat)Xrot, 1., 0., 0.);
	}

	// uniformly scale the scene:
	if (Scale < MINSCALE)
		Scale = MINSCALE;
	glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (Light1On) {
		glEnable(GL_LIGHT1);
	}
	else {
		glDisable(GL_LIGHT1);
	}
	if (Light2On) {
		glEnable(GL_LIGHT2);
	}
	else {
		glDisable(GL_LIGHT2);
	}
	if (Light3On) {
		glEnable(GL_LIGHT3);
	}
	else {
		glDisable(GL_LIGHT3);
	}

	glEnable(GL_LIGHTING);

	////////////////////////////////////////////
	//Set the scene//
	// Positions found at begining of file (~line 46)
	////////////////////////////////////////////

	// Load in lunar surface 
	// (Original model scale is 30X30 Kilometers - https://nasa3d.arc.nasa.gov/detail/Apollo11-Landing)
	if (loadMoon != 1) {
		glPushMatrix();
		glRotatef(-90., 1., 0., 0.);
		FragmentLight->Use();
		FragmentLight->SetUniformVariable("uLightX", LightXYZ[0]);
		FragmentLight->SetUniformVariable("uLightY", LightXYZ[1]);
		FragmentLight->SetUniformVariable("uLightZ", LightXYZ[2]);
		FragmentLight->SetUniformVariable("uLunarX", MoonXYZ[0]);
		FragmentLight->SetUniformVariable("uLunarY", MoonXYZ[1]);
		FragmentLight->SetUniformVariable("uLunarZ", MoonXYZ[2]);
		FragmentLight->SetUniformVariable("AmbientR", LunarMat.First->Ka[0]);
		FragmentLight->SetUniformVariable("AmbientG", LunarMat.First->Ka[1]);
		FragmentLight->SetUniformVariable("AmbientB", LunarMat.First->Ka[2]);
		FragmentLight->SetUniformVariable("DiffuseR", LunarMat.First->Kd[0]);
		FragmentLight->SetUniformVariable("DiffuseG", LunarMat.First->Kd[1]);
		FragmentLight->SetUniformVariable("DiffuseB", LunarMat.First->Kd[2]);
		FragmentLight->SetUniformVariable("SpecularR", LunarMat.First->Ks[0]);
		FragmentLight->SetUniformVariable("SpecularG", LunarMat.First->Ks[1]);
		FragmentLight->SetUniformVariable("SpecularB", LunarMat.First->Ks[2]);
		FragmentLight->SetUniformVariable("dissolve", dissolve);
		FragmentLight->SetUniformVariable("specExp", SpecularExponant);
		glCallList(LandingSite);
		FragmentLight->Use(0);
		glPopMatrix();
	}


	//Current moon hotkey - 'M'
	if (loadMoon == 1) {
		// Load in the Moon, center if needed
		glPushMatrix();
		if (View == 1) {
			glTranslatef(-3400, 0, 0);
		}
		else {
			glTranslatef(MoonXYZ[0], MoonXYZ[1], MoonXYZ[2]);
		}
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TexM);
		MoonShadeModel->SetUniformVariable("uImageOne", 0);
		MoonShadeModel->SetUniformVariable("uLightX", (float)-1800);
		MoonShadeModel->SetUniformVariable("uLightY", (float)0);
		MoonShadeModel->SetUniformVariable("uLightZ", (float)-4000);
		MoonShadeModel->SetUniformVariable("uModelX", MoonXYZ[0]);
		MoonShadeModel->SetUniformVariable("uModelY", MoonXYZ[1]);
		MoonShadeModel->SetUniformVariable("uModelZ", MoonXYZ[2]);
		MoonShadeModel->SetUniformVariable("uTol", (float) 0.18);
		MoonShadeModel->SetUniformVariable("uDb", (float)1);
		MoonShadeModel->SetUniformVariable("uDc", (float)1);
		MoonShadeModel->SetUniformVariable("uDs", (float)1);
		MoonShadeModel->SetUniformVariable("uNb", (float).15);
		MoonShadeModel->SetUniformVariable("uNc", (float)1);
		MoonShadeModel->SetUniformVariable("uNs", (float)1);
		MjbSphere(MoonDiameter / 2, 100, 100);
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	}


	//load the landing site on top of moon so people can see it when they zoom in
	if (View == 3) {
		glPushMatrix();
		float Alp = .36;
		float Rad = .1;

		FragmentLight->Use();
		FragmentLight->SetUniformVariable("uLightX", 1800);
		FragmentLight->SetUniformVariable("uLightY", 100);
		FragmentLight->SetUniformVariable("uLightZ", -1100);
		FragmentLight->SetUniformVariable("uLunarX", 1765);
		FragmentLight->SetUniformVariable("uLunarY", 20);
		FragmentLight->SetUniformVariable("uLunarZ", -1065);
		FragmentLight->SetUniformVariable("AmbientR", LunarMat.First->Ka[0]);
		FragmentLight->SetUniformVariable("AmbientG", LunarMat.First->Ka[1]);
		FragmentLight->SetUniformVariable("AmbientB", LunarMat.First->Ka[2]);
		FragmentLight->SetUniformVariable("DiffuseR", LunarMat.First->Kd[0]);
		FragmentLight->SetUniformVariable("DiffuseG", LunarMat.First->Kd[1]);
		FragmentLight->SetUniformVariable("DiffuseB", LunarMat.First->Kd[2]);
		FragmentLight->SetUniformVariable("SpecularR", LunarMat.First->Ks[0]);
		FragmentLight->SetUniformVariable("SpecularG", LunarMat.First->Ks[1]);
		FragmentLight->SetUniformVariable("SpecularB", LunarMat.First->Ks[2]);
		FragmentLight->SetUniformVariable("dissolve", dissolve);
		FragmentLight->SetUniformVariable("specExp", SpecularExponant);

		LunarMask->Use();

		LunarMask->SetUniformVariable("uAlpha", Alp);
		//int loc = LunarMask->GetUniformLocation("uAlpha");
		//printf("******* %d",loc);
		LunarMask->SetUniformVariable("uRadius", Rad);
		//int loc2 = LunarMask->GetUniformLocation("uRadius");
		//printf("****** %d",loc2);

		glScalef(.1, .1, .1);

		glTranslatef(1800., 0., -925.);
		glRotatef(-60., 0., 1., 0.);
		glRotatef(180., 1., 0., 0.);


		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glCallList(LandingSite);

		glDisable(GL_BLEND);
		//FragmentLight->Use(0);
		LunarMask->Use(0);
		glPopMatrix();


	}
	glDisable(GL_BLEND);


	// Load in lunar module
	// (Real Lunar lander is about 31 ft wide and 23 ft tall - http://georgetyson.com/files/apollostatistics.pdf Page 17)
	if (View != 1) {
		glPushMatrix();
		SetMaterial(.4, .7, .8, 1, 1, 1, 4);
		if (View == 8) {
			glTranslatef(20 / (Time), 15 / Time, 15);
			glScalef(.0025, .0025, .0025);
		}
		else if (View == 9) {
			glTranslatef(3 / Time, 15 / Time, 15.);
			glScalef(.001, .001, .001);
		}
		else if (View == 0) {
			glTranslatef(8 / (Time * 2), 13 / Time, 15.);
			glScalef(.001, .001, .001);
		}
		else {
			glTranslatef(LM_XYZ[0], LM_XYZ[1], LM_XYZ[2]);
			glScalef(.0015, .0015, .0015);

		}
		glRotatef(180, 0, 1, 0);
		glColor3f(1., 1., 1.);
		glCallList(LunarModule);
		glPopMatrix();
	}

	// Load in Flag unless module is landing
	// Real dimensions: 1 inch poles, 3 X 5 foot flag	
	if (View != 0 && View != 1 && View != 9) {
		glPushMatrix();
		glColor3f(1, 1, 1);
		glTranslatef(FlagXYZ[0], FlagXYZ[1] + .5, FlagXYZ[2]);
		glRotatef(-90, 1, 0, 0);
		glScalef(.30, .30, .30);
		glRotatef(-20, 0, 0, 1);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, AFlag);
		glCallList(FlagList);
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();

		glPushMatrix();
		glColor3f(1, 1, 1);
		glTranslatef(FlagXYZ[0], FlagXYZ[1] + .5, FlagXYZ[2]);
		glRotatef(-90, 1, 0, 0);
		glScalef(.30, .30, .30);
		glRotatef(-20, 0, 0, 1);
		glCallList(FlagPoleList);
		glPopMatrix();

	}

	// Load in Astronaut unless module is landing
	// Model loads in about 10 ft, assuming space suit height is ~7 ft

	if (View != 0 && View != 1 && View != 9) {
		glPushMatrix();
		SetMaterial(.4, .7, .8, 1, 1, 1, 4);
		glTranslatef(AstroXYZ[0], AstroXYZ[1], AstroXYZ[2]);
		glScalef(.7, .7, .7);
		glRotatef(180, 0, 1, 0);
		glColor3f(1., 1., 1.);
		glCallList(Astronaut);
		glPopMatrix();
	}

	//Load the Star Map
	if (stars == 1) {
		glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, TexSt);
		MjbSphere(9000., 200, 200);
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	}

	// Load in the Earth
	glPushMatrix();
	glTranslatef(EarthXYZ[0], EarthXYZ[1], EarthXYZ[2]);
	if (View == 1) {
		glTranslatef(-3400, 0, 0);
		glRotatef(-90, 1, 0, 0);
	}
	glRotatef(90., 1., 0., 0.);
	glRotatef(180., 0., 1., 0.);
	EarthShadeModel->Use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, DayEarthTex);
	EarthShadeModel->SetUniformVariable("uImageOne", 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, NightEarthTex);
	EarthShadeModel->SetUniformVariable("uImageTwo", 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, CloudEarthTex);
	EarthShadeModel->SetUniformVariable("uImageThree", 2);
	glActiveTexture(GL_TEXTURE0);
	EarthShadeModel->SetUniformVariable("uLightX", (float)10000);
	EarthShadeModel->SetUniformVariable("uLightY", (float)-4000);
	EarthShadeModel->SetUniformVariable("uLightZ", (float)8000);
	EarthShadeModel->SetUniformVariable("uModelX", EarthXYZ[0]);
	EarthShadeModel->SetUniformVariable("uModelY", EarthXYZ[1]);
	EarthShadeModel->SetUniformVariable("uModelZ", EarthXYZ[2]);
	EarthShadeModel->SetUniformVariable("uTol", (float) 0.18);
	EarthShadeModel->SetUniformVariable("uDb", (float)2);
	EarthShadeModel->SetUniformVariable("uDc", (float)1);
	EarthShadeModel->SetUniformVariable("uDs", (float)1);
	EarthShadeModel->SetUniformVariable("uNb", (float)1);
	EarthShadeModel->SetUniformVariable("uNc", (float)1);
	EarthShadeModel->SetUniformVariable("uNs", (float)1);
	EarthShadeModel->SetUniformVariable("uDCloud", (float) 0.6);
	EarthShadeModel->SetUniformVariable("uNCloud", (float) .075);
	MjbSphere(EarthDiameter / 2, 100, 100);
	EarthShadeModel->Use(0);
	glPopMatrix();

	//Objects before this will be lit
	glDisable(GL_LIGHTING);
	//Objects past this will not be lit

	// Flight Path
	// https://airandspace.si.edu/sites/default/files/images/5317h.jpg
	//	Launch: 00:00
		//	Moon at +15.4 degrees
	//	Leave earth orbit: 1:35
	//	Enter moon orbit: 63:23
		//	Moon around +5.66 degrees
	//	Landing: 70:37
	//	Liftoff: 105:19
	//	Exit moon orbit: 112:12
		//	Moon around -1.866 degrees
	//	about earth orbit: 200:16
	//	Splashdown: 200:41
		//	Moon at -15.4 degrees

	if (View > 3) {
		FPath = 0;
	}
	//flying sphere
	if (View == 1 && FPath == 0) {
		glPushMatrix();
		glTranslatef(Flight.X, Flight.Y, Flight.Z);
		glColor3f(.75, .75, .75);
		MjbSphere(40, 50, 50);
		glPopMatrix();
	}
	if (View == 1 && FPath == 1) {

		struct Curve curve;
		curve.r = .8;
		curve.g = .8;
		curve.b = .8;

		//Earth
		//Launch to orbit 'right'
		struct Point p0 = { LaunchXYZ[0] + XMove, LaunchXYZ[1], LaunchXYZ[2] };
		struct Point p1 = { 6800 - 250 + XMove, EOrbit / 2 - 100, -EOrbit / 2 - 275 };
		struct Point p2 = { 6800 - EOrbit / 2 - 450 + XMove, EOrbit / 2 - 100, -600 };
		struct Point p3 = { 6800 - EOrbit + XMove, 100, 0 };
		curve.p0 = p0;
		curve.p1 = p1;
		curve.p2 = p2;
		curve.p3 = p3;
		//DrawCurve(&curve);
		storePoints(&curve);

		//1st orbit
		//'right' to 'top'
		curve.p0 = { 6800 - EOrbit + XMove, 100, 0 };
		curve.p1 = { 6800 - EOrbit + XMove, 0, EOrbit / 2 + 150 };
		curve.p2 = { 6800 - EOrbit / 2 + 150 + XMove, 0, EOrbit };
		curve.p3 = { 6800 + XMove, 0, EOrbit };
		//DrawCurve(&curve);
		storePoints(&curve);
		//'top' to 'left'
		curve.p0 = { 6800 + XMove, 0, EOrbit };
		curve.p1 = { 6800 + EOrbit / 2 + 150 + XMove, 0, EOrbit };
		curve.p2 = { 6800 + EOrbit + XMove, 0, EOrbit / 2 + 150 };
		curve.p3 = { 6800 + EOrbit + 75 + XMove, 0, 0 };
		//DrawCurve(&curve);
		storePoints(&curve);
		//'left' to 'bot'
		curve.p0 = { 6800 + EOrbit + 75 + XMove, 0, 0 };
		curve.p1 = { 6800 + EOrbit + XMove, 0, -EOrbit / 2 - 150 };
		curve.p2 = { 6800 + EOrbit / 2 + 150 + XMove, 0, -EOrbit };
		curve.p3 = { 6800 + XMove, 0, -EOrbit - 50 };
		//DrawCurve(&curve);
		storePoints(&curve);
		//'bot' to 'right'
		curve.p0 = { 6800 + XMove, 0, -EOrbit - 50 };
		curve.p1 = { 6800 - EOrbit / 2 - 150 + XMove, 0, -EOrbit };
		curve.p2 = { 6800 - EOrbit + XMove, 0, -EOrbit / 2 - 150 };
		curve.p3 = { 6800 - EOrbit + XMove, 0, 0 };
		//DrawCurve(&curve);
		storePoints(&curve);

		//2nd orbit
		//'right' to 'top'
		curve.p0 = { 6800 - EOrbit + XMove, 0, 0 };
		curve.p1 = { 6800 - EOrbit + XMove, 0, EOrbit / 2 + 150 };
		curve.p2 = { 6800 - EOrbit / 2 + 150 + XMove, 0, EOrbit };
		curve.p3 = { 6800 + XMove, 0, EOrbit };
		//DrawCurve(&curve);
		storePoints(&curve);
		//'top' to 'left'
		curve.p0 = { 6800 + XMove, 0, EOrbit };
		curve.p1 = { 6800 + EOrbit / 2 + 150 + XMove, 0, EOrbit };
		curve.p2 = { 6800 + EOrbit + XMove, 0, EOrbit / 2 + 150 };
		curve.p3 = { 6800 + EOrbit + 75 + XMove, 0, 0 };
		//DrawCurve(&curve);
		storePoints(&curve);
		//'left' to 'bot'
		curve.p0 = { 6800 + EOrbit + 75 + XMove, 0, 0 };
		curve.p1 = { 6800 + EOrbit + XMove, 0, -EOrbit / 2 - 150 };
		curve.p2 = { 6800 + EOrbit / 2 + 150 + XMove, 0, -EOrbit };
		curve.p3 = { 6800 + XMove, 0, -EOrbit - 50 };
		//DrawCurve(&curve);
		storePoints(&curve);

		//Not dependent on objects!!!
		//Travel to Moon
		//'bot' of Earth to 'top' of Moon
		curve.p0 = { 6800 + XMove, 0, -EOrbit - 50 };
		curve.p1 = { 6800 - EOrbit / 2 + 250 + XMove, 0, -EOrbit - 150 };
		curve.p2 = { 10.25 + MOrbit + XMove, 0, 362.05 + MOrbit };
		curve.p3 = { MoonXYZ[0] + XMove, MoonXYZ[0],  MoonXYZ[3] + MOrbit };
		//DrawCurve(&curve);
		storePoints(&curve);

		//Moon 1st orbit
		//'top' to 'right'
		curve.p0 = { MoonXYZ[0] + XMove, MoonXYZ[1],  MoonXYZ[2] + MOrbit };
		curve.p1 = { MoonXYZ[0] - MOrbit / 1.25 + 100 + XMove, MoonXYZ[1], MoonXYZ[2] + MOrbit };
		curve.p2 = { MoonXYZ[0] - MOrbit + XMove, MoonXYZ[1], MoonXYZ[2] + MOrbit / 1.25 - 100 };
		curve.p3 = { MoonXYZ[0] - MOrbit + XMove, MoonXYZ[1], MoonXYZ[2] };
		//DrawCurve(&curve);
		storePoints(&curve);
		//'right' to 'bot'
		curve.p0 = { MoonXYZ[0] - MOrbit + XMove, MoonXYZ[1], MoonXYZ[2] };
		curve.p1 = { MoonXYZ[0] - MOrbit + XMove, MoonXYZ[1], MoonXYZ[2] - MOrbit / 1.25 + 100 };
		curve.p2 = { MoonXYZ[0] - MOrbit / 1.25 + 100 + XMove, MoonXYZ[1], MoonXYZ[2] - MOrbit };
		curve.p3 = { MoonXYZ[0] + XMove, MoonXYZ[1], MoonXYZ[2] - MOrbit };
		//DrawCurve(&curve);
		storePoints(&curve);
		// 'bot' to 'left'
		curve.p0 = { MoonXYZ[0] + XMove, MoonXYZ[1],  MoonXYZ[2] - MOrbit };
		curve.p1 = { MoonXYZ[0] + MOrbit / 1.25 - 100 + XMove, MoonXYZ[1], MoonXYZ[2] - MOrbit };
		curve.p2 = { MoonXYZ[0] + MOrbit + XMove, MoonXYZ[1], MoonXYZ[2] - MOrbit / 1.25 + 100 };
		curve.p3 = { MoonXYZ[0] + MOrbit + XMove, MoonXYZ[1], MoonXYZ[2] };
		//DrawCurve(&curve);
		storePoints(&curve);
		//'left' to 'top'
		curve.p0 = { MoonXYZ[0] + MOrbit + XMove, MoonXYZ[1], MoonXYZ[2] };
		curve.p1 = { MoonXYZ[0] + MOrbit + XMove, MoonXYZ[1], MoonXYZ[2] + MOrbit / 1.25 - 100 };
		curve.p2 = { MoonXYZ[0] + MOrbit / 1.25 - 100 + XMove, MoonXYZ[1], MoonXYZ[2] + MOrbit };
		curve.p3 = { MoonXYZ[0] + XMove, MoonXYZ[1], MoonXYZ[2] + MOrbit };
		//DrawCurve(&curve);
		storePoints(&curve);

		//2nd orbit
		//'top' to 'right'
		curve.p0 = { MoonXYZ[0] + XMove, MoonXYZ[1], MoonXYZ[2] + MOrbit };
		curve.p1 = { MoonXYZ[0] - MOrbit / 1.25 + 100 + XMove, MoonXYZ[1], MoonXYZ[2] + MOrbit };
		curve.p2 = { MoonXYZ[0] - MOrbit + XMove, MoonXYZ[1], MoonXYZ[2] + MOrbit / 1.25 - 100 };
		curve.p3 = { MoonXYZ[0] - MOrbit + XMove, MoonXYZ[1], MoonXYZ[2] };
		//DrawCurve(&curve);
		storePoints(&curve);
		//'right' to 'bot'
		curve.p0 = { MoonXYZ[0] - MOrbit + XMove, MoonXYZ[1], MoonXYZ[2] };
		curve.p1 = { MoonXYZ[0] - MOrbit + XMove, MoonXYZ[1], MoonXYZ[2] - MOrbit / 1.25 + 125 };
		curve.p2 = { MoonXYZ[0] - MOrbit / 1.25 + 100 + XMove, MoonXYZ[1], MoonXYZ[2] - MOrbit };
		curve.p3 = { MoonXYZ[0] + XMove, MoonXYZ[1], MoonXYZ[2] - MOrbit };
		//DrawCurve(&curve);
		storePoints(&curve);
		//'bot' to base
		curve.p0 = { MoonXYZ[0] + XMove, MoonXYZ[1], MoonXYZ[2] - MOrbit };
		curve.p1 = { MoonXYZ[0] + 100 + XMove, MoonXYZ[1], MoonXYZ[2] - MOrbit };
		curve.p2 = { MoonXYZ[0] + MOrbit / 1.5 + XMove, MoonXYZ[1], MoonXYZ[2] - MOrbit / 1.5 };
		curve.p3 = { MoonXYZ[0] + BaseXYZ[0] + XMove, MoonXYZ[1] + BaseXYZ[1], MoonXYZ[2] + BaseXYZ[2] };
		//DrawCurve(&curve);
		storePoints(&curve);

		//DrawCurve(&curve);

		//Takeoff from Moon, travel to Earth
		curve.r = .8;
		curve.g = 0;
		curve.b = 0;

		//Takeoff to 'left'
		curve.p0 = { MoonXYZ[0] + BaseXYZ[0] + XMove, MoonXYZ[1] + BaseXYZ[1], MoonXYZ[2] + BaseXYZ[2] };
		curve.p1 = { MoonXYZ[0] + BaseXYZ[0] + 30 + XMove, MoonXYZ[1] + BaseXYZ[1], MoonXYZ[2] - 100 };
		curve.p2 = { MoonXYZ[0] + BaseXYZ[0] + 55 + XMove, MoonXYZ[1] + 10, MoonXYZ[2] - 50 };
		curve.p3 = { MoonXYZ[0] + MOrbit * .6 + XMove, MoonXYZ[1] + 10, MoonXYZ[2] };
		//DrawCurve(&curve);
		storePoints(&curve);

		//'left' to 'top'/2 
		curve.p0 = { MoonXYZ[0] + MOrbit * .6 + XMove, MoonXYZ[1] + 10, MoonXYZ[2] };
		curve.p1 = { MoonXYZ[0] + MOrbit * .6 + XMove, MoonXYZ[1], MoonXYZ[2] + MOrbit * .3 };
		curve.p2 = { MoonXYZ[0] + MOrbit * .45 + XMove, MoonXYZ[1] + 10, MoonXYZ[2] + MOrbit * .5 };
		curve.p3 = { MoonXYZ[0] + XMove, MoonXYZ[1] + 10, MoonXYZ[2] + MOrbit * .6 };
		//DrawCurve(&curve);
		storePoints(&curve);

		//Moon 1st orbit
		//'top' to 'right'
		curve.p0 = { MoonXYZ[0] + XMove, MoonXYZ[1] + 10,  MoonXYZ[2] + MOrbit * .6 };
		curve.p1 = { MoonXYZ[0] - MOrbit / 1.75 + 100 + XMove, MoonXYZ[1] + 10, MoonXYZ[2] + MOrbit * .6 };
		curve.p2 = { MoonXYZ[0] - MOrbit / 1.5 + 25 + XMove, MoonXYZ[1] + 10, MoonXYZ[2] + MOrbit * .5 };
		curve.p3 = { MoonXYZ[0] - MOrbit / 1.5 + XMove, MoonXYZ[1] + 10, MoonXYZ[2] };
		//DrawCurve(&curve);
		storePoints(&curve);
		//'right' to 'bot'
		curve.p0 = { MoonXYZ[0] - MOrbit / 1.5 + XMove, MoonXYZ[1] + 10, MoonXYZ[2] };
		curve.p1 = { MoonXYZ[0] - MOrbit / 1.5 + XMove, MoonXYZ[1] + 10, MoonXYZ[2] - MOrbit / 1.25 + 125 };
		curve.p2 = { MoonXYZ[0] - MOrbit / 1.5 + 125 + XMove, MoonXYZ[1] + 10, MoonXYZ[2] - MOrbit * .75 };
		curve.p3 = { MoonXYZ[0] + XMove + 100, MoonXYZ[1] + 10, MoonXYZ[2] - MOrbit * .8 };
		//DrawCurve(&curve);
		storePoints(&curve);
		// 'bot' to 'left'
		curve.p0 = { MoonXYZ[0] + XMove + 100, MoonXYZ[1] + 10,  MoonXYZ[2] - MOrbit * .8 };
		curve.p1 = { MoonXYZ[0] + MOrbit / 1.25 - 100 + XMove, MoonXYZ[1] + 10, MoonXYZ[2] - MOrbit * .8 };
		curve.p2 = { MoonXYZ[0] + MOrbit + XMove - 10, MoonXYZ[1] + 10, MoonXYZ[2] - MOrbit / 2 };
		curve.p3 = { MoonXYZ[0] + MOrbit + XMove - 10, MoonXYZ[1] + 10, MoonXYZ[2] };
		//DrawCurve(&curve);
		storePoints(&curve);
		//'left' to 'top'
		curve.p0 = { MoonXYZ[0] + MOrbit + XMove - 10, MoonXYZ[1] + 10, MoonXYZ[2] };
		curve.p1 = { MoonXYZ[0] + MOrbit + XMove, MoonXYZ[1] + 10, MoonXYZ[2] + MOrbit / 1.25 - 100 };
		curve.p2 = { MoonXYZ[0] + MOrbit / 1.25 - 100 + XMove, MoonXYZ[1] + 10, MoonXYZ[2] + MOrbit };
		curve.p3 = { MoonXYZ[0] + XMove, MoonXYZ[1] + 10, MoonXYZ[2] + MOrbit };
		//DrawCurve(&curve);
		storePoints(&curve);

		//2nd orbit
		//'top' to 'right'
		curve.p0 = { MoonXYZ[0] + XMove, MoonXYZ[1] + 10, MoonXYZ[2] + MOrbit };
		curve.p1 = { MoonXYZ[0] - MOrbit / 1.25 + 50 + XMove, MoonXYZ[1] + 10, MoonXYZ[2] + MOrbit };
		curve.p2 = { MoonXYZ[0] - MOrbit + XMove, MoonXYZ[1] + 10, MoonXYZ[2] + MOrbit / 1.25 - 100 };
		curve.p3 = { MoonXYZ[0] - MOrbit + XMove, MoonXYZ[1] + 10, MoonXYZ[2] };
		//DrawCurve(&curve);
		storePoints(&curve);
		//'right' to 'bot'
		curve.p0 = { MoonXYZ[0] - MOrbit + XMove, MoonXYZ[1] + 10, MoonXYZ[2] };
		curve.p1 = { MoonXYZ[0] - MOrbit + XMove, MoonXYZ[1] + 10, MoonXYZ[2] - MOrbit / 1.25 + 125 };
		curve.p2 = { MoonXYZ[0] - MOrbit / 1.25 + XMove, MoonXYZ[1] + 10, MoonXYZ[2] - MOrbit * 1.1 };
		curve.p3 = { MoonXYZ[0] + XMove, MoonXYZ[1] + 10, MoonXYZ[2] - MOrbit * 1.2 };
		//DrawCurve(&curve);
		storePoints(&curve);

		//Not dependent on objects!!!
		//Travel to Earth
		//'bot' of Moon to 'top' of Earth
		curve.p0 = { MoonXYZ[0] + XMove, MoonXYZ[0] + 10, MoonXYZ[0] - MOrbit * 1.2 };
		curve.p1 = { MoonXYZ[0] + XMove + 2 * MOrbit, 0, MoonXYZ[0] - MOrbit - 150 };
		curve.p2 = { 6800 + XMove - EOrbit, 0, EOrbit + 250 };
		curve.p3 = { 6800 + XMove, 0, EOrbit };
		//DrawCurve(&curve);
		storePoints(&curve);

		//'top' to 'left'/2
		curve.p0 = { 6800 + XMove, 10, EOrbit };
		curve.p1 = { 6800 + EOrbit * .6 + XMove, 10, EOrbit*.85 };
		curve.p2 = { 6800 + EOrbit / 1.5 + XMove, 10, EOrbit / 2 + 150 };
		curve.p3 = { 6800 + EOrbit * .7 + XMove, 10, 0 };
		//DrawCurve(&curve);
		storePoints(&curve);

		//'left' to splashdown
		curve.p0 = { 6800 + EOrbit * .7 + XMove, 10, 0 };
		curve.p1 = { 6800 + EOrbit / 2 + XMove, 10, -EOrbit * .75 };
		curve.p2 = { 6800 + EOrbit / 2.25 + XMove, 10, -EOrbit / 2 - 50 };
		curve.p3 = { 6800 + EarthDiameter / 2 + XMove, 10, 0 };
		//DrawCurve(&curve);
		storePoints(&curve);
		DrawCurve();
	}

	if (text) {
		if (!Zoom && View == 3) {//dont display during zoom animation
			glColor3f(1, 0, 0);
			DoRasterString(BaseXYZ[0] + 20, BaseXYZ[1] + 15, BaseXYZ[2] - 5, "Tranquility Base");
		}
		//Marker for the landing site
		if (View == 3) {
			glPushMatrix();
			glColor3f(1, 0, 0);
			glTranslatef(BaseXYZ[0], BaseXYZ[1], BaseXYZ[2]);
			MjbSphere(5, 50, 50);
			glPopMatrix();
		}
		// draw some gratuitous text that is fixed on the screen:
		glDisable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0., 100., 0., 100.);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glColor3f(1., 1., 1.);
		DoRasterString(3., 29., 0., "Apollo 11 - Engr Expo - 5/17/19");
		DoRasterString(3., 25., 0., "0-9, e, w - Viewpoints");
		DoRasterString(3., 21., 0., "F - Toggle Flight Path, r/R - Toggle Repeat");
		DoRasterString(3., 17., 0., "S - Toggle Star Map");
		DoRasterString(3., 13., 0., "M - Toggle Text");
		DoRasterString(3., 9., 0., "Z - Zoom animation");
		DoRasterString(3., 5., 0., "Mouse wheel or +/- - Zoom");
		DoRasterString(3., 1., 0., "Dean Akin, Jonathan Ropp, Shannon Sandy");
	}
	// swap the double-buffered framebuffers:
	glutSwapBuffers();

	// be sure the graphics buffer has been sent:
	glFlush();
}

void
DoColorMenu(int id)
{
	WhichColor = id - RED;
	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

// main menu callback:
void
DoMainMenu(int id)
{
	switch (id)
	{
	case RESET:
		Reset();
		break;

	case QUIT:
		// gracefully close out the graphics:
		// gracefully close the graphics window:
		// gracefully exit the program:
		glutSetWindow(MainWindow);
		glFinish();
		glutDestroyWindow(MainWindow);
		exit(0);
		break;

	default:
		fprintf(stderr, "Don't know what to do with Main Menu ID %d\n", id);
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

void
DoProjectMenu(int id)
{
	WhichProjection = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// use glut to display a string of characters using a raster font:
void
DoRasterString(float x, float y, float z, char *s)
{
	glRasterPos3f((GLfloat)x, (GLfloat)y, (GLfloat)z);

	char c;			// one character to print
	for (; (c = *s) != '\0'; s++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	}
}


// use glut to display a string of characters using a stroke font:
void
DoStrokeString(float x, float y, float z, float ht, char *s)
{
	glPushMatrix();
	glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z);
	float sf = ht / (119.05f + 33.33f);
	glScalef((GLfloat)sf, (GLfloat)sf, (GLfloat)sf);
	char c;			// one character to print
	for (; (c = *s) != '\0'; s++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
	}
	glPopMatrix();
}


// return the number of seconds since the start of the program:
float
ElapsedSeconds()
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet(GLUT_ELAPSED_TIME);

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus()
{
	glutSetWindow(MainWindow);

	int numColors = sizeof(Colors) / (3 * sizeof(int));
	int colormenu = glutCreateMenu(DoColorMenu);
	for (int i = 0; i < numColors; i++)
	{
		glutAddMenuEntry(ColorNames[i], i);
	}

	int projmenu = glutCreateMenu(DoProjectMenu);
	glutAddMenuEntry("Orthographic", ORTHO);
	glutAddMenuEntry("Perspective", PERSP);

	int mainmenu = glutCreateMenu(DoMainMenu);
	glutAddSubMenu("Colors", colormenu);
	glutAddSubMenu("Projection", projmenu);
	glutAddMenuEntry("Reset", RESET);
	glutAddMenuEntry("Quit", QUIT);

	// attach the pop-up menu to the right mouse button:

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}


// initialize the glut and OpenGL libraries:
//	also setup display lists and callback functions

void
InitGraphics()
{
	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	// set the initial window configuration:

	glutInitWindowPosition(0, 0);
	glutInitWindowSize(INIT_WINDOW_SIZE, INIT_WINDOW_SIZE);

	// open the window and set its title:

	MainWindow = glutCreateWindow(WINDOWTITLE);
	glutSetWindowTitle(WINDOWTITLE);

	// set the framebuffer clear values:

	glClearColor(BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3]);

	glutSetWindow(MainWindow);
	glutDisplayFunc(Display);
	glutReshapeFunc(Resize);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(NULL);
	glutVisibilityFunc(Visibility);
	glutEntryFunc(NULL);
	glutSpecialFunc(NULL);
	glutSpaceballMotionFunc(NULL);
	glutSpaceballRotateFunc(NULL);
	glutSpaceballButtonFunc(NULL);
	glutButtonBoxFunc(NULL);
	glutDialsFunc(NULL);
	glutTabletMotionFunc(NULL);
	glutTabletButtonFunc(NULL);
	glutMenuStateFunc(NULL);
	glutTimerFunc(-1, NULL, 0);
	glutIdleFunc(Animate);
	glutFullScreen();

	// init glew (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		fprintf(stderr, "glewInit Error\n");
	}
	else
		fprintf(stderr, "GLEW initialized OK\n");
	fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

	//Texture initialization
	int width = 1024;
	int height = 512;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &TexE);
	glGenTextures(1, &TexM);
	glGenTextures(1, &TexS);
	glGenTextures(1, &TexSt);
	glGenTextures(1, &AFlag);
	glGenTextures(1, &DayEarthTex);
	glGenTextures(1, &NightEarthTex);
	glGenTextures(1, &CloudEarthTex);

	glBindTexture(GL_TEXTURE_2D, DayEarthTex);
	unsigned char* EarthDay = BmpToTexture("2k_earth_daymap.bmp", &width, &height);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, EarthDay);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glBindTexture(GL_TEXTURE_2D, NightEarthTex);
	unsigned char* EarthNight = BmpToTexture("2k_earth_nightmap.bmp", &width, &height);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, EarthNight);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glBindTexture(GL_TEXTURE_2D, CloudEarthTex);
	unsigned char* EarthClouds = BmpToTexture("2k_earth_clouds.bmp", &width, &height);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, EarthClouds);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glBindTexture(GL_TEXTURE_2D, TexS);
	unsigned char* SunTexA = BmpToTexture("2k_sun.bmp", &width, &height);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, SunTexA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glBindTexture(GL_TEXTURE_2D, TexM);
	unsigned char* MoonXYZexA = BmpToTexture("2k_moon.bmp", &width, &height);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, MoonXYZexA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glBindTexture(GL_TEXTURE_2D, TexSt);
	unsigned char* StarMapTexA = BmpToTexture("2k_stars.bmp", &width, &height);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, StarMapTexA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	int width2 = 1200;
	int height2 = 632;

	glBindTexture(GL_TEXTURE_2D, AFlag);
	unsigned char* AmericanFlagTex = BmpToTexture("AFlag.bmp", &width2, &height2);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, AmericanFlagTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	//Shader initiliazation
	//if (LunarMat.Open("LandingSite.mtl") != 0)
	//{
	//	fprintf(stderr, "Could not read file name.\n");
	//}
	//LunarMat.ReadMtlFile();
	//LunarMat.Close();
	char * mtlFilename = (char *)("./LandingSite.mtl");

	if (LunarMat.Open(mtlFilename) != 0)
	{
		fprintf(stderr, "Could not read the mtl file\n");
		return;
	}
	else
		fprintf(stderr, "READING ./LandSite.mtl");
	LunarMat.ReadMtlFile();
	LunarMat.Close();

	FragmentLight = new GLSLProgram();
	bool valid = FragmentLight->Create("fragmentlight.vert", "fragmentlight.frag");
	if (!valid)
	{
		fprintf(stderr, "GLSL Shader error\n");
	}
	else {
		fprintf(stderr, "GLSL Fragment Lighting Shader Successfully Initialized\n");
	}

	LunarMask = new GLSLProgram();
	valid = LunarMask->Create("lunarSurfaceMask.vert", "lunarSurfaceMask.frag");
	if (!valid)
	{
		fprintf(stderr, "GLSL shader error\n");
	}
	else {
		fprintf(stderr, "GLSL lunar mask shader is all good\n");
	}

	EarthShadeModel = new GLSLProgram();
	valid = EarthShadeModel->Create("ShadeModel.vert", "ShadeModel.frag");
	if (!valid)
	{
		fprintf(stderr, "GLSL Shader error\n");
	}
	else {
		fprintf(stderr, "GLSL Earth Shade Model Shader Successfully Initialized\n");
	}

	MoonShadeModel = new GLSLProgram();
	valid = MoonShadeModel->Create("ShadeModel.vert", "ShadeModelMod.frag");
	if (!valid)
	{
		fprintf(stderr, "GLSL Shader error\n");
	}
	else {
		fprintf(stderr, "GLSL Moon Shade Model Shader Successfully Initialized\n");
	}
	//Initializing keyframe animation objects.
	EarthWalk.keyframeData(EyeEarth,26);
	Test.keyframeData(test, 5);
	Flight.keyframeData(path, 35);
	LMFirstStep.keyframeData(SmallStepForMan, 37);
}

// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists()
{
	glutSetWindow(MainWindow);

	FlagPoleList = glGenLists(1);
	glNewList(FlagPoleList, GL_COMPILE);

	//vertical cylinder
	glPushMatrix();
	glColor3f(1, 1, 1);
	GLUquadricObj *quad1;
	quad1 = gluNewQuadric();
	gluCylinder(quad1, .04, .04, 7, 10, 100);
	glPopMatrix();

	//horizontal cylinder
	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslatef(0, 0, 7);
	glRotatef(90, 0, 1, 0);
	GLUquadricObj *quad2;
	quad2 = gluNewQuadric();
	gluCylinder(quad2, .04, .04, 5 + .04, 10, 100);
	glPopMatrix();
	glEndList();

	FlagList = glGenLists(1);
	glNewList(FlagList, GL_COMPILE);

	//flag
	glPushMatrix();
	glBegin(GL_QUADS);

	glTexCoord2f(0, 0);
	glVertex3f(0, 0, 4);
	glTexCoord2f(0, 1);
	glVertex3f(0, 0, 7 - .04);
	glTexCoord2f(1, 1);
	glVertex3f(5, 0, 7 - .04);
	glTexCoord2f(1, 0);
	glVertex3f(5, 0, 4);

	glEnd();
	glPopMatrix();
	glEndList();

	//Other objects
	SaturnV = glGenLists(1);
	glNewList(SaturnV, GL_COMPILE);
	LoadObjFileO("./Apollo_SpaceCraft/Apollo_Spacecraft.obj");
	glEndList();

	Astronaut = glGenLists(1);
	glNewList(Astronaut, GL_COMPILE);
	LoadObjFileO("Astronaut_.obj");
	glEndList();
	
	LandingSite = glGenLists(1);
	glNewList(LandingSite, GL_COMPILE);
	LoadObjFileO("LandingSite.obj");
	glEndList();


	LunarModule = glGenLists(1);
	glNewList(LunarModule, GL_COMPILE);
	LunarModule = LoadObjFile("./ApolloLunarModule/Ap.obj");
	glEndList();

}

// the keyboard callback:

void
Keyboard(unsigned char c, int x, int y)
{

	switch (c)
	{

	case 'f':
	case 'F':
		FPath = !FPath;
		break;

	case 'p':
	case 'P':
		WhichProjection = PERSP;
		break;

	case 'q':
	case 'Q':
	case ESCAPE:
		DoMainMenu(QUIT);	// will not return here
		break;				// happy compiler

	case 'l':
	case 'L':
		Light1On = !Light1On;
		Light2On = !Light2On;
		Light3On = !Light3On;
		break;

	case 's':
	case 'S':
		stars = !stars;
		break;

	case 'm':
	case 'M':
		text = !text;
		break;

	case '1':
		Zoom = false;
		Xrot = Yrot = 0.;
		Scale = 1.0;
		View = 1;
		loadMoon = 1;
		Flight.Reset();
		PlaySound(NULL, NULL, SND_APPLICATION);
		break;

	case '2':
		Zoom = false;
		Xrot = Yrot = 0.;
		Scale = 1.0;
		View = 2;
		loadMoon = 1;
		PlaySound(NULL, NULL, SND_APPLICATION);
		break;

	case '3':
		Zoom = false;
		Xrot = Yrot = 0.;
		Scale = 1.0;
		View = 3;
		loadMoon = 1;
		PlaySound(NULL, NULL, SND_APPLICATION);
		break;

	case '4':
		Zoom = false;
		Xrot = Yrot = 0.;
		Scale = 1.0;
		View = 4;
		loadMoon = 0;
		PlaySound(NULL, NULL, SND_APPLICATION);
		break;

	case '5':
		Zoom = false;
		Xrot = Yrot = 0.;
		Scale = 1.0;
		View = 5;
		loadMoon = 0;
		PlaySound(NULL, NULL, SND_APPLICATION);
		break;

	case '6':
		Zoom = false;
		Xrot = Yrot = 0.;
		Scale = 1.0;
		View = 6;
		loadMoon = 0;
		PlaySound("OneSmallStep.wav", NULL, SND_ASYNC);
		break;

	case '7':
		Zoom = false;
		Xrot = Yrot = 0.;
		Scale = 1.0;
		View = 7;
		loadMoon = 0;
		PlaySound(NULL, NULL, SND_APPLICATION);
		break;

	case '8':
		Zoom = false;
		Xrot = Yrot = 0;
		Scale = 1.0;
		View = 8;
		loadMoon = 0;
		PlaySound("landing.wav", NULL, SND_ASYNC);
		break;

	case '9':
		Zoom = false;
		Xrot = Yrot = 0.;
		Scale = 1.0;
		View = 9;
		loadMoon = 0;
		break;

	case '0':
		Zoom = false;
		Xrot = Yrot = 0.;
		Scale = 1.0;
		View = 0;
		loadMoon = 0;
		break;
	case 'e':
		Zoom = false;
		Xrot = Yrot = 0.;
		Scale = 1.0;
		View = 10;
		loadMoon = 0;
		PlaySound(NULL, NULL, SND_APPLICATION);
		break;
	case 'w':
		Zoom = false;
		Xrot = Yrot = 0.;
		Scale = 1.;
		View = 11;
		loadMoon = 0;
		break;
	case 'r':
	case 'R':
		EarthWalk.Repeat = !EarthWalk.Repeat;
		Flight.Repeat = !Flight.Repeat;
		LMFirstStep.Repeat = !LMFirstStep.Repeat;
		break;
	case 'z':
		Zoom = true;
		Xrot = Yrot = 0.;
		Scale = 1.;
		View = 3;
		loadMoon = 1;
		PlaySound(NULL, NULL, SND_APPLICATION);
		break;
	case '-':
		Scale = Scale - .1;
		break;
	case '+':
	case '=':	//I don't want to keep hitting shift to get '+'
		Scale = Scale + .1;
		break;

		/*case '0':
			dotPosX = dotPosX + 1;
			fprintf(stderr, "X: '%f' Y: '%f' Z: '%f')\n", dotPosX, dotPosY, dotPosZ);
			break;
		case '9':
			dotPosY = dotPosY + 1;
			fprintf(stderr, "X: '%f' Y: '%f' Z: '%f')\n", dotPosX, dotPosY, dotPosZ);
			break;
		case '8':
			dotPosZ = dotPosZ + 1;
			fprintf(stderr, "X: '%f' Y: '%f' Z: '%f')\n", dotPosX, dotPosY, dotPosZ);
			break;
		case ')':
			dotPosX = dotPosX - 1;
			fprintf(stderr, "X: '%f' Y: '%f' Z: '%f')\n", dotPosX, dotPosY, dotPosZ);
			break;
		case '(':
			dotPosY = dotPosY - 1;
			fprintf(stderr, "X: '%f' Y: '%f' Z: '%f')\n", dotPosX, dotPosY, dotPosZ);
			break;
		case '*':
			dotPosZ = dotPosZ - 1;
			fprintf(stderr, "X: '%f' Y: '%f' Z: '%f')\n", dotPosX, dotPosY, dotPosZ);
			break;
			*/
	default:
		fprintf(stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c);
	}

	// force a call to Display( ):

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// called when the mouse button transitions down or up:

void
MouseButton(int button, int state, int x, int y)
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	// get the proper button bit mask:

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		b = LEFT;		break;

	case GLUT_MIDDLE_BUTTON:
		b = MIDDLE;		break;

	case GLUT_RIGHT_BUTTON:
		b = RIGHT;		break;

	default:
		b = 0;
		fprintf(stderr, "Unknown mouse button: %d\n", button);
	}

	// button down sets the bit, up clears the bit:

	if (state == GLUT_DOWN)
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}
}


// called when the mouse moves while a button is down:

void
MouseMotion(int x, int y)
{

	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if ((ActiveButton & LEFT) != 0)
	{
		Xrot += (ANGFACT*dy);
		Yrot += (ANGFACT*dx);
	}


	if ((ActiveButton & MIDDLE) != 0)
	{
		Scale += SCLFACT * (float)(dx - dy);

		// keep object from turning inside-out or disappearing:

		if (Scale < MINSCALE)
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset()
{
	ActiveButton = 0;
	Scale = 1.0;
	WhichColor = WHITE;
	WhichProjection = PERSP;
	Xrot = Yrot = 0.;

}


// called when user resizes the window:

void
Resize(int width, int height)
{
	// don't really need to do anything since window size is
	// checked each time in Display( ):
	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

// handle a change to the window's visibility:

void
Visibility(int state)
{
	if (state == GLUT_VISIBLE)
	{
		glutSetWindow(MainWindow);
		glutPostRedisplay();
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}


///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////

// the stroke characters 'X' 'Y' 'Z' :

static float xx[] = {
		0.f, 1.f, 0.f, 1.f
};

static float xy[] = {
		-.5f, .5f, .5f, -.5f
};

static int xorder[] = {
		1, 2, -3, 4
};

static float yx[] = {
		0.f, 0.f, -.5f, .5f
};

static float yy[] = {
		0.f, .6f, 1.f, 1.f
};

static int yorder[] = {
		1, 2, 3, -2, 4
};

static float zx[] = {
		1.f, 0.f, 1.f, 0.f, .25f, .75f
};

static float zy[] = {
		.5f, .5f, -.5f, -.5f, 0.f, 0.f
};

static int zorder[] = {
		1, 2, 3, 4, -5, 6
};

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;