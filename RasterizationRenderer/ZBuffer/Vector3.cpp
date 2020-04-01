#include "Vector3.h"

// œÚ¡ø≤Ê≥À
Vector3 cross(Vector3& a, Vector3& b)
{
	return Vector3((a.y * b.z - a.z * b.y), (a.z * b.x - a.x * b.z), (a.x * b.y - a.y * b.x));
}

Color operator* (float a, Color& color)
{
	return color * a;
}