#pragma once

#define M_PI        3.14159265358979323846
// Converts degrees to radians.
#define DEG2RAD( angleDegrees ) ( float )( angleDegrees * M_PI / 180.0 )
// Converts radians to degrees.
#define RAD2DEG( angleRadians ) ( float )( angleRadians * 180.0 / M_PI )