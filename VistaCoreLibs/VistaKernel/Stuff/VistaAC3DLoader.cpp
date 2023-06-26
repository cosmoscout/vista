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

#if defined(WIN32)
#pragma warning(disable : 4996)
#endif

#include "VistaAC3DLoader.h"
//#include <VistaKernel/Stuff/VistaAC3DLowLevelLoad.h>

#include <VistaAspects/VistaConversion.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamUtils.h>
#include <VistaTools/VistaFileSystemDirectory.h>

#include <VistaKernel/GraphicsManager/VistaGeomNode.h>
#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaGroupNode.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLNode.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
//#include <VistaKernel/GraphicsManager/VistaSphere.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLPolyLine.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <string>
using namespace std;

typedef struct ACPoint_t {
  float x, y, z;
} ACPoint;

typedef struct ACNormal_t {
  float x, y, z;
} ACNormal;

typedef struct ACVertex_t {
  float    x, y, z;
  ACNormal normal;
} ACVertex;

typedef struct ACUV_t {
  float u, v;
} ACUV;

typedef struct ACSurface_t {
  int*     vertref;
  ACUV*    uvs;
  int      num_vertref;
  ACNormal normal;
  int      flags;
  int      mat;
} ACSurface;

typedef struct ACObject_t {
  ACPoint   loc;
  char*     name;
  char*     data;
  char*     url;
  ACVertex* vertices;
  int       num_vert;

  ACSurface* surfaces;
  int        num_surf;
  float      texture_repeat_x, texture_repeat_y;
  float      texture_offset_x, texture_offset_y;

  int                 num_kids;
  struct ACObject_t** kids;
  float               matrix[9];
  int                 type;
  char*               texture;
  float               crease;
} ACObject;

typedef struct ACCol_t {
  float r, g, b, a;
} ACCol;

typedef struct Material_t {
  ACCol rgb; /* diffuse **/
  ACCol ambient;
  ACCol specular;
  ACCol emissive;
  float shininess;
  float transparency;
  char* name;
} ACMaterial;

#define AC3D_OBJECT_WORLD 999
#define AC3D_OBJECT_NORMAL 0
#define AC3D_OBJECT_GROUP 1
#define AC3D_OBJECT_LIGHT 2

#define AC3D_SURFACE_SHADED (1 << 4)
#define AC3D_SURFACE_TWOSIDED (1 << 5)

#define AC3D_SURFACE_TYPE_POLYGON (0)
#define AC3D_SURFACE_TYPE_CLOSEDLINE (1)
#define AC3D_SURFACE_TYPE_LINE (2)

#ifdef WIN32
#if defined _AC3DLOAD_DLLEXPORT
#define Prototype __declspec(dllexport)
#elif defined _AC3DLOAD_DLLIMPORT
#define Prototype __declspec(dllimport)
#else
// define empty macro
#define Prototype
#endif // DLL binding
#else  // non win32 omit prototype define
#define Prototype
#endif

static Prototype ACObject* ac3d_load_ac3d(
    const char* filename, ACMaterial* pPalette, int& iNumPalette);
static Prototype void ac3d_object_free(ACObject* ob);

#define Boolean int

#ifndef TRUE
#define FALSE (0)
#define TRUE (!FALSE)
#endif

#define STRING(s) (char*)(strcpy((char*)myalloc(strlen(s) + 1), s))
#define streq(a, b) (!strcmp(a, b))
#define myalloc malloc
#define myfree free

static Boolean read_line(FILE* f, char* buff, int& line) {
  fgets(buff, 255, f);
  line++;
  return (TRUE);
}

static int   tokc = 0;
static char* tokv[30];

