#ifndef _CUSTOMMATH_H
#define _CUSTOMMATH_H

#include "Vector.h"
#include <math.h>
#include <d3dx9.h>

Vector3 crossProduct(Vector3, Vector3);
void normalise(Vector3&);
float length(Vector3);

typedef struct Mouse
{
	int x, y, drag, old_x, new_x, old_y, new_y , drag_x, drag_y;
}Mouse;

#endif