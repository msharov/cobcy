/* mesh.h
**
**	Defines a polygon mesh structure.
**
** includes:
**	streamable.h	- base class
**	vector.h	- vector definition
**	matrix.h	- matrix definition
*/

#ifndef __MESH_H
#define __MESH_H

#include <streamab.h>
#include <matrix.h>
#include <vector.h>
#include <xcamera.h>
#include <3d.h>

#define MESH_SIGNATURE		"MESH\0"
		  
typedef enum {
    Wireframe,
    Empty,
    Filled
} DrawMode;

typedef struct {
    WORD		start;
    WORD       		end;
} Edge;

typedef struct {
    WORD       		nEdges;
    WORD *     		Edges;
    WORD *		Vertices;
    Vector<int>		normal;
} Face;

class Mesh : public Streamable {
protected:
    Point *		Points;
    Edge *		Edges;
    Face *		Faces;
    WORD		nPoints;
    WORD		nEdges;
    WORD		nFaces;
    
protected:
    void		FindFaceVertices (void);
    void		ComputeFaceNormals (void);
     
public:
			Mesh (void);
    virtual void	Transform (const Matrix<double>& tm);
    virtual void	Draw (XCamera * xc, DrawMode mode = Wireframe);
    virtual void	Read (ifstream& is);
    virtual	       ~Mesh (void);
}; 			

/*-----------------------------------------------------
  Point&	operator= (Point& p1, Point& p2);
-----------------------------------------------------*/

#endif