static Prototype int get_tokens(char* s, int* argc, char* argv[])
/** bung '\0' chars at the end of tokens and set up the array (tokv) and count (tokc)
        like argv argc **/
{
  char* p = s;
  char* st;
  char  c;
  // int n;
  int tc;

  tc = 0;
  while ((c = *p) != 0) {
    if ((c != ' ') && (c != '\t') && (c != '\n') && (c != 13)) {
      if (c == '"') {
        c  = *p++;
        st = p;
        while ((c = *p) && ((c != '"') && (c != '\n') && (c != 13))) {
          if (c == '\\')
            strcpy(p, p + 1);
          p++;
        }
        *p         = 0;
        argv[tc++] = st;
      } else {
        st = p;
        while ((c = *p) && ((c != ' ') && (c != '\t') && (c != '\n') && (c != 13)))
          p++;
        *p         = 0;
        argv[tc++] = st;
      }
    }
    p++;
  }

  *argc = tc;
  return (tc);
}

static ACObject* new_object() {
  ACObject* ob = (ACObject*)myalloc(sizeof(ACObject));

  ob->loc.x = ob->loc.y = ob->loc.z = 0.0;
  ob->name = ob->url   = NULL;
  ob->data             = NULL;
  ob->vertices         = NULL;
  ob->num_vert         = 0;
  ob->surfaces         = NULL;
  ob->num_surf         = 0;
  ob->texture          = NULL;
  ob->texture_repeat_x = ob->texture_repeat_y = 1.0;
  ob->texture_offset_x = ob->texture_offset_y = 0.0;
  ob->kids                                    = NULL;
  ob->num_kids                                = 0;
  ob->matrix[0]                               = 1;
  ob->matrix[1]                               = 0;
  ob->matrix[2]                               = 0;
  ob->matrix[3]                               = 0;
  ob->matrix[4]                               = 1;
  ob->matrix[5]                               = 0;
  ob->matrix[6]                               = 0;
  ob->matrix[7]                               = 0;
  ob->matrix[8]                               = 1;
  return (ob);
}

