/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2016 RWTH Aachen University               */
/*============================================================================*/
/*                                  License                                   */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU Lesser General Public License as published  */
/*  by the Free Software Foundation, either version 3 of the License, or      */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU Lesser General Public License for more details.                       */
/*                                                                            */
/*  You should have received a copy of the GNU Lesser General Public License  */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/*============================================================================*/
/*                                Contributors                                */
/*                                                                            */
/*============================================================================*/



//-------------------------------
//  Includes
//-------------------------------
#include "OSGAC3DFileType.h"

#include <VistaBase/VistaStreamUtils.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(push)
#pragma warning(disable: 4127)
#pragma warning(disable: 4189)
#pragma warning(disable: 4231)
#pragma warning(disable: 4267)
#endif

#include <OpenSG/OSGConfig.h>

#include <OpenSG/OSGGL.h>
#include <OpenSG/OSGLog.h>

#include <OpenSG/OSGNode.h>
#include <OpenSG/OSGGeometry.h>
#include <OpenSG/OSGGeoProperty.h>
#include <OpenSG/OSGGeoFunctions.h>

#include <OpenSG/OSGImageFileHandler.h>
#include <OpenSG/OSGPathHandler.h>
#include <OpenSG/OSGGroup.h>
#include <OpenSG/OSGSceneFileHandler.h>
#include <OpenSG/OSGSimpleGeometry.h>
#include <OpenSG/OSGTransform.h>
#include <OpenSG/OSGPolygonChunk.h>
#include <OpenSG/OSGSimpleAttachments.h>
#include <OpenSG/OSGSimpleMaterial.h>
#include <OpenSG/OSGSimpleTexturedMaterial.h>
#include <OpenSG/OSGGeoFunctions.h>

#ifdef WIN32
#pragma warning(pop)
#endif

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>

OSG_USING_NAMESPACE

#if defined(OSG_WIN32_ICL) && !defined(OSG_CHECK_FIELDSETARG)
#pragma warning (disable : 383)
#endif

/*****************************
 *   Types
 *****************************/
// Static Class Varible implementations:
const Char8 *OSGAC3DSceneFileType::_suffixA[] =  { "ac" };

OSGAC3DSceneFileType  OSGAC3DSceneFileType::_the         (_suffixA,
							  sizeof(_suffixA),
							  false,
							  10,
							  SceneFileType::OSG_READ_SUPPORTED);

/*****************************
 *    Classvariables
 *****************************/


/********************************
 *    Class methodes
 *******************************/


typedef struct ACPoint_t
{
	float x, y, z;
} ACPoint;


typedef struct ACNormal_t
{
	float x, y, z;
} ACNormal;


typedef struct ACVertex_t
{
	float x, y, z;
	ACNormal normal;
} ACVertex;

typedef struct ACUV_t
{
	float u, v;
} ACUV;


typedef struct ACSurface_t
{
	int *vertref;
	ACUV *uvs;
	int num_vertref;
	ACNormal normal;
	int flags;
	int mat;
} ACSurface;

typedef struct ACObject_t
{
	ACPoint loc;
	char *name;
	char *data;
	char *url;
	ACVertex *vertices;
	int num_vert;

	ACSurface *surfaces;
	int num_surf;
	float texture_repeat_x, texture_repeat_y;
	float texture_offset_x, texture_offset_y;

	int num_kids;
	struct ACObject_t **kids;
	float matrix[9];
	int type;
	char *texture;
	float crease;
} ACObject;

typedef struct ACCol_t
{
	float r, g, b, a;
} ACCol;

typedef struct Material_t
{
	ACCol rgb; /* diffuse **/
	ACCol ambient;
	ACCol specular;
	ACCol emissive;
	float shininess;
	float transparency;
	char *name;
} ACMaterial;

#define AC3D_OBJECT_WORLD 999
#define AC3D_OBJECT_NORMAL 0
#define AC3D_OBJECT_GROUP 1
#define AC3D_OBJECT_LIGHT 2

#define AC3D_SURFACE_SHADED (1<<4)
#define AC3D_SURFACE_TWOSIDED (1<<5)

#define AC3D_SURFACE_TYPE_POLYGON (0)
#define AC3D_SURFACE_TYPE_CLOSEDLINE (1)
#define AC3D_SURFACE_TYPE_LINE (2)

#ifdef WIN32
#if defined _AC3DLOAD_DLLEXPORT
#define Prototype  __declspec( dllexport )
#elif defined _AC3DLOAD_DLLIMPORT
#define Prototype  __declspec( dllimport )
#else
// define empty macro
#define Prototype
#endif // DLL binding
#else // non win32 omit prototype define
#define Prototype
#endif

static Prototype ACObject *ac3d_load_ac3d(std::istream &file, ACMaterial *pPalette, int &iNumPalette);
static Prototype void ac3d_object_free(ACObject *ob);

#define Private static
#define Boolean int

#ifndef TRUE
#define FALSE (0)
#define TRUE (!FALSE)
#endif

#define STRING(s)  (char *)(strcpy((char *)myalloc(strlen(s)+1), s))
#define streq(a,b)  (!strcmp(a,b))
#define myalloc malloc
#define myfree free

