#pragma once

#define EASY_MODE   false    /* for Julia */

#if EASY_MODE
#define MAX_LIFES 5
#define MAX_ENEMIES 3
#else
#define MAX_LIFES 3
#define MAX_ENEMIES 5
#endif


#define M_PI        3.14159265358979323846
// Converts degrees to radians.
#define DEG2RAD( angleDegrees ) ( float )( angleDegrees * M_PI / 180.0 )
// Converts radians to degrees.
#define RAD2DEG( angleRadians ) ( float )( angleRadians * 180.0 / M_PI )