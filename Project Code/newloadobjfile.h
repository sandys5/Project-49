#ifndef LOADOBJFILE_H
#define LOADOBJFILE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <GL/gl.h>
#include <vector>


#define OBJDELIMS		" \t"

const void  *	NULLPTR = (void *)0;

const char *	DELIMS = " \t";

struct Vertex
{
	float x, y, z;
};

struct Normal
{
	float nx, ny, nz;
};

struct TextureCoord
{
	float s, t, p;
};

struct face
{
	int v, n, t;
};


GLuint	LoadObjFile( char * );

void	Cross( float [3], float [3], float [3] );
char *	ReadRestOfLine( FILE * );
void	ReadObjVTN( char *, int *, int *, int * );
float	Unit( float [3] );
float	Unit( float [3], float [3] );

int	Readline( FILE *, char * );
float * Array3( float, float, float );
float * Array3( float * );

unsigned char * BmpToTexture( char *, int *, int * );


class
Mtl
{
    public:
	char *		Name;
	bool		HasKa, HasKd, HasKs, HasKe, HasTr, HasTf, HasD, HasNi, HasNs, HasIllum;
	bool		HasMapKa, HasMapKd, HasMapBump, HasMapRefl;
	float		Ka[3];
	float		Kd[3];
	float		Ke[3];
	float		Ks[3];
	float		Tr;
	float		Tf[3];
	float		D;
	float		Ni;
	float		Ns;
	float		Illum;
	char *		MapKa;
	char *		MapKd;
	char *		MapBump;
	GLuint		BindKaTexture;
	GLuint		BindKdTexture;
	GLuint		BindBumpTexture;
	float		bumpFactor;
	char *		MapRefl;
	Mtl *		Next;

	Mtl( )
	{
		Next = (Mtl *) NULLPTR;
		HasKa = false;
		HasKd = false;
		HasKs = false;
		HasKe = false;
		HasD = false;
		HasNi = false;
		HasNs = false;
		HasIllum = false;
		HasMapKa = false;
		HasMapKd = false;
		HasMapBump = false;
		HasMapRefl = false;
	};

	void
	SetOpenglMtlProperties( )
	{
		glDisable( GL_TEXTURE_2D );
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT,   Array3( 0., 0., 0. ) );
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE,   Array3( 0., 0., 0. ) );
		glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION,  Array3( 0., 0., 0. ) );
		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,  Array3( 0., 0., 0. ) );
		glMaterialf ( GL_FRONT_AND_BACK, GL_SHININESS, 1. );

		if( this->HasKa )
			glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT,   Array3( this->Ka ) );
		if( this->HasKd )
			glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE,   Array3( this->Kd ) );
		if( this->HasKe )
			glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION,  Array3( this->Ke ) );
		if( this->HasKs )
			glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,  Array3( this->Ks ) );
		if( this->HasNs )
			glMaterialf ( GL_FRONT_AND_BACK, GL_SHININESS, this->Ns );
		if( this->HasMapKa )
		{
			glBindTexture( GL_TEXTURE_2D, this->BindKaTexture );
			glEnable( GL_TEXTURE_2D );
			glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
		}
		if( this->HasMapKd )
		{
			glBindTexture( GL_TEXTURE_2D, this->BindKdTexture );
			glEnable( GL_TEXTURE_2D );
			glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
		}
		if( this->HasMapBump )
		{
			//glBindTexture( GL_TEXTURE_2D, this->BindBumpTexture );
			//glEnable( GL_TEXTURE_2D );
			//glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
		}
	};

};


class
Mtls
{
    public:
	FILE *		Fp;
	Mtl *		First;
	Mtl *		Last;

	int Open( char * fileName );
	void ReadMtlFile( );
	void Close( );
	Mtl * FindMtl( char *mtlName );

	Mtls( )
	{
		Fp = (FILE *) NULLPTR;
		First = Last = (Mtl *) NULLPTR;
	};
};

int			// 0 = success, !0 = failure
Mtls::Open( char * fileName )
{
	Fp = fopen( fileName, "r" );
	if( Fp == NULLPTR )
	{
		fprintf( stderr, "Cannot open file '%s'\n", fileName );
		return 1;
	}

	return 0;
}


// NOTE: this needs to be executed when we are in immediate mode,
// 	 not when we are in display list (retained) mode
// 	 because of setting the texture image