/*static Boolean read_line(FILE *f, char *buff, int &line)
  {
  fgets(buff, 255, f); line++;
  return(TRUE);
  }
*/

static int tokc = 0;
static char *tokv[30];

static Prototype int get_tokens(char *s, int *argc, char *argv[])
/** bung '\0' chars at the end of tokens and set up the array (tokv) and count (tokc)
like argv argc **/
{
	char *p = s;
	char *st;
	char c;
	//int n;
	int tc;

	tc = 0;
	while ((c=*p) != 0)
	{
		if ((c != ' ') && (c != '\t') && (c != '\n') && ( c != 13))
		{
			if (c == '"')
			{
				c = *p++;
				st = p;
				while ((c = *p) && ((c != '"')&&(c != '\n')&& ( c != 13)) )
				{
					if (c == '\\')
						strcpy(p, p+1);
					p++;
				}
				*p=0;
				argv[tc++] = st;
			}
			else
			{
				st = p;
				while ((c = *p) && ((c != ' ') && (c != '\t') && (c != '\n') && ( c != 13)) )
					p++;
				*p=0;
				argv[tc++] = st;
			}
		}
		p++;
	}

	*argc = tc;
	return(tc);
}


static ACObject *new_object()
{
	ACObject *ob = (ACObject *)myalloc(sizeof(ACObject));

	ob->loc.x = ob->loc.y = ob->loc.z = 0.0;
	ob->name = ob->url = NULL;
	ob->data = NULL;
	ob->vertices = NULL;
	ob->num_vert = 0;
	ob->surfaces = NULL;
	ob->num_surf = 0;
	ob->texture = NULL;
	ob->crease = 45;
	ob->texture_repeat_x = ob->texture_repeat_y = 1.0;
	ob->texture_offset_x = ob->texture_offset_y = 0.0;
	ob->kids = NULL;
	ob->num_kids = 0;
	ob->matrix[0] = 1;
	ob->matrix[1] = 0;
	ob->matrix[2] = 0;
	ob->matrix[3] = 0;
	ob->matrix[4] = 1;
	ob->matrix[5] = 0;
	ob->matrix[6] = 0;
	ob->matrix[7] = 0;
	ob->matrix[8] = 1;
	return(ob);
}


static void ac3d_object_free(ACObject *ob)
{
	int n, s;

	for (n = 0; n < ob->num_kids; n++)
		ac3d_object_free(ob->kids[n]);

	if(ob->kids)
		free(ob->kids);

	if (ob->vertices)
		free(ob->vertices);

	for (s = 0; s < ob->num_surf; s ++)
	{
		free(ob->surfaces[s].vertref);
		free(ob->surfaces[s].uvs);
	}

	if (ob->surfaces)
		free(ob->surfaces);

	if (ob->data)
		free(ob->data);

	if (ob->url)
		free(ob->url);

	if (ob->name)
		free(ob->name);
	if(ob->texture)
		free(ob->texture);

	free(ob);

}

static void init_surface(ACSurface *s)
{
	s->vertref = NULL;
	s->uvs = NULL;
	s->num_vertref = 0;
	s->flags = 0;
	s->mat = 0;
	s->normal.x = 0.0; s->normal.z = 0.0; s->normal.z = 0.0;
}

static void tri_calc_normal(ACPoint *v1, ACPoint *v2, ACPoint *v3, ACPoint *n)
{
	double len;

	n->x = (v2->y-v1->y)*(v3->z-v1->z)-(v3->y-v1->y)*(v2->z-v1->z);
	n->y = (v2->z-v1->z)*(v3->x-v1->x)-(v3->z-v1->z)*(v2->x-v1->x);
	n->z = (v2->x-v1->x)*(v3->y-v1->y)-(v3->x-v1->x)*(v2->y-v1->y);
	len = sqrt(n->x*n->x + n->y*n->y + n->z*n->z);

	if (len > 0)
	{
		n->x /= (float)len;
		n->y /= (float)len;
		n->z /= (float)len;
	}

}

static ACSurface *read_surface(std::istream &file, ACSurface *s, ACObject *ob, char *buff, int &line)
{
	char t[20];


	init_surface(s);

	while (!file.eof())
	{
		file.getline(buff, 256);
		++line;
		//read_line(f, buff ,line);
		sscanf(buff, "%s", t);

		if (streq(t, "SURF"))
		{
			int flgs;

			if (get_tokens(buff, &tokc, tokv) != 2)
			{
				printf("SURF should be followed by one flags argument\n");
			}
			else
			{
				flgs = strtol(tokv[1], NULL, 0);
				s->flags = flgs;
			}
		}
		else
			if (streq(t, "mat"))
			{
				int mindx;

				sscanf(buff, "%s %d", t, &mindx);
				s->mat = mindx;
			}
			else
				if (streq(t, "refs"))
				{
					int num, n;
					int ind;
					float tx, ty;

					sscanf(buff, "%s %d", t, &num);

					s->num_vertref = num;
					s->vertref = (int *)malloc( num * sizeof(int));
					s->uvs = (ACUV *)malloc( num * sizeof(ACUV));

					for (n = 0; n < num; n++)
					{
						char buffer[4096];
						file.getline(buffer, 4096);
						sscanf(buffer, "%d %f %f\n", &ind, &tx, &ty); line++;
						s->vertref[n] = ind;
						s->uvs[n].u = tx;
						s->uvs[n].v = ty;
					}

					/** calc surface normal **/
					if (s->num_vertref >= 3)
						tri_calc_normal((ACPoint *)&ob->vertices[s->vertref[0]],
						(ACPoint *)&ob->vertices[s->vertref[1]],
						(ACPoint *)&ob->vertices[s->vertref[2]], (ACPoint *)&s->normal);

					return(s);
				}
				else
					printf("ignoring %s\n", t);

	}
	return(NULL);
}


