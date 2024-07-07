/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#ifndef TINY3D_T3DDEBUG_H
#define TINY3D_T3DDEBUG_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Initializes the debug print system, make sure to have 'font.ia4.png' in your FS
 * @deprecated Use 'rdpq_text_register_font' with one of the built-in fonts instead
 * E.g.: 'rdpq_text_register_font(FONT_BUILTIN_DEBUG_MONO, rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO));'
 */
__attribute__((deprecated)) void t3d_debug_print_init();

/**
 * @brief Prepares the RDP for debug printing
 * @deprecated Can be removed. Check if a sync is needed before removing
 */
void t3d_debug_print_start();

/**
 * @brief Prints a string at the given position
 * @deprecated Use 'rdpq_text_print' instead
 */
void t3d_debug_print(float x, float y, const char* str);

/**
 * @brief Prints a formatted string at the given position
 * @deprecated Use 'rdpq_text_printf' instead
 */
void t3d_debug_printf(float x, float y, const char* fmt, ...);

#define T3D_DEBUG_CHAR_C_LEFT "\x7b"
#define T3D_DEBUG_CHAR_C_RIGHT "\x7c"
#define T3D_DEBUG_CHAR_C_UP "\x7d"
#define T3D_DEBUG_CHAR_C_DOWN "\x7e"
#define T3D_DEBUG_CHAR_A "\x7f"
#define T3D_DEBUG_CHAR_B "\x80"

#ifdef __cplusplus
}
#endif

#endif // TINY3D_T3DDEBUG_H