void
Mtls::ReadMtlFile( )
{
	char line[256];
	Mtl * thisMtl = (Mtl *) NULLPTR;
	int c;
	while( ( c = Readline( Fp, line) ) != EOF )
	{
		if( c == EOF )
			break;

		if( line[0] == '\0' )
			continue;

		//fprintf( stderr, "line = '%s'\n", line );

		char * tok = strtok( line, DELIMS );			// first token is the variable name
		char cc;
		for( char *cp = tok; ( cc = *cp ) != '\0'; cp++ )
		{
			if( isupper(cc) )
				*cp = tolower( cc );
		}
		//fprintf( stderr, "Variable name = '%s'\n", tok );

		// process the rest of the line:

		while( tok != (char *) NULLPTR )
		{
			if( tok[0] == '#' )
				break;		// comment causes us to stop reading this line
			//fprintf( stderr, "tok = *%s*\n", tok );

			if( strcmp( tok, "newmtl" ) == 0 )
			{
				thisMtl = new Mtl;
				thisMtl->Next = (Mtl *) NULLPTR;
				if( First == (Mtl *) NULLPTR )
				{
					First = thisMtl;
				}
				if( Last != (Mtl *) NULLPTR )
				{
					Last->Next = thisMtl;
				}
				Last = thisMtl;
				tok = strtok( (char *)NULLPTR, DELIMS );
				thisMtl->Name = strdup(tok);
				fprintf( stderr, "Material name = '%s'\n", thisMtl->Name );
				break;
			}

			if( strcmp( tok, "ka" ) == 0 )
			{
				tok = strtok( (char *)NULLPTR, DELIMS );
				if( tok != (char *)NULLPTR )	thisMtl->Ka[0] = atof( tok );
				tok = strtok( (char *)NULLPTR, DELIMS );
				if( tok != (char *)NULLPTR )	thisMtl->Ka[1] = atof( tok );
				tok = strtok( (char *)NULLPTR, DELIMS );
				if( tok != (char *)NULLPTR )	thisMtl->Ka[2] = atof( tok );
				thisMtl->HasKa = true;
				break;
			}

			if( strcmp( tok, "kd" ) == 0 )
			{
				tok = strtok( (char *)NULLPTR, DELIMS );
				if( tok != (char *)NULLPTR )	thisMtl->Kd[0] = atof( tok );
				tok = strtok( (char *)NULLPTR, DELIMS );
				if( tok != (char *)NULLPTR )	thisMtl->Kd[1] = atof( tok );
				tok = strtok( (char *)NULLPTR, DELIMS );
				if( tok != (char *)NULLPTR )	thisMtl->Kd[2] = atof( tok );
				thisMtl->HasKd = true;
				break;
			}

			if( strcmp( tok, "ke" ) == 0 )
			{
				tok = strtok( (char *)NULLPTR, DELIMS );
				thisMtl->Ke[0] = atof( tok );
				tok = strtok( (char *)NULLPTR, DELIMS );
				thisMtl->Ke[1] = atof( tok );
				tok = strtok( (char *)NULLPTR, DELIMS );
				thisMtl->Ke[2] = atof( tok );
				thisMtl->HasKe = true;
				break;
			}

			if( strcmp( tok, "ks" ) == 0 )
			{
				tok = strtok( (char *)NULLPTR, DELIMS );
				thisMtl->Ks[0] = atof( tok );
				tok = strtok( (char *)NULLPTR, DELIMS );
				thisMtl->Ks[1] = atof( tok );
				tok = strtok( (char *)NULLPTR, DELIMS );
				thisMtl->Ks[2] = atof( tok );
				thisMtl->HasKs = true;
				break;
			}

			if( strcmp( tok, "tr" ) == 0 )
			{
				tok = strtok( (char *)NULLPTR, DELIMS );
				thisMtl->Tr = atof( tok );
				thisMtl->HasTr = true;
				break;
			}

			if( strcmp( tok, "tf" ) == 0 )
			{
				tok = strtok( (char *)NULLPTR, DELIMS );
				thisMtl->Tf[0] = atof( tok );
				tok = strtok( (char *)NULLPTR, DELIMS );
				thisMtl->Tf[1] = atof( tok );
				tok = strtok( (char *)NULLPTR, DELIMS );
				thisMtl->Tf[2] = atof( tok );
				thisMtl->HasTf = true;
				break;
			}

			if( strcmp( tok, "d" ) == 0 )
			{
				tok = strtok( (char *)NULLPTR, DELIMS );
				thisMtl->D = atof( tok );
				thisMtl->HasD = true;
				break;
			}

			if( strcmp( tok, "ni" ) == 0 )
			{
				tok = strtok( (char *)NULLPTR, DELIMS );
				thisMtl->Ni = atof( tok );
				thisMtl->HasNi = true;
				break;
			}

			if( strcmp( tok, "ns" ) == 0 )
			{
				tok = strtok( (char *)NULLPTR, DELIMS );
				thisMtl->Ns = atof( tok );
				thisMtl->HasNs = true;
				break;
			}

			if( strcmp( tok, "illum" ) == 0 )
			{
				tok = strtok( (char *)NULLPTR, DELIMS );
				thisMtl->Illum = atof( tok );
				thisMtl->HasIllum = true;
				break;
			}

			if( strcmp( tok, "map_ka" ) == 0 )
			{
				tok = strtok( (char *)NULLPTR, DELIMS );
				thisMtl->MapKa = strdup( tok );
				thisMtl->HasMapKa = true;
				int width, height;
				unsigned char * texture = BmpToTexture( thisMtl->MapKa, &width, &height );
				if( texture == NULL )
				{
					fprintf( stderr, "Cannot open texture '%s'\n", thisMtl->MapKa );
					thisMtl->HasMapKa = false;
				}
				else
				{
					//fprintf(stderr, "Opened texture '%s'\n", thisMtl->MapKa);
					glGenTextures( 1, &thisMtl->BindKaTexture );
					fprintf( stderr, "BindKaTexture = %d\n", thisMtl->BindKaTexture );
					glBindTexture( GL_TEXTURE_2D, thisMtl->BindKaTexture );
					glEnable( GL_TEXTURE_2D );
					glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
					glTexImage2D( GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture );
					delete [ ] texture;
				}
				break;
			}

			if( strcmp( tok, "map_kd" ) == 0 )
			{
				tok = strtok( (char *)NULLPTR, DELIMS );
				thisMtl->MapKd = strdup( tok );
				thisMtl->HasMapKd = true;
				int width, height;
				unsigned char * texture = BmpToTexture( thisMtl->MapKd, &width, &height );
				if( texture == NULL )
				{
					fprintf( stderr, "Cannot open texture '%s'\n", thisMtl->MapKd );
					thisMtl->HasMapKd = false;
				}
				else
				{
					//fprintf(stderr, "Opened texture '%s'\n", thisMtl->MapKd);
					glGenTextures( 1, &thisMtl->BindKdTexture );
					fprintf( stderr, "BindKdTexture = %d\n", thisMtl->BindKdTexture );
					glBindTexture( GL_TEXTURE_2D, thisMtl->BindKdTexture );
					glEnable( GL_TEXTURE_2D );
					glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
					glTexImage2D( GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture );
					delete [ ] texture;
				}
				break;
			}

			if( strcmp( tok, "map_bump" ) == 0 )
			{
				thisMtl->bumpFactor = 1.0;
				tok = strtok( (char *)NULLPTR, DELIMS );
				thisMtl->MapBump = strdup(tok);
				if( tok[0] == '-' )
				{
					if( strcmp( tok, "-bm" ) == 0 )
					{
						tok = strtok( (char *)NULLPTR, DELIMS );
						thisMtl->bumpFactor = atof( tok );
						fprintf( stderr, "bumpFactor = %6.3f\n", thisMtl->bumpFactor );
						tok = strtok((char *)NULLPTR, DELIMS);
						thisMtl->MapBump = strdup(tok);
					}
					else
					{
						fprintf( stderr, "Unknown option in map_bump: '%s'\n", tok );
					}
				}
				thisMtl->HasMapBump = true;
				int width, height;
				unsigned char * texture = BmpToTexture( thisMtl->MapBump, &width, &height );
				if( texture == NULL )
				{
					fprintf( stderr, "Cannot open texture '%s'\n", thisMtl->MapBump );
					thisMtl->HasMapBump = false;
				}
				else
				{
					thisMtl->HasMapBump = false;
					fprintf( stderr, "Width = %d ; Height = %d\n", width, height );
					//glGenTextures( 1, &thisMtl->BindBumpTexture );
					//glBindTexture( GL_TEXTURE_2D, thisMtl->BindBumpTexture );
					//glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
					//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
					//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
					//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
					//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					//glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
					//glTexImage2D( GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture );
					delete [ ] texture;
				}
				break;
			}

			if( strcmp( tok, "map_refl" ) == 0 )
			{
#ifdef NOTDEF
				tok = strtok( (char *)NULLPTR, DELIMS );
				thisMtl->MapRefl = strdup( tok );
				thisMtl->HasMapRefl = true;
				int width, height;
				unsigned char * texture = BmpToTexture( thisMtl->MapRefl, &width, &height );
				if( texture == NULL )
				{
					fprintf( stderr, "Cannot open texture '%s'\n", thisMtl->MapRefl );
					thisMtl->HasMapRefl = false;
				}
				else
				{
					fprintf( stderr, "Width = %d ; Height = %d\n", width, height );
					glGenTextures( 1, &thisMtl->BindReflTexture );
					glBindTexture( GL_TEXTURE_2D, thisMtl->BindReflTexture );
					glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
					glTexImage2D( GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture );
					delete [ ] texture;
				}
#endif
				break;
			}

			fprintf( stderr, "Don't recognize Mtl file token '%s'\n", tok );
		}
	}
}

void
Mtls::Close( )
{
	fclose( Fp );
	Fp = (FILE *)NULLPTR;
};

Mtl *
Mtls::FindMtl( char *mtlName )
{
	for( Mtl * mtl = First; mtl != (Mtl *)NULLPTR; mtl = mtl->Next )
	{
		if( strcmp( mtl->Name, mtlName ) == 0 )
		{
			return mtl;
		}
	}
	return (Mtl *)NULLPTR;
};

#endif		// #ifndef