static void ac3d_object_calc_vertex_normals(ACObject *ob)
{
	int s, v, vr;

	/** for each vertex in this object **/
	for (v = 0; v < ob->num_vert; v++)
	{
		ACNormal n = {0, 0, 0};
		int found = 0;

		/** go through each surface **/
		for (s = 0; s < ob->num_surf; s++)
		{
			ACSurface *surf = &ob->surfaces[s];

			/** check if this vertex is used in this surface **/
			/** if it is, use it to create an average normal **/
			for (vr = 0; vr < surf->num_vertref; vr++)
				if (surf->vertref[vr] == v)
				{
					n.x+=surf->normal.x;
					n.y+=surf->normal.y;
					n.z+=surf->normal.z;
					found++;
				}
		}
		if (found > 0)
		{
			n.x /= found;
			n.y /= found;
			n.z /= found;
		}
		ob->vertices[v].normal = n;
	}


}


static int string_to_objecttype(char *s)
{
	if (streq("world", s))
		return(AC3D_OBJECT_WORLD);
	if (streq("poly", s))
		return(AC3D_OBJECT_NORMAL);
	if (streq("group", s))
		return(AC3D_OBJECT_GROUP);
	if (streq("light", s))
		return(AC3D_OBJECT_LIGHT);
	return(AC3D_OBJECT_NORMAL);
}

