/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#ifndef TINY3D_T3DDEBUG_H
#define TINY3D_T3DDEBUG_H

/// @brief Initializes the debug print system, make sure to have 'font.ia4.png' in your FS
void t3d_debug_print_init();

/// @brief Prepares the RDP for debug printing
void t3d_debug_print_start();

/// @brief Prints a string at the given position
void t3d_debug_print(float x, float y, const char* str);

/// @brief Prints a formatted string at the given position
void t3d_debug_printf(float x, float y, const char* fmt, ...);

#endif // TINY3D_T3DDEBUG_H