static void ac3d_object_free(ACObject* ob) {
  int n, s;

  for (n = 0; n < ob->num_kids; n++)
    ac3d_object_free(ob->kids[n]);

  if (ob->kids)
    free(ob->kids);

  if (ob->vertices)
    free(ob->vertices);

  for (s = 0; s < ob->num_surf; s++) {
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
  if (ob->texture)
    free(ob->texture);

  free(ob);
}

static void init_surface(ACSurface* s) {
  s->vertref     = NULL;
  s->uvs         = NULL;
  s->num_vertref = 0;
  s->flags       = 0;
  s->mat         = 0;
  s->normal.x    = 0.0;
  s->normal.z    = 0.0;
  s->normal.z    = 0.0;
}

static void tri_calc_normal(ACPoint* v1, ACPoint* v2, ACPoint* v3, ACPoint* n) {
  double len;

  n->x = (v2->y - v1->y) * (v3->z - v1->z) - (v3->y - v1->y) * (v2->z - v1->z);
  n->y = (v2->z - v1->z) * (v3->x - v1->x) - (v3->z - v1->z) * (v2->x - v1->x);
  n->z = (v2->x - v1->x) * (v3->y - v1->y) - (v3->x - v1->x) * (v2->y - v1->y);
  len  = sqrt(n->x * n->x + n->y * n->y + n->z * n->z);

  if (len > 0) {
    n->x /= (float)len;
    n->y /= (float)len;
    n->z /= (float)len;
  }
}

static ACSurface* read_surface(FILE* f, ACSurface* s, ACObject* ob, char* buff, int& line) {
  char t[20];

  init_surface(s);

  while (!feof(f)) {
    read_line(f, buff, line);
    sscanf(buff, "%s", t);

    if (streq(t, "SURF")) {
      int flgs;

      if (get_tokens(buff, &tokc, tokv) != 2) {
        printf("SURF should be followed by one flags argument\n");
      } else {
        flgs     = strtol(tokv[1], NULL, 0);
        s->flags = flgs;
      }
    } else if (streq(t, "mat")) {
      int mindx;

      sscanf(buff, "%s %d", t, &mindx);
      s->mat = mindx;
    } else if (streq(t, "refs")) {
      int   num, n;
      int   ind;
      float tx, ty;

      sscanf(buff, "%s %d", t, &num);

      s->num_vertref = num;
      s->vertref     = (int*)malloc(num * sizeof(int));
      s->uvs         = (ACUV*)malloc(num * sizeof(ACUV));

      for (n = 0; n < num; n++) {
        fscanf(f, "%d %f %f\n", &ind, &tx, &ty);
        line++;
        s->vertref[n] = ind;
        s->uvs[n].u   = tx;
        s->uvs[n].v   = ty;
      }

      /** calc surface normal **/
      if (s->num_vertref >= 3)
        tri_calc_normal((ACPoint*)&ob->vertices[s->vertref[0]],
            (ACPoint*)&ob->vertices[s->vertref[1]], (ACPoint*)&ob->vertices[s->vertref[2]],
            (ACPoint*)&s->normal);

      return (s);
    } else
      printf("ignoring %s\n", t);
  }
  return (NULL);
}

static void ac3d_object_calc_vertex_normals(ACObject* ob) {
  int s, v, vr;

  /** for each vertex in this object **/
  for (v = 0; v < ob->num_vert; v++) {
    ACNormal n     = {0, 0, 0};
    int      found = 0;

    /** go through each surface **/
    for (s = 0; s < ob->num_surf; s++) {
      ACSurface* surf = &ob->surfaces[s];

      /** check if this vertex is used in this surface **/
      /** if it is, use it to create an average normal **/
      for (vr = 0; vr < surf->num_vertref; vr++)
        if (surf->vertref[vr] == v) {
          n.x += surf->normal.x;
          n.y += surf->normal.y;
          n.z += surf->normal.z;
          found++;
        }
    }
    if (found > 0) {
      n.x /= found;
      n.y /= found;
      n.z /= found;
    }
    ob->vertices[v].normal = n;
  }
}

static int string_to_objecttype(char* s) {
  if (streq("world", s))
    return (AC3D_OBJECT_WORLD);
  if (streq("poly", s))
    return (AC3D_OBJECT_NORMAL);
  if (streq("group", s))
    return (AC3D_OBJECT_GROUP);
  if (streq("light", s))
    return (AC3D_OBJECT_LIGHT);
  return (AC3D_OBJECT_NORMAL);
}

static ACObject* ac3d_load_object(
    FILE* f, ACObject* parent, ACMaterial* pPalette, int& iNumPalette, char* buff, int& line) {
  char      t[20];
  ACObject* ob = NULL;

  while (!feof(f)) {
    read_line(f, buff, line);

    sscanf(buff, "%s", t);

    if (streq(t, "MATERIAL")) {
      ACMaterial m;

      if (get_tokens(buff, &tokc, tokv) != 22) {
        printf("expected 21 params after \"MATERIAL\" - line %d\n", line);
      } else {
        m.name  = STRING(tokv[1]);
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

        m.shininess    = (float)atof(tokv[19]);
        m.transparency = (float)atof(tokv[21]);

        pPalette[iNumPalette++] = m;
      }
    } else if (streq(t, "OBJECT")) {
      char type[20];
      char str[20];
      ob = new_object();

      sscanf(buff, "%s %s", str, type);

      ob->type = string_to_objecttype(type);
    } else if (streq(t, "data")) {
      if (get_tokens(buff, &tokc, tokv) != 2)
        printf("expected 'data <number>' at line %d\n", line);
      else {
        char* str;
        int   len;

        len = atoi(tokv[1]);
        if (len > 0) {
          str = (char*)myalloc(len + 1);
          fread(str, len, 1, f);
          str[len] = 0;
          fscanf(f, "\n");
          line++;
          ob->data = STRING(str);
          myfree(str);
        }
      }
    } else if (streq(t, "name")) {
      int numtok = get_tokens(buff, &tokc, tokv);
      if (numtok != 2) {
        printf("expected quoted name at line %d (got %d tokens)\n", line, numtok);
      } else
        ob->name = STRING(tokv[1]);
    } else if (streq(t, "texture")) {
      if (get_tokens(buff, &tokc, tokv) != 2)
        printf("expected quoted texture name at line %d\n", line);

      else {
        ob->texture = STRING(tokv[1]);
      }
    } else if (streq(t, "crease")) {
      char str[10];

      sscanf(buff, "%s %f", str, &ob->crease);
    } else if (streq(t, "texrep")) {
      if (get_tokens(buff, &tokc, tokv) != 3)
        printf("expected 'texrep <float> <float>' at line %d\n", line);
      else {
        ob->texture_repeat_x = (float)atof(tokv[1]);
        ob->texture_repeat_y = (float)atof(tokv[2]);
      }
    } else if (streq(t, "texoff")) {
      if (get_tokens(buff, &tokc, tokv) != 3)
        printf("expected 'texoff <float> <float>' at line %d\n", line);
      else {
        ob->texture_offset_x = (float)atof(tokv[1]);
        ob->texture_offset_y = (float)atof(tokv[2]);
      }
    } else if (streq(t, "rot")) {
      float r[9];
      char  str2[5];
      int   n;

      sscanf(buff, "%s %f %f %f %f %f %f %f %f %f", str2, &r[0], &r[1], &r[2], &r[3], &r[4], &r[5],
          &r[6], &r[7], &r[8]);

      for (n = 0; n < 9; n++)
        ob->matrix[n] = r[n];

    } else if (streq(t, "loc")) {
      char str[5];
      sscanf(buff, "%s %f %f %f", str, &ob->loc.x, &ob->loc.y, &ob->loc.z);
    } else if (streq(t, "url")) {
      if (get_tokens(buff, &tokc, tokv) != 2)
        printf("expected one arg to url at line %d (got %s)\n", line, tokv[0]);
      else
        ob->url = STRING(tokv[1]);
    } else if (streq(t, "numvert")) {
      int  num, n;
      char str[10];

      sscanf(buff, "%s %d", str, &num);

      if (num > 0) {
        ob->num_vert = num;
        ob->vertices = (ACVertex*)myalloc(sizeof(ACVertex) * num);

        for (n = 0; n < num; n++) {
          ACVertex p;
          fscanf(f, "%f %f %f\n", &p.x, &p.y, &p.z);
          line++;
          ob->vertices[n] = p;
        }
      }
    } else if (streq(t, "numsurf")) {
      int  num, n;
      char str[10];

      sscanf(buff, "%s %d", str, &num);
      if (num > 0) {
        ob->num_surf = num;
        ob->surfaces = (ACSurface*)myalloc(sizeof(ACSurface) * num);

        for (n = 0; n < num; n++) {
          ACSurface* news = read_surface(f, &ob->surfaces[n], ob, buff, line);
          if (news == NULL) {
            printf("error whilst reading surface at line: %d\n", line);
            return (NULL);
          }
        }
      }
    } else if (streq(t, "kids")) /** 'kids' is the last token in an object **/
    {
      int num, n;

      sscanf(buff, "%s %d", t, &num);

      if (num != 0) {
        ob->kids     = (ACObject**)myalloc(num * sizeof(ACObject*));
        ob->num_kids = num;

        for (n = 0; n < num; n++) {
          ACObject* k = ac3d_load_object(f, ob, pPalette, iNumPalette, buff, line);

          if (k == NULL) {
            printf("error reading expected child object %d of %d at line: %d\n", n + 1, num, line);
            return (ob);
          } else
            ob->kids[n] = k;
        }
      }
      return (ob);
    }
  }
  return (ob);
}

static void ac3d_calc_vertex_normals(ACObject* ob) {
  int n;

  ac3d_object_calc_vertex_normals(ob);
  if (ob->num_kids)
    for (n = 0; n < ob->num_kids; n++)
      ac3d_calc_vertex_normals(ob->kids[n]);
}

static ACObject* ac3d_load_ac3d(const char* fname, ACMaterial* pPalette, int& iNumPalette) {
  FILE*     f   = fopen(fname, "r");
  ACObject* ret = NULL;
  char      buff[256];
  int       line = 0;

  if (f == NULL) {
    printf("can't open %s\n", fname);
    return (NULL);
  }

  read_line(f, buff, line);

  if (strncmp(buff, "AC3D", 4)) {
    printf("ac3d_load_ac '%s' is not a valid AC3D file.", fname);
    fclose(f);
    return (0);
  }

  // startmatindex = num_palette;
  ret = ac3d_load_object(f, NULL, pPalette, iNumPalette, buff, line);

  fclose(f);

  //    ac3d_calc_vertex_normals(ret);

  return (ret);
}

static void ac3d_dump(ACObject* ob) {
  int n;

  printf("OBJECT name %s\nloc %f %f %f\nnum_vert %d\nnum_surf %d\n", ob->name, ob->loc.x, ob->loc.y,
      ob->loc.z, ob->num_vert, ob->num_surf);

  for (n = 0; n < ob->num_vert; n++)
    printf("\tv %f %f %f\n", ob->vertices[n].x, ob->vertices[n].y, ob->vertices[n].z);

  for (n = 0; n < ob->num_surf; n++) {
    ACSurface* s = &ob->surfaces[n];
    printf("surface %d, %d refs, mat %d\n", n, s->num_vertref, s->mat);
  }

  if (ob->num_kids)
    for (n = 0; n < ob->num_kids; n++)
      ac3d_dump(ob->kids[n]);
}

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

// PROTOTYPES

static IVistaNode* ReadPolyLineKid(const std::string& sFileName, VistaSceneGraph* pSG,
    VistaGroupNode* pParent, ACObject* obj, std::vector<ACMaterial>& mat);
static IVistaNode* ReadGeomKid(const std::string& sFileName, const std::string& sTexturePrefix,
    VistaSceneGraph* pSG, VistaGroupNode* pParent, ACObject* obj, std::vector<ACMaterial>& mat);
static IVistaNode* DiveGroup(const std::string& sFileName, const std::string& sTexturePrefix,
    VistaSceneGraph* pSG, VistaGroupNode* pParent, ACObject* pObj, std::vector<ACMaterial>& mat);

static std::string ConstructMaterialName(
    const std::string& sFileName, const std::string& sMatNam, int iIndex) {
  std::string sName = sFileName + std::string(".");
  if (sMatNam.empty())
    sName = sName + VistaConversion::ToString(iIndex);
  else
    sName = sName + sMatNam;

  return sName;
}

// IMPLEMENTATION OF STATIC FUNCTIONS

static IVistaNode* DiveGroup(const std::string& sFileName, const std::string& sTexturePrefix,
    VistaSceneGraph* pSG, VistaGroupNode* pParent, ACObject* pObj, std::vector<ACMaterial>& mat) {
  if (pObj->type == AC3D_OBJECT_NORMAL) {
    if (pObj->num_surf == 1 && ((pObj->surfaces[0].flags & AC3D_SURFACE_TYPE_LINE) ||
                                   ((pObj->surfaces[0].flags & AC3D_SURFACE_TYPE_CLOSEDLINE)))) {
      return ReadPolyLineKid(sFileName, pSG, pParent, pObj, mat);
    } else {
      if (pObj->num_kids > 0) {
        VistaTransformNode* pTrans = pSG->NewTransformNode(pParent); // shadow argument
        pTrans->SetTranslation(pObj->loc.x, pObj->loc.y, pObj->loc.z);
        pTrans->SetName(pObj->name ? pObj->name : "_noname");
        ReadGeomKid(sFileName, sTexturePrefix, pSG, pTrans, pObj, mat);
        for (int i = 0; i < pObj->num_kids; ++i) {
          // dive group will add childs properly
          // to the new group
          DiveGroup(sFileName, sTexturePrefix, pSG, pTrans, pObj->kids[i], mat);
        }
        return pParent; // return new group node
      } else            // simple case: 0 kids, this is a leaf node
        return ReadGeomKid(sFileName, sTexturePrefix, pSG, pParent, pObj, mat);
    }
  } else if (pObj->type == AC3D_OBJECT_LIGHT) {
    vstr::errp() << "Light not supported... yet" << std::endl;
    return NULL;
  } else if (pObj->type == AC3D_OBJECT_GROUP || pObj->type == AC3D_OBJECT_WORLD) {

    VistaTransformNode* pTransform = pSG->NewTransformNode(pParent);
    if (pObj->name)
      pTransform->SetName(pObj->name);

    VistaTransformMatrix m(pObj->matrix[0], pObj->matrix[1], pObj->matrix[2], pObj->loc.x,
        pObj->matrix[3], pObj->matrix[4], pObj->matrix[5], pObj->loc.y, pObj->matrix[6],
        pObj->matrix[7], pObj->matrix[8], pObj->loc.z, 0, 0, 0, 1);

    float f[16];
    m.GetValues(f);
    pTransform->SetTransform(f);
    pTransform->SetTranslation(pObj->loc.x, pObj->loc.y, pObj->loc.z);

    for (int i = 0; i < pObj->num_kids; ++i) {
      DiveGroup(sFileName, sTexturePrefix, pSG, pTransform, pObj->kids[i], mat);
    }
    return pTransform;
  } else {
    vstr::errp() << "unkown node type (" << pObj->type << "). <skip>" << std::endl;
    return NULL;
  }
}

static IVistaNode* ReadGeomKid(const std::string& sFileName, const std::string& sTexturePrefix,
    VistaSceneGraph* pSG, VistaGroupNode* pParent, ACObject* obj, std::vector<ACMaterial>& matTbl) {
  vector<VistaVector3D>      coords;
  vector<VistaVector3D>      textureCoords;
  vector<VistaVector3D>      normals;
  vector<VistaColor>         colors;
  vector<VistaIndexedVertex> vertices;

  coords.resize(obj->num_vert);
  //	normals.resize(obj->num_vert);

  for (int i = 0; i < obj->num_vert; ++i) {
    coords[i] = VistaVector3D(obj->vertices[i].x, obj->vertices[i].y, obj->vertices[i].z);
    //		normals[i] = VistaVector3D(obj->vertices[i].normal.x,
    //			obj->vertices[i].normal.y, obj->vertices[i].normal.z);
    // normals[i].Normalize();
  }

  VistaVertexFormat format;
  format.coordinate = VistaVertexFormat::COORDINATE;
  format.normal     = VistaVertexFormat::NORMAL_AUTO;
  format.color      = VistaVertexFormat::COLOR_NONE;
  if (obj->texture)
    format.textureCoord = VistaVertexFormat::TEXTURE_COORD_2D;
  else
    format.textureCoord = VistaVertexFormat::TEXTURE_COORD_NONE;

  int iShadeFlag = 0;
  int iCullFlag  = 0;

  VistaIndexedVertex vert;

  for (int j = 0; j < obj->num_surf; ++j) {
    if (obj->surfaces[j].num_vertref != 3) {
      vstr::errp() << "Surface "
                   << "[" << j << "] of obj [" << (obj->name ? obj->name : "_noname") << "] has "
                   << (obj->surfaces[j].num_vertref > 3 ? "more" : "less") << " than 3 vertices ("
                   << obj->surfaces[j].num_vertref << "). Parent = ["
                   << (pParent ? pParent->GetName() : "_noparent") << "]"
                   << ". ChildIdx = [" << (pParent ? pParent->GetNumChildren() + 1 : 0)
                   << "]. Skipping surface (triangulate your model)." << std::endl;
      continue;
    }
    // or the shade flag. iff there is _one_ surface in this
    // object that is shaded, we set the style for
    // thie geometry completely
    if (iShadeFlag == 0)
      iShadeFlag = (obj->surfaces[j].flags & AC3D_SURFACE_SHADED);
    // cout << j << "=" << iShadeFlag << std::endl;

    if (iCullFlag == 0)
      iCullFlag = (obj->surfaces[j].flags & AC3D_SURFACE_TWOSIDED);

    if ((obj->surfaces[j].flags & AC3D_SURFACE_TYPE_POLYGON) != AC3D_SURFACE_TYPE_POLYGON) {
      vstr::errp() << "surface (" << j << ") is of type (";
      if ((obj->surfaces[j].flags & AC3D_SURFACE_TYPE_CLOSEDLINE) == AC3D_SURFACE_TYPE_CLOSEDLINE) {
        vstr::err() << "CLOSEDLINE";
      } else if ((obj->surfaces[j].flags & AC3D_SURFACE_TYPE_LINE) == AC3D_SURFACE_TYPE_LINE) {
        vstr::err() << "LINE";
      } else {
        vstr::err() << "<UNKNOWN TYPE (flag=" << obj->surfaces[j].flags << ")>";
      }
      vstr::err() << "-- skipping." << std::endl;
      continue;
    }

    for (int k = 0; k < obj->surfaces[j].num_vertref; ++k) {
      int cIdx = obj->surfaces[j].vertref[k];
      vert.SetCoordinateIndex(cIdx);
      vert.SetNormalIndex(cIdx);
      vert.SetColorIndex(cIdx);
      // the following is ok, as this loader only accepts triangles
      // so, num_vertref is always 3
      vert.SetTextureCoordinateIndex(j * obj->surfaces[j].num_vertref + k);

      textureCoords.push_back(VistaVector3D(
          obj->surfaces[j].uvs[k].u * float(obj->texture_repeat_x) + obj->texture_offset_x,
          obj->surfaces[j].uvs[k].v * float(obj->texture_repeat_y) + obj->texture_offset_y, 0));

      vertices.push_back(vert);
    }
  }

  VistaGeometry* geom =
      pSG->NewIndexedGeometry(vertices, coords, textureCoords, normals, colors, format);
  if (geom) {
    VistaMaterial oMat;
    ACMaterial*   pMat = &matTbl[obj->surfaces[0].mat];
    std::string   sMatName =
        ConstructMaterialName(sFileName, (*pMat).name, (int)obj->surfaces[0].mat);
    if (pSG->GetMaterialByName(sMatName, oMat) == false)
      vstr::errp() << "Warning! could not get material [" << sMatName << "]" << std::endl;
    else {
      geom->SetMaterial(oMat);
    }

    if (obj->texture) {
      if (!sTexturePrefix.empty()) {
        geom->SetTexture(sTexturePrefix + VistaFileSystemDirectory::GetOSSpecificSeparator() +
                         std::string(obj->texture));
      } else {
        std::string sPrefix = sFileName.substr(
            0, 1 + sFileName.rfind(VistaFileSystemDirectory::GetOSSpecificSeparator()));

#if defined(WIN32)
        if (sPrefix.empty())
          sPrefix = sFileName.substr(0, 1 + sFileName.rfind("/"));
#endif
        geom->SetTexture(sPrefix + std::string(obj->texture));
      }
    }

    VistaRenderingAttributes att;
    geom->GetRenderingAttributes(att);

    att.culling =
        iCullFlag ? VistaRenderingAttributes::CULL_NONE : VistaRenderingAttributes::CULL_BACK;

    att.shading = iShadeFlag ? VistaRenderingAttributes::SHADING_NICEST
                             : VistaRenderingAttributes::SHADING_FLAT;

    // cout << "Setting shading to (" << att.shading << ")\n";

    geom->SetRenderingAttributes(att);

    VistaTransformNode* pT = pSG->NewTransformNode(pParent);

    VistaGeomNode* pGeom = pSG->NewGeomNode(pT, geom);
    if (!pGeom) {
      delete geom;
      delete pT;
      return NULL;
    }

    VistaTransformMatrix mat(obj->matrix[0], obj->matrix[1], obj->matrix[2], obj->loc.x,
        obj->matrix[3], obj->matrix[4], obj->matrix[5], obj->loc.y, obj->matrix[6], obj->matrix[7],
        obj->matrix[8], obj->loc.z, 0, 0, 0, 1);

    float f[16];
    mat.GetValues(f);
    pT->SetTransform(f);
    pGeom->SetName(obj->name ? obj->name : "_noname");
    return pT;
  }

  return NULL;
}

static IVistaNode* ReadPolyLineKid(const std::string& sFileName, VistaSceneGraph* pSG,
    VistaGroupNode* pParent, ACObject* obj, std::vector<ACMaterial>& mat) {
  std::vector<float> vecPoints;
  vecPoints.reserve(obj->num_vert * 3);

  for (int i = 0; i < obj->num_vert; ++i) {
    vecPoints.push_back(obj->vertices[i].x); //+obj->loc.x);
    vecPoints.push_back(obj->vertices[i].y); //+obj->loc.y);
    vecPoints.push_back(obj->vertices[i].z); //+obj->loc.z);
  }

  VistaOpenGLPolyLine* pContainer = new VistaOpenGLPolyLine(pSG, pParent);
  pContainer->SetLinePoints(vecPoints);

  ACMaterial*   pMat = &mat[obj->surfaces[0].mat];
  VistaMaterial oMat;
  std::string   sName = ConstructMaterialName(sFileName, (*pMat).name, (int)obj->surfaces[0].mat);
  if (pSG->GetMaterialByName(sName, oMat) == false) {
    vstr::warnp() << "IVistaNode::ReadPolyLineKid() -- "
                  << "Could not resolve material [" << sName << "]" << std::endl;
  } else {
    pContainer->SetMaterial(oMat);
  }

  pContainer->SetIsClosedLine(
      (obj->surfaces[0].flags & AC3D_SURFACE_TYPE_CLOSEDLINE) ? true : false);

  IVistaNode* pGLNode = pContainer->GetVistaNode();
  pGLNode->SetName(obj->name);

  VistaTransformNode* pT = pSG->NewTransformNode(pParent);

  if (pT) {
    VistaTransformMatrix trans(obj->matrix[0], obj->matrix[1], obj->matrix[2], obj->loc.x,
        obj->matrix[3], obj->matrix[4], obj->matrix[5], obj->loc.y, obj->matrix[6], obj->matrix[7],
        obj->matrix[8], obj->loc.z, 0, 0, 0, 1);

    float f[16];
    trans.GetValues(f);
    pT->SetTransform(f);
    pT->AddChild(pGLNode);
  }

  // delete container, but leave sg node intact
  pContainer->SetRemoveFromSGOnDelete(false);
  delete pContainer;

  // return the created node
  return pT;
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaAC3DLoader::VistaAC3DLoader() {
}

VistaAC3DLoader::~VistaAC3DLoader() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
IVistaNode* VistaAC3DLoader::LoadNode(VistaSceneGraph* pSG, VistaGroupNode* pParent,
    const std::string& sFileName, const std::string& sTexturePathPrefix) {
  std::vector<ACMaterial> vecPalette;
  vecPalette.resize(256); // we can only load 256 materials
  int       iNumMaterials = 0;
  ACObject* world         = ac3d_load_ac3d(sFileName.c_str(), &vecPalette[0], iNumMaterials);
  if (!world)
    return NULL;

  vecPalette.resize(iNumMaterials); // trim to size

  // load materials
  for (int i = 0; i < iNumMaterials; ++i) {
    ACMaterial  rMat = vecPalette[i];
    ACMaterial* mat  = &rMat;

    VistaColor ambient((*mat).ambient.r, (*mat).ambient.g, (*mat).ambient.b);

    VistaColor specular((*mat).specular.r, (*mat).specular.g, (*mat).specular.b);

    VistaColor emmissive((*mat).emissive.r, (*mat).emissive.g, (*mat).emissive.b);

    VistaColor diffuse((*mat).rgb.r, (*mat).rgb.g, (*mat).rgb.b);

    std::string   sName = ConstructMaterialName(sFileName, (*mat).name, i);
    VistaMaterial oMat(
        ambient, diffuse, specular, emmissive, (*mat).shininess, 1 - (*mat).transparency, sName);
    pSG->AddMaterial(oMat);
  }

  IVistaNode* pRet = NULL;
  if (pParent == NULL) {
    pParent = pSG->NewGroupNode(NULL); // create unattached group node
    pRet    = pParent;
  }

  IVistaNode* acFile = DiveGroup(sFileName, sTexturePathPrefix, pSG, pParent, world, vecPalette);

  if (pRet == NULL)
    pRet = acFile;

  // free material names
  for (int l = 0; l < iNumMaterials; ++l) {
    ACMaterial* rMat = &vecPalette[l];
    free(rMat->name);
  }

  if (world)
    ac3d_object_free(world);

  acFile->SetName(sFileName);
  return pRet;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