static ACObject *ac3d_load_object(std::istream &file, ACObject *parent, ACMaterial *pPalette, int &iNumPalette, char *buff, int &line)
{
	char t[20];
	ACObject *ob = NULL;

	while (!file.eof())
	{

		file.getline(buff, 256);
		++line;
		//read_line(f, buff, line);

		sscanf(buff, "%s", t);

		if (streq(t, "MATERIAL"))
		{
			//float shi, tran;
			ACMaterial m;

			if (get_tokens(buff, &tokc, tokv) != 22)
			{
				printf("expected 21 params after \"MATERIAL\" - line %d\n", line);
			}
			else
			{
				m.name = STRING(tokv[1]);
				m.rgb.r = (float)atof(tokv[3]);
				m.rgb.g = (float)atof(tokv[4]);
				m.rgb.b = (float)atof(tokv[5]);
				m.rgb.a = 0.0f;

				m.ambient.r = (float)atof(tokv[7]);
				m.ambient.g = (float)atof(tokv[8]);
				m.ambient.b = (float)atof(tokv[9]);
				m.ambient.a = 0.0f;

				m.emissive.r = (float)atof(tokv[11]);
				m.emissive.g = (float)atof(tokv[12]);
				m.emissive.b = (float)atof(tokv[13]);
				m.emissive.a = 0.0f;

				m.specular.r = (float)atof(tokv[15]);
				m.specular.g = (float)atof(tokv[16]);
				m.specular.b = (float)atof(tokv[17]);
				m.specular.a = 0.0f;

				m.shininess = (float)atof(tokv[19]);
				m.transparency = (float)atof(tokv[21]);

				// values are not needed
				// shi = (float)atof(tokv[6]);
				// tran =  (float)atof(tokv[7]);

				pPalette[iNumPalette++] = m;

			}
		}
		else if (streq(t, "OBJECT"))
		{
			char type[20];
			char str[20];
			ob = new_object();

			sscanf(buff, "%s %s", str, type);

			ob->type = string_to_objecttype(type);
		}
		else if (streq(t, "data"))
		{
			if (get_tokens(buff, &tokc, tokv) != 2)
				printf("expected 'data <number>' at line %d\n", line);
			else
			{
				char *str;
				int len;

				len = atoi(tokv[1]);
				if (len > 0)
				{
					str = (char *)myalloc(len+1);
					file.getline(str, len);
					file.read(str, len+1);
					//fread(str, len, 1, f);
					str[len] = 0;
					ob->data = STRING(str);
					myfree(str);
				}
			}
		}
		else if (streq(t, "name"))
		{
			int numtok = get_tokens(buff, &tokc, tokv);
			if (numtok != 2)
			{
				printf("expected quoted name at line %d (got %d tokens)\n", line, numtok);
			}
			else
				ob->name = STRING(tokv[1]);
		}
		else if (streq(t, "crease"))
		{
			int numtok = get_tokens(buff, &tokc, tokv);
			if(numtok != 2)
			{
			}
			else
			{
				ob->crease = (float)atof(tokv[1]);
			}
		}
		else if (streq(t, "texture"))
		{
			if (get_tokens(buff, &tokc, tokv) != 2)
				printf("expected quoted texture name at line %d\n", line);

			else
			{
				ob->texture = STRING(tokv[1]);
			}
		}
		else if (streq(t, "texrep"))
		{
			if (get_tokens(buff, &tokc, tokv) != 3)
				printf("expected 'texrep <float> <float>' at line %d\n", line);
			else
			{
				ob->texture_repeat_x = (float)atof(tokv[1]);
				ob->texture_repeat_y = (float)atof(tokv[2]);
			}
		}
		else if (streq(t, "texoff"))
		{
			if (get_tokens(buff, &tokc, tokv) != 3)
				printf("expected 'texoff <float> <float>' at line %d\n", line);
			else
			{
				ob->texture_offset_x = (float)atof(tokv[1]);
				ob->texture_offset_y = (float)atof(tokv[2]);
			}
		}
		else if (streq(t, "rot"))
		{
			float r[9];
			char str2[5];
			int n;

			sscanf(buff, "%s %f %f %f %f %f %f %f %f %f", str2,
				&r[0], &r[1], &r[2], &r[3], &r[4], &r[5], &r[6], &r[7], &r[8] );

			for (n = 0; n < 9; n++)
				ob->matrix[n] = r[n];

		}
		else if (streq(t, "loc"))
		{
			char str[5];
			sscanf(buff, "%s %f %f %f", str,
				&ob->loc.x, &ob->loc.y, &ob->loc.z);
		}
		else if (streq(t, "url"))
		{
			if (get_tokens(buff, &tokc, tokv) != 2)
				printf("expected one arg to url at line %d (got %s)\n", line, tokv[0]);
			else
				ob->url = STRING(tokv[1]);
		}
		else if (streq(t, "numvert"))
		{
			int num, n;
			char str[10];

			sscanf(buff, "%s %d", str, &num);

			if (num > 0)
			{
				ob->num_vert = num;
				ob->vertices = (ACVertex *)myalloc(sizeof(ACVertex)*num);

				for (n = 0; n < num; n++)
				{
					ACVertex p;
					char buffer[4096];
					file.getline(buffer, 4096, '\n');
					//file.getline(buffer, 4096);

					sscanf(buffer, "%f %f %f", &p.x, &p.y, &p.z); line++;
					ob->vertices[n] = p;
				}

			}
		}
		else if (streq(t, "numsurf"))
		{
			int num, n;
			char str[10];

			sscanf(buff, "%s %d", str, &num);
			if (num > 0)
			{
				ob->num_surf = num;
				ob->surfaces = (ACSurface *)myalloc(sizeof(ACSurface) * num);

				for (n = 0; n < num; n++)
				{
					ACSurface *news = read_surface(file, &ob->surfaces[n], ob, buff, line);
					if (news == NULL)
					{
						printf("error whilst reading surface at line: %d\n", line);
						return(NULL);
					}

				}
			}
		}
		else if (streq(t, "kids")) /** 'kids' is the last token in an object **/
		{
			int num, n;

			sscanf(buff, "%s %d", t, &num);

			if (num != 0)
			{
				ob->kids = (ACObject **)myalloc(num * sizeof(ACObject *) );
				ob->num_kids = num;

				for (n = 0; n < num; n++)
				{
					ACObject *k = ac3d_load_object(file, ob, pPalette, iNumPalette, buff, line);

					if (k == NULL)
					{
						printf("error reading expected child object %d of %d at line: %d\n", n+1, num, line);
						return(ob);
					}
					else
						ob->kids[n] = k;
				}

			}
			return(ob);
		}

	}
	return(ob);

}


static void ac3d_calc_vertex_normals(ACObject *ob)
{
	int n;

	ac3d_object_calc_vertex_normals(ob);
	if (ob->num_kids)
		for (n = 0; n < ob->num_kids; n++)
			ac3d_calc_vertex_normals(ob->kids[n]);
}


static ACObject *ac3d_load_ac3d(std::istream &file, ACMaterial *pPalette, int &iNumPalette)
{
	ACObject *ret = NULL;
	char buff[256];
	int line=0;

	if (file.bad())
	{
		//printf("can't open %s\n", fname);
		return(NULL);
	}

	file.getline(buff, 256);
	//read_line(f, buff, line);

	if (strncmp(buff, "AC3D", 4))
	{
		printf("ac3d_load_ac is not a valid AC3D file.");
		return(0);
	}


	//startmatindex = num_palette;
	ret = ac3d_load_object(file, NULL, pPalette, iNumPalette, buff, line);
	//ac3d_calc_vertex_normals(ret);
	return(ret);
}

static void ac3d_dump(ACObject *ob)
{
	int n;

	printf("OBJECT name %s\nloc %f %f %f\nnum_vert %d\nnum_surf %d\n",
		ob->name, ob->loc.x, ob->loc.y, ob->loc.z, ob->num_vert, ob->num_surf);


	for (n=0; n < ob->num_vert; n++)
		printf("\tv %f %f %f\n", ob->vertices[n].x, ob->vertices[n].y, ob->vertices[n].z);

	for (n=0; n < ob->num_surf; n++)
	{
		ACSurface *s = &ob->surfaces[n];
		printf("surface %d, %d refs, mat %d\n", n, s->num_vertref, s->mat);
	}

	if (ob->num_kids)
		for (n = 0; n < ob->num_kids; n++)
			ac3d_dump(ob->kids[n]);

}

