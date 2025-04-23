/**
 * @file skydome.h
 * @brief Defines the Skydome entity for use as a 3D game backdrop.
 *
 * This header file contains the definition and functions for managing a Skydome entity,
 * which represents the sky in a 3D game scene. It includes properties for the sun, clouds,
 * colors, and internal variables, as well as functions for creating, updating, and rendering
 * the Skydome.
 * 
 * Skydome library and models made by SpookyIluha
 */

 #ifndef SKYDOME_H
 #define SKYDOME_H
 
 #include <libdragon.h>
 #include <t3d/t3d.h>
 #include <t3d/t3dmath.h>
 #include <t3d/t3dmodel.h>
 
 /**
  * @struct skydome_t
  * @brief Represents a skydome entity and its associated data.
  *
  * This structure contains all the properties and internal variables required
  * to define and manage a single skydome entity
  */
 typedef struct {
     struct {
         float elevation; /**< Pitch of the sun (angle above the horizon) in radians. */
         float rotation;  /**< Yaw of the sun (angle around the vertical axis) in radians. */
 
         color_t direct;  /**< Color of the sunlight. */
         color_t ambient; /**< Color of the ambient light in the environment. */
     } sun; /**< Properties of the sun in the skydome. */
 
     struct {
         float density; /**< Density of the clouds (how many clouds are present) 0-1 range. */
         float opacity; /**< Opacity of the clouds (how transparent they are) 0-1 range. */
 
         fm_vec3_t speed;       /**< Speed of the main texture movement. */
         fm_vec3_t speedclouds; /**< Speed of the cloud texture movement. */
     } clouds; /**< Properties of the clouds in the skydome. */
 
     struct {
         color_t stars;  /**< Color of the stars in the sky. */
         color_t main;   /**< Main interpolation color of the sky. */
         color_t fog;    /**< Color of the fog in the sky. */
         color_t clouds; /**< Color of the clouds. */
     } color; /**< Different colors used in the skydome. */
 
     struct {
         sprite_t* texmain;       
         sprite_t* texclouds;     
         T3DModel* model;         
 
         fm_vec3_t sunforward;    
 
         rspq_block_t* modelblock; 
         fm_vec3_t cloudtexoffset; 
         fm_vec3_t texoffset;     
 
         struct {
             sprite_t* sprites[3]; 
             float alpha;        
             uint16_t depth;   
             float xpos, ypos; 
         } lensflare; 
 
         T3DViewport* viewport; /**< Internal viewport reference for rendering. */
     } __vars; /**< Internal variables for managing the skydome. */
 } skydome_t;
 
 /**
  * @brief Creates a skydome with empty data.
  * @return Pointer to the newly created skydome.
  */
 skydome_t* skydome_create();
 
 /**
  * @brief Loads model and texture data into the skydome. Leave arguments as NULL to load default files.
  *
  * @param skydome Pointer to the skydome to populate.
  * @param skydomemodelfile Path to the skydome model file.
  * @param texmainfile Path to the main texture file (RGBA16 format).
  * @param texcloudsfile Path to the clouds texture file (I/A 8 format).
  */
 void skydome_load_data(skydome_t* skydome, const char* skydomemodelfile, const char* texmainfile, const char* texcloudsfile);
 
 /**
  * @brief Loads sprites for the lens flare effect. Leave arguments as NULL to load default files.
  *
  * @param skydome Pointer to the Skydome to populate.
  * @param lensflaremain Path to the main lens flare sprite.
  * @param lensflaresecond Path to the second lens flare sprite.
  * @param lensflarethird Path to the third lens flare sprite.
  */
 void skydome_load_data_lensflare(skydome_t* skydome, const char* lensflaremain, const char* lensflaresecond, const char* lensflarethird);
 
 /**
  * @brief Sets the skydome colors based on the time of day.
  *
  * @param skydome Pointer to the skydome to update.
  * @param timeofday Time of day in seconds as a double (e.g., 0.0 for noon, 12.0 * 60 * 60 for midnight), can wrap around.
  */
 void skydome_time_of_day(skydome_t* skydome, double timeofday);
 
 /**
  * @brief Updates the cloud texture offset based on delta time.
  *
  * @param skydome Pointer to the skydome to update.
  * @param time Delta time since the last update.
  */
 void skydome_cloud_pass(skydome_t* skydome, double time);
 
 /**
  * @brief Sets the internal viewport reference for lens flare calculations.
  *
  * @param skydome Pointer to the skydome to update.
  * @param viewport Pointer to the T3DViewport to associate with the skydome.
  */
 void skydome_set_viewport(skydome_t* skydome, T3DViewport* viewport);
 
 /**
  * @brief Interpolates between two skydome colors.
  *
  * @param skydome Pointer to the skydome to update.
  * @param a Pointer to the first skydome for interpolation.
  * @param b Pointer to the second skydome for interpolation.
  * @param t Interpolation factor (0.0 to 1.0).
  */
 void skydome_lerp(skydome_t* skydome, skydome_t* a, skydome_t* b, float t);
 
 /**
  * @brief Draws the skydome using T3D's renderer.
  *
  * @param skydome Pointer to the skydome to draw.
  */
 void skydome_draw(skydome_t* skydome);
 
 /**
  * @brief Updates the depth buffer for lens flare occlusion checks.
  *
  * @param skydome Pointer to the skydome to update.
  */
 void skydome_lensflare_update_zbuf(skydome_t* skydome);
 
 /**
  * @brief Draws the lens flare effect.
  *
  * @param skydome Pointer to the skydome to draw.
  */
 void skydome_draw_lensflare(skydome_t* skydome);
 
 /**
  * @brief Frees the data associated with the skydome.
  *
  * @param skydome Pointer to the skydome to free.
  */
 void skydome_free(skydome_t* skydome);
 
 #endif