/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#ifndef TINY3D_T3DDEBUG_H
#define TINY3D_T3DDEBUG_H

void t3d_debug_print_init();
void t3d_debug_print_start();
void t3d_debug_print(float x, float y, const char* str);
void t3d_debug_printf(float x, float y, const char* fmt, ...);

#endif // TINY3D_T3DDEBUG_H