/*******************************
 *public
 *******************************/

//----------------------------
// Function name: read
//----------------------------
//
//Parameters:
//p: Scene &image, const char *fileName
//GlobalVars:
//g:
//Returns:
//r:bool
// Caution
//c:
//Assumations:
//a:
//Describtions:
//d: read the image from the given file
//SeeAlso:
//s:
//
//------------------------------

/******************************
 *protected
 ******************************/


/******************************
 *private
 ******************************/


/***************************
 *instance methodes
 ***************************/

static OSG::GeometryPtr createGeometry(ACObject *pObj, ACMaterial *pMatTable,
									   std::map<std::string, OSG::ImagePtr> &mpTextures)
{

	if(pObj->num_surf == 0 && pObj->num_vert == 0)
	{
		vstr::warnp() << "[AC3DLoader]: Degenerated object ["
            << (pObj->name ? pObj->name : "<no-name>")
			<< "] (no vertices, no surfaces, skipping)" << std::endl;
		return NullFC;
	}

	std::map<unsigned int, unsigned int> colMap;
	std::set<unsigned int> matSet;

	GeoPositionsPtr coordPtr      = GeoPositions3f::create();
	GeoTexCoordsPtr texCoordPtr  = GeoTexCoords2f::create();
	GeoNormalsPtr   normalPtr   = GeoNormals3f::create();

	//GeoColors4fPtr  colors = GeoColors4f::create();

	GeoPTypesPtr type = GeoPTypesUI8::create();
	GeoPLengthsPtr lens = GeoPLengthsUI32::create();

	// index mapping, static setup
	GeoIndicesUI32Ptr indices = GeoIndicesUI32::create();

	// copy coordinates
	// copy normals
	beginEditCP(coordPtr);
	beginEditCP(normalPtr);
	for(int i=0; i < pObj->num_vert; ++i)
	{
		coordPtr->addValue(Pnt3f(pObj->vertices[i].x,
			pObj->vertices[i].y,
			pObj->vertices[i].z));
		normalPtr->addValue(Vec3f(pObj->vertices[i].normal.x,
			pObj->vertices[i].normal.y,
			pObj->vertices[i].normal.z));
	}
	endEditCP(coordPtr);
	endEditCP(normalPtr);


	// surface processing
	beginEditCP(texCoordPtr);
	beginEditCP(type, GeoPTypesUI8::GeoPropDataFieldMask);
	beginEditCP(lens, GeoPLengthsUI32::GeoPropDataFieldMask);

	int smooth=0;
	int twosided=0;

	if(pObj->num_surf == 0)
	{
		// collection of points
		type->addValue(GL_POINTS);
		lens->addValue(pObj->num_vert);
		beginEditCP(indices, GeoIndicesUI32::GeoPropDataFieldMask);
		texCoordPtr->addValue(Vec2f(0,0));

		//colors->addValue(Color3f( 0, 1, 0 ) );
		//colors->getField().push_back(Color3f( 0, 1, 0 ));

		for(int k=0; k < pObj->num_vert; ++k)
		{
			indices->push_back(k); // coord index
			indices->push_back(k); // normal index
			indices->push_back(0); // tex-coord index
			indices->push_back(0); // color index

		}
		endEditCP  (indices, GeoIndicesUI32::GeoPropDataFieldMask);

	}
	else
	{

		for(int j=0; j < pObj->num_surf; ++j)
		{
			if((pObj->surfaces[j].flags & AC3D_SURFACE_TYPE_CLOSEDLINE) == AC3D_SURFACE_TYPE_CLOSEDLINE)
			{
				type->addValue(GL_LINE_LOOP);
				lens->addValue(pObj->surfaces[j].num_vertref);
			}
			else if((pObj->surfaces[j].flags & AC3D_SURFACE_TYPE_LINE) == AC3D_SURFACE_TYPE_LINE)
			{
				type->addValue(GL_LINES);
				lens->addValue(pObj->surfaces[j].num_vertref);
			}
			else if((pObj->surfaces[j].flags & AC3D_SURFACE_TYPE_POLYGON) == AC3D_SURFACE_TYPE_POLYGON)
			{
				// try to be as precise as possible here
				switch(pObj->surfaces[j].num_vertref)
				{
				case 3:
					type->addValue(GL_TRIANGLES);
					break;
				case 4:
					type->addValue(GL_QUADS);
					break;
				default:
					type->addValue(GL_POLYGON);
					break;
				}
				// add number of vertices
				lens->addValue(pObj->surfaces[j].num_vertref);
			}
			else
			{
				vstr::warnp() << "[AC3DLoader]: encountered unknown surface type (flag=" 
					<< pObj->surfaces[j].flags << ")>" << std::endl;
			}

			smooth = smooth ? smooth : (pObj->surfaces[j].flags & AC3D_SURFACE_SHADED);
			twosided = twosided ? twosided : (pObj->surfaces[j].flags & AC3D_SURFACE_TWOSIDED);

			matSet.insert(pObj->surfaces[j].mat);

			beginEditCP(indices, GeoIndicesUI32::GeoPropDataFieldMask);
			for(int k=0; k < pObj->surfaces[j].num_vertref; ++k)
			{
				texCoordPtr->addValue(Vec2f(pObj->surfaces[j].uvs[k].u*float(pObj->texture_repeat_x)+pObj->texture_offset_x,
					pObj->surfaces[j].uvs[k].v*float(pObj->texture_repeat_y)+pObj->texture_offset_y));
				int cIdx = pObj->surfaces[j].vertref[k];
				indices->push_back(cIdx); // coord index
				indices->push_back(cIdx); // normal index
				indices->push_back(texCoordPtr->getSize()-1); // tex-coord index

				std::map<unsigned int, unsigned int>::const_iterator cit = colMap.find(pObj->surfaces[j].mat);
				if(cit == colMap.end())
				{
					// insert new color lookup
					//colors->addValue(Color3f( pMatTable[pObj->surfaces[j].mat].rgb.r,
					//			  pMatTable[pObj->surfaces[j].mat].rgb.g,
					//			  pMatTable[pObj->surfaces[j].mat].rgb.b) );
					//colors->getField().push_back(Color4f( pMatTable[pObj->surfaces[j].mat].rgb.r,
					//	pMatTable[pObj->surfaces[j].mat].rgb.g,
					//	pMatTable[pObj->surfaces[j].mat].rgb.b,
					//	1.0f - pMatTable[pObj->surfaces[j].mat].transparency) );
					//colMap[pObj->surfaces[j].mat] = colors->getSize()-1;
				}

				//cit = colMap.find(pObj->surfaces[j].mat);
				//indices->push_back((*cit).second);
			}
			endEditCP  (indices, GeoIndicesUI32::GeoPropDataFieldMask);
		}
	}

	endEditCP(texCoordPtr);
	endEditCP(lens, GeoPLengthsUI32::GeoPropDataFieldMask);
	endEditCP(type, GeoPTypesUI8::GeoPropDataFieldMask);


	SimpleMaterialPtr sm = NullFC;

	if(pObj->texture != NULL)
	{
		SimpleTexturedMaterialPtr stm =	SimpleTexturedMaterial::create();
		beginEditCP(stm);
		{
			stm->setEnvMode(GL_MODULATE);
			stm->setEnvMap(GL_FALSE);
			stm->setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
			stm->setMagFilter(GL_LINEAR);


			ImagePtr texture = NullFC;

			std::map<std::string, ImagePtr>::iterator cit = mpTextures.find(pObj->texture);
			if(cit == mpTextures.end())
			{
				// load texture
				texture = Image::create();
				beginEditCP(texture);
				texture->read(pObj->texture);
				endEditCP(texture);
				if(texture != NullFC)
				{
					mpTextures.insert(std::map<std::string,ImagePtr>::value_type(pObj->texture,texture));
				}
				else
				{
					vstr::warnp() << "[AC3DLoader]:  Could not read texture ["
						<< pObj->texture << "]" << std::endl;
				}

			}
			else
			{
				texture = (*cit).second;
			}

			if(texture != NullFC)
			{
				stm->setImage(texture);
			}

			sm = stm;
		}
		endEditCP(stm);
	}
	else
	{
		sm = SimpleMaterial::create();
	}

	PolygonChunkPtr p = PolygonChunk::create();
	beginEditCP(p);

	// looks bad and produces artifacts with blending
	// (at least with current nvidia drivers)
	//if(smooth == AC3D_SURFACE_SHADED)
	//	p->setSmooth(GL_TRUE);
	//else
	//	p->setSmooth(GL_FALSE);
	//

	if(twosided == AC3D_SURFACE_TWOSIDED)
		p->setCullFace(GL_NONE);
	else
		p->setCullFace(GL_BACK);

	//p->setFrontMode(GL_LINE);
	//p->setBackMode(GL_FILL);
	endEditCP(p);

	if(matSet.empty())
	{
		beginEditCP(sm);
		sm->setAmbient(Color3f(0.2f, 0.2f, 0.2f));
		sm->setDiffuse(Color3f(0.55f, 0.55f, 0.55f));
		sm->setSpecular(Color3f(0.7f, 0.7f, 0.7f));
		sm->setEmission(Color3f(0, 0, 0));
		sm->setShininess(128 * 0.25f);
		sm->setLit(GL_TRUE);
		sm->addChunk(p);
		endEditCP(sm);
		vstr::warnp() << "[AC3DLoader]: No material set"
				<< " - using default (white plastic)." << std::endl;
	}
	else
	{
		int nMatIdx = *matSet.begin();

		if(matSet.size() != 1)
		{
			vstr::warnp() << "[AC3DLoader]: More than one material in geometry ["
						<< pObj->name << "] -- assigning first material" << std::endl;
		}

		ACMaterial mat = pMatTable[nMatIdx];

		beginEditCP(sm);
		sm->setAmbient(Color3f(mat.ambient.r, mat.ambient.g, mat.ambient.b));
		sm->setDiffuse(Color3f(mat.rgb.r, mat.rgb.g, mat.rgb.b));
		sm->setSpecular(Color3f(mat.specular.r, mat.specular.g, mat.specular.b));
		sm->setEmission(Color3f(mat.emissive.r, mat.emissive.g, mat.emissive.b));
		sm->setShininess(mat.shininess);
		sm->setTransparency(mat.transparency);
		sm->setLit(GL_TRUE);
		sm->addChunk(p);
		endEditCP(sm);

	}

	GeometryPtr geo = Geometry::create();
	beginEditCP(geo, Geometry::TypesFieldMask      |
		Geometry::LengthsFieldMask    |
		Geometry::IndicesFieldMask    |
		Geometry::PositionsFieldMask  |
		//Geometry::ColorsFieldMask     |
		Geometry::NormalsFieldMask    |
		Geometry::TexCoords2FieldMask |
		Geometry::MaterialFieldMask );

	geo->setTypes    ( type      );
	//std::cout << "types of [ " << pObj->name << "] has length: " << (*type).size()
	//	<< " and data @" << (void*)(*type).getData() << std::endl;

	geo->setLengths  ( lens      );
	geo->setIndices  ( indices   );

	// index mapping first
	geo->getIndexMapping().push_back( Geometry::MapPosition  );
	geo->getIndexMapping().push_back( Geometry::MapNormal    );

	geo->getIndexMapping().push_back( Geometry::MapTexCoords );

	//geo->getIndexMapping().push_back( Geometry::MapColor     );


	geo->setPositions(coordPtr);
	geo->setNormals(normalPtr );

	geo->setTexCoords(texCoordPtr);
	//geo->setColors(colors);

	geo->setMaterial(sm);
	endEditCP(geo, Geometry::TypesFieldMask      |
		Geometry::LengthsFieldMask    |
		Geometry::IndicesFieldMask    |
		Geometry::PositionsFieldMask  |
		//Geometry::ColorsFieldMask     |
		Geometry::NormalsFieldMask    |
		Geometry::TexCoords2FieldMask |
		Geometry::MaterialFieldMask );

	createSharedIndex(geo);


	if(smooth == AC3D_SURFACE_SHADED)
		calcVertexNormals(geo, deg2rad(pObj->crease));
	else
		calcFaceNormals(geo);

	return geo;
}

static NodePtr createKid(ACObject *pParentObj, ACMaterial *pMatTab,
						 std::map<std::string, ImagePtr> &mpTextures)
{
	// stop recursion
	if(pParentObj->type == AC3D_OBJECT_NORMAL
		// can happen with some importers: GROUP type obj with no kids but geometry attached
		|| ((pParentObj->type == AC3D_OBJECT_GROUP && pParentObj->kids == 0) && (pParentObj->num_surf > 0) ))
	{
		// this is a geometry
		//std::cout << "Encounter normal object [" << (pParentObj->name ? pParentObj->name : std::string("<noname>")) << "]\n";
		GeometryPtr pGeo = createGeometry(pParentObj, pMatTab,mpTextures);
		if(pGeo != NullFC)
		{
			if( pParentObj->loc.x || pParentObj->loc.y || pParentObj->loc.z || (pParentObj->data  && (std::string(pParentObj->data)=="transform")) )
			{
				NodePtr gNode = Node::create();
				TransformPtr trans = Transform::create();

				std::string sName = pParentObj->name ? std::string(pParentObj->name) + std::string(" transform")
					: "kid <transform>";
				setName(gNode, sName);

				Matrix m;
				m.setTranslate(pParentObj->loc.x, pParentObj->loc.y,pParentObj->loc.z);
				for(int i=0;i <3;++i)
					for(int j=0;j<3;++j)
						m[i][j] = pParentObj->matrix[i*3+j];

				beginEditCP(trans, Transform::MatrixFieldMask);
				trans->setMatrix(m);
				endEditCP  (trans, Transform::MatrixFieldMask);

				NodePtr geo = Node::create();
				beginEditCP(geo);
				geo->setCore(pGeo);
				endEditCP(geo);


				beginEditCP(gNode);
				setName(geo, (pParentObj->name ? pParentObj->name : "<none>"));
				gNode->setCore(trans);
				gNode->addChild(geo);
				endEditCP(gNode);
				return gNode;
			}
			else
			{
				NodePtr geo = Node::create();
				beginEditCP(geo);
					setName(geo, (pParentObj->name ? pParentObj->name : "<none>"));
					geo->setCore(pGeo);
				endEditCP(geo);
				return geo;
			}
		}
		else
		{
			//std::cout << "Could not createGeometry ["
			//	      << (pParentObj->name ? pParentObj->name : std::string("<noname>")) << "]\n";
			return NullFC;
		}
	}
	else if(pParentObj->type == AC3D_OBJECT_LIGHT)
	{
		//std::cerr << "Skipping light node.\n";
	}
	else if(pParentObj->type == AC3D_OBJECT_GROUP
		||pParentObj->type == AC3D_OBJECT_WORLD)
	{

		//std::cout << "Encounter group [" << (pParentObj->name ? pParentObj->name : std::string("<noname>")) << "]\n";
		// create local parent node

		NodePtr gNode = OSG::NullFC;
		if( pParentObj->loc.x || pParentObj->loc.y || pParentObj->loc.z )
		{
			gNode = Node::create();
			TransformPtr trans = Transform::create();
			Matrix m;
			m.setTranslate(pParentObj->loc.x, pParentObj->loc.y,pParentObj->loc.z);
			for(int i=0;i <3;++i)
				for(int j=0;j<3;++j)
					m[i][j] = pParentObj->matrix[i*3+j];

			beginEditCP(trans, Transform::MatrixFieldMask);
			trans->setMatrix(m);
			endEditCP  (trans, Transform::MatrixFieldMask);


			beginEditCP(gNode);
			gNode->setCore( trans );
			endEditCP(gNode);


			setName(gNode, (pParentObj->name ?
				(std::string(pParentObj->name)+std::string(" transform")).c_str() : "<none> transform"));
		}
		else
		{
			if( pParentObj->num_kids > 0)
			{
				gNode = Node::create();
				// create group only
				GroupPtr group = Group::create();
				setName(group, (pParentObj->name ? (std::string(pParentObj->name)+std::string(" group")).c_str() : "<none> group"));
				beginEditCP(gNode);
				gNode->setCore( group );
				endEditCP(gNode);
			}
		}

		if(pParentObj->num_kids > 0 )
		{
			for(int i=0; i < pParentObj->num_kids; ++i)
			{
				NodePtr child = createKid(pParentObj->kids[i], pMatTab, mpTextures); // recurse
				if(child != NullFC)
					gNode->addChild(child);
			}
			return gNode;
		}
		//else
			//return createKid(pParentObj->kids[0], pMatTab, mpTextures); // recurse
	}

	return NullFC;
}


/***************************
 *public
 ***************************/
NodePtr OSGAC3DSceneFileType::read(std::istream &is,
				   const Char8 *fileNameOrExtension) const
{
/* there is no setMode() method for std::basic_istream, only a setmode()
   method for std::basic_ifstream. temporarily commented out to get HEAD
   compiled on linux, gcc 4.1
   aren't basic_iostreams (without the 'f') always in binary mode?
 */
//#if !defined(WIN32)
//	is.setMode(std::ios::binary);
//#endif


	ACMaterial vecPalette[256];
	int iNumMaterials = 0;

	ACObject *world = ac3d_load_ac3d(is, &vecPalette[0], iNumMaterials);
	if(world)
	{
		//ac3d_dump(world);

		std::map<std::string, ImagePtr> mpTextures;
		NodePtr gScene = createKid(world, &vecPalette[0],
			mpTextures);
		ac3d_object_free(world);

		return gScene;

		//return gScene;
	}

	return NullFC;
}


/**constructors & destructors**/


//----------------------------
// Function name: OBJSceneFileType
//----------------------------
//
//Parameters:
//p: const char *suffixArray[], UInit16 suffixByteCount
//GlobalVars:
//g:
//Returns:
//r:
// Caution
//c:
//Assumations:
//a:
//Describtions:
//d: Default Constructor
//SeeAlso:
//s:
//
//------------------------------

OSGAC3DSceneFileType::OSGAC3DSceneFileType(const Char8  *suffixArray[],
					   UInt16  suffixByteCount,
					   bool    override,
					   UInt32  overridePriority,
					   UInt32  flags) :
	SceneFileType(suffixArray,
				  suffixByteCount,
				  override,
				  overridePriority,
				  flags)
{
}

//----------------------------
// Function name: OBJSceneFileType
//----------------------------
//
//Parameters:
//p: const OBJSceneFileType &obj
//GlobalVars:
//g:
//Returns:
//r:
// Caution
//c:
//Assumations:
//a:
//Describtions:
//d: Copy Constructor
//SeeAlso:
//s:
//
//------------------------------

OSGAC3DSceneFileType::OSGAC3DSceneFileType(const OSGAC3DSceneFileType &obj) :
	SceneFileType(obj)
{
}

//----------------------------
// Function name: ~OBJSceneFileType
//----------------------------
//
//Parameters:
//p: void
//GlobalVars:
//g:
//Returns:
//r:
// Caution
//c:
//Assumations:
//a:
//Describtions:
//d: Destructor
//SeeAlso:
//s:
//
//------------------------------

OSGAC3DSceneFileType &OSGAC3DSceneFileType::the(void)
{
	return _the;
}

OSGAC3DSceneFileType::~OSGAC3DSceneFileType(void)
{
	return;
}

const Char8 *OSGAC3DSceneFileType::getName(void) const
{
	return "AC3D GEOMETRY";
}


/*------------access----------------*/

/*------------properies-------------*/

/*------------your Category---------*/

/*------------Operators-------------*/

/****************************
 *protected
 ****************************/


/****************************
 *private
 ****************************/


/*-------------------------------------------------------------------------*/
/*                              cvs id's                                   */

#ifdef __sgi
#pragma set woff 1174
#endif

#ifdef OSG_LINUX_ICC
#pragma warning( disable : 177 )
#endif

//namespace
//{
//	static Char8 cvsid_cpp[] = "@(#)$Id$";
//	static Char8 cvsid_hpp[] = OSGAC3DSCENEFILETYPE_HEADER_CVSID;
//}

