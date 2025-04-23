#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include "skydome.h"

inline bool gfx_pos_within_viewport(float x, float y){
    return x > 0 && x < display_get_width() && y > 0 && y < display_get_height();
}

color_t color_lerp(color_t a, color_t b, float t){
    color_t res;
    res.r = fm_lerp(a.r, b.r, t);
    res.g = fm_lerp(a.g, b.g, t);
    res.b = fm_lerp(a.b, b.b, t);
    res.a = fm_lerp(a.a, b.a, t);
    return res;
}

double fwrap(double x, double min, double max) {
    if (min > max) {
        return fwrap(x, max, min);
    }
    return (x >= 0 ? min : max) + fmod(x, max - min);
}

void fm_vec3_dir_from_euler(fm_vec3_t *out, const fm_vec3_t *euler) {
    out->z = -cos(euler->y)*cos(euler->x);
    out->x = -sin(euler->y)*cos(euler->x);
    out->y = sin(euler->x);
}

float fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (float) ((float)x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + (float) out_min;
}


struct{
    uint32_t ambient, direct, stars, main, fog, clouds;
    double   time, sun;
} daycycle[] = {
    {.main = 0x243C65FF, .ambient = 0xF5FFFBFF, .direct = 0xD2D6DCFF, .clouds = 0xFFF6F2FF, .fog = 0x3E3C67FF, .stars = 0x191E19FF, .time = 0,     .sun = 90},
    {.main = 0x243C65FF, .ambient = 0xF5FFFBFF, .direct = 0xD2D6DCFF, .clouds = 0xFFF6F2FF, .fog = 0x3E3C67FF, .stars = 0x191E19FF, .time = 8700,  .sun = 120},
    {.main = 0x3A4341FF, .ambient = 0xFFF4E2FF, .direct = 0xE4D6DDFF, .clouds = 0xFFCEAAFF, .fog = 0x3E3C67FF, .stars = 0x090E09FF, .time = 15200, .sun = 140},
    {.main = 0x39472BFF, .ambient = 0x999287FF, .direct = 0xFFB392FF, .clouds = 0xD08C70FF, .fog = 0x5B4567FF, .stars = 0x030505FF, .time = 21000, .sun = 150},
    {.main = 0x393C63FF, .ambient = 0xCA8E5AFF, .direct = 0xFF9868FF, .clouds = 0xD8744AFF, .fog = 0x402452FF, .stars = 0x030505FF, .time = 24300, .sun = 160},

    {.main = 0x5A363FFF, .ambient = 0x43301DFF, .direct = 0xFF743DFF, .clouds = 0x512D2FFF, .fog = 0x00073EFF, .stars = 0x252923FF, .time = 29600, .sun = 180},
    {.main = 0x393B41FF, .ambient = 0x353422FF, .direct = 0xFF0008FF, .clouds = 0x4F555BFF, .fog = 0x20233EFF, .stars = 0x343A3BFF, .time = 35000, .sun = 200},
    {.main = 0x121C2DFF, .ambient = 0x323636FF, .direct = 0x8C808BFF, .clouds = 0x3E4148FF, .fog = 0x40392EFF, .stars = 0x4A454AFF, .time = 43300, .sun = 340},
    {.main = 0x0D1A21FF, .ambient = 0x2E2B27FF, .direct = 0x898C88FF, .clouds = 0x434246FF, .fog = 0x423E32FF, .stars = 0x635E5EFF, .time = 50000, .sun = 360},
    {.main = 0x0C191FFF, .ambient = 0x2C1C09FF, .direct = 0x8F8D83FF, .clouds = 0x484246FF, .fog = 0x423F36FF, .stars = 0x4C4947FF, .time = 56500, .sun = 450},

    {.main = 0x131C2DFF, .ambient = 0x312436FF, .direct = 0x8F8D83FF, .clouds = 0x2F323FFF, .fog = 0x2F323FFF, .stars = 0x6C666CFF, .time = 67200, .sun = 600},
    {.main = 0x41242CFF, .ambient = 0x351D15FF, .direct = 0xF97B21FF, .clouds = 0x48333CFF, .fog = 0x331C00FF, .stars = 0x9D8D94FF, .time = 69400, .sun = 650},
    {.main = 0x39323AFF, .ambient = 0x68261CFF, .direct = 0xFF8F00FF, .clouds = 0x954A32FF, .fog = 0x331C00FF, .stars = 0x5E3E2BFF, .time = 73700, .sun = 680},
    {.main = 0x303A4AFF, .ambient = 0xD45000FF, .direct = 0xFFD8BDFF, .clouds = 0xCF9A7DFF, .fog = 0x5B3035FF, .stars = 0x342817FF, .time = 78000, .sun = 700},
    {.main = 0x3C6B50FF, .ambient = 0xFFB196FF, .direct = 0xF4D9FFFF, .clouds = 0xFFE4D4FF, .fog = 0x5B424CFF, .stars = 0x191E19FF, .time = 82400, .sun = 740},
    {.main = 0x243C65FF, .ambient = 0xF5FFFBFF, .direct = 0xD2D6DCFF, .clouds = 0xFFF6F2FF, .fog = 0x3E3C67FF, .stars = 0x191E19FF, .time = 86400, .sun = 360 + 360 + 90},
};

#define DAYCYCLE_COUNT 16


skydome_t* skydome_create(){
    skydome_t* sky = malloc(sizeof(skydome_t));
    memset(sky, 0, sizeof(skydome_t));
    return sky;
}

void skydome_load_data(skydome_t* skydome, const char* skydomemodelfile, const char* texmainfile, const char* texcloudsfile){
    skydome->__vars.model = t3d_model_load(skydomemodelfile? skydomemodelfile : "rom:/skydome.t3dm");
    skydome->__vars.texmain = sprite_load(texmainfile? texmainfile : "rom:/skydome_tex1.rgba16.sprite");
    skydome->__vars.texclouds = sprite_load(texcloudsfile? texcloudsfile : "rom:/skydome_clouds1.ia8.sprite");
}

void skydome_time_of_day(skydome_t* skydome, double timeofday){
    double time = fwrap(timeofday, 0, daycycle[DAYCYCLE_COUNT - 1].time);
    double indextime = 0; int index = 0;
    double indextimeprev = 0;
    while(time > indextime){
        index++;
        indextimeprev = indextime;
        indextime = daycycle[index].time;
    }

    double T = fmap(time, indextimeprev, indextime, 0.0f, 1.0f);
    skydome->color.main =   (color_lerp(color_from_packed32(daycycle[index - 1].main),    color_from_packed32(daycycle[index].main),   T));
    skydome->color.clouds = (color_lerp(color_from_packed32(daycycle[index - 1].clouds),  color_from_packed32(daycycle[index].clouds), T));
    skydome->color.stars =  (color_lerp(color_from_packed32(daycycle[index - 1].stars),   color_from_packed32(daycycle[index].stars),  T));
    skydome->color.fog =    (color_lerp(color_from_packed32(daycycle[index - 1].fog),     color_from_packed32(daycycle[index].fog),    T));

    skydome->sun.direct =   (color_lerp(color_from_packed32(daycycle[index - 1].direct),  color_from_packed32(daycycle[index].direct), T));
    skydome->sun.ambient =  (color_lerp(color_from_packed32(daycycle[index - 1].ambient), color_from_packed32(daycycle[index].ambient), T));

    skydome->sun.elevation = FM_DEG2RAD(fm_lerp(daycycle[index - 1].sun,daycycle[index].sun, T));
}

void skydome_cloud_pass(skydome_t* skydome, double time){
    fm_vec3_t offset = {0};
    fm_vec3_t offsetclouds = {0};
    fm_vec3_scale(&offset, &skydome->clouds.speed, time);
    fm_vec3_scale(&offsetclouds, &skydome->clouds.speedclouds, time);
    fm_vec3_add(&skydome->__vars.texoffset, &skydome->__vars.texoffset, &offset);
    fm_vec3_add(&skydome->__vars.cloudtexoffset, &skydome->__vars.cloudtexoffset, &offsetclouds);
}

void skydome_lerp(skydome_t* skydome, skydome_t* a, skydome_t* b, float t){
    skydome->color.main =   (color_lerp(a->color.main,    b->color.main,   t));
    skydome->color.clouds = (color_lerp(a->color.clouds,  b->color.clouds, t));
    skydome->color.stars =  (color_lerp(a->color.stars,   b->color.stars,  t));
    skydome->color.fog =    (color_lerp(a->color.fog,     b->color.fog,    t));

    skydome->sun.direct =   (color_lerp(a->sun.direct,   b->sun.direct, t));
    skydome->sun.ambient =  (color_lerp(a->sun.ambient,  b->sun.ambient, t));

    skydome->sun.elevation = fm_lerp(a->sun.elevation, b->sun.elevation, t);
}

void skydome_draw(skydome_t* skydome){

    fm_vec3_t lightdir = {0};
    fm_vec3_t euler = {.x = skydome->sun.elevation, .y = skydome->sun.rotation, .z = 0};
    fm_vec3_dir_from_euler(&lightdir, &euler);

    t3d_light_set_ambient((uint8_t*)&skydome->sun.ambient);
    t3d_light_set_directional(0, (uint8_t*)&skydome->sun.direct, (T3DVec3*)&lightdir);
    t3d_light_set_count(1);

    T3DObject* obj = t3d_model_get_object_by_index(skydome->__vars.model, 0);
    surface_t surf1 = sprite_get_pixels(skydome->__vars.texmain);
    surface_t surf2 = sprite_get_pixels(skydome->__vars.texclouds);

    t3d_model_draw_material(obj->material, NULL);

    float texshift_x = fwrap(skydome->__vars.texoffset.x, 0, 128);
    float texshift_y = fwrap(skydome->__vars.texoffset.y,0, 128);

    float cloudtexshift_x = fwrap(skydome->__vars.cloudtexoffset.x, 0, 128);
    float cloudtexshift_y = fwrap(skydome->__vars.cloudtexoffset.y,0, 128);

    uint8_t cl_density = (1 - skydome->clouds.density) * 255;
    uint8_t cl_opacity = (skydome->clouds.opacity) * 255;

    skydome->color.stars.a = cl_density;
    skydome->color.main.a = cl_opacity;

      rdpq_mode_mipmap(MIPMAP_NONE, 0);
      rdpq_mode_dithering(DITHER_SQUARE_INVSQUARE);
      rdpq_tex_multi_begin();
      rdpq_tex_upload(TILE0, &surf1, &(rdpq_texparms_t){
        .s.repeats = REPEAT_INFINITE, .t.repeats = REPEAT_INFINITE, 
        .s.mirror = true, .t.mirror = false, 
        .s.scale_log = 0, .t.scale_log = 0,
        .s.translate = texshift_x, .t.translate = texshift_y});
      rdpq_tex_upload(TILE1, &surf2, &(rdpq_texparms_t){
        .s.repeats = REPEAT_INFINITE, .t.repeats = REPEAT_INFINITE, 
        .s.mirror = true, .t.mirror = true, 
        .s.scale_log = -1, .t.scale_log = 0,
        .s.translate = cloudtexshift_x, .t.translate = cloudtexshift_y});
      rdpq_tex_multi_end();

        rdpq_set_env_color(skydome->color.main);
        rdpq_set_prim_color(skydome->color.stars);

        rdpq_set_blend_color(skydome->color.clouds);
        rdpq_set_fog_color(skydome->color.fog);
        rdpq_sync_pipe();
    if(!skydome->__vars.modelblock) {
        rspq_block_begin();
        rdpq_mode_mipmap(MIPMAP_NONE, 0);
        rdpq_mode_combiner(RDPQ_COMBINER2(
        (SHADE, TEX0, ENV, SHADE), (1, SHADE, TEX1, 0),
        (PRIM, 0, TEX1, COMBINED), (COMBINED, PRIM, ENV, 0)));
      
        rdpq_mode_blender(RDPQ_BLENDER2(
        (BLEND_RGB, IN_ALPHA,    IN_RGB, INV_MUX_ALPHA),
        (CYCLE1_RGB,   SHADE_ALPHA, CYCLE1_RGB, INV_MUX_ALPHA)));

        rdpq_mode_antialias(AA_NONE);
        rdpq_mode_zbuf(false,false);
        rdpq_sync_pipe();
        t3d_model_draw_object(obj, NULL);
        rdpq_sync_pipe();
        skydome->__vars.modelblock = rspq_block_end();
    }

    // for the actual draw, you can use the generic rspq-api.
    rspq_block_run(skydome->__vars.modelblock);
    rdpq_sync_pipe();
    t3d_frame_start();
}


void skydome_set_viewport(skydome_t* skydome, T3DViewport* viewport){
    skydome->__vars.viewport = viewport;
}

void skydome_load_data_lensflare(skydome_t* skydome, const char* lensflaremain, const char* lensflaresecond, const char* lensflarethird){
    skydome->__vars.lensflare.sprites[0] = sprite_load(lensflaremain? lensflaremain : "rom:/lensflare1.i8.sprite");
    skydome->__vars.lensflare.sprites[1] = sprite_load(lensflaresecond? lensflaresecond : "rom:/lensflare2.i8.sprite");
    skydome->__vars.lensflare.sprites[2] = sprite_load(lensflarethird? lensflarethird : "rom:/lensflare3.i8.sprite");
}


void skydome_lensflare_update_zbuf(skydome_t* skydome){
    assert(skydome);

    float xpos = skydome->__vars.lensflare.xpos;
    float ypos = skydome->__vars.lensflare.ypos;
    
    if(gfx_pos_within_viewport(xpos, ypos)){
        surface_t* zbuffer = display_get_zbuf();
        if(zbuffer){
                uint16_t* pixels = zbuffer->buffer;
                skydome->__vars.lensflare.depth = pixels[(int)ypos * zbuffer->width + (int)xpos];
        }
    }

    
}

void skydome_draw_lensflare(skydome_t* skydome){
    fm_vec3_t viewpos, worldpos;
    fm_vec3_t sunangles = {0}; sunangles.x = skydome->sun.elevation; sunangles.y = skydome->sun.rotation;

    fm_vec3_dir_from_euler(&worldpos, &sunangles);
    fm_vec3_scale(&worldpos, &worldpos, 99999);
    t3d_viewport_calc_viewspace_pos(skydome->__vars.viewport, (T3DVec3*)&viewpos, (T3DVec3*)&worldpos);
    T3DVec4 posScreen;  
    t3d_mat4_mul_vec3(&posScreen, &skydome->__vars.viewport->matCamProj, (T3DVec3*)&worldpos);

    float xpos = viewpos.v[0];
    float ypos = viewpos.v[1];
    skydome->__vars.lensflare.xpos = xpos;
    skydome->__vars.lensflare.ypos = ypos;

    if(gfx_pos_within_viewport(xpos, ypos) && worldpos.y > 0 && posScreen.w > 0 && skydome->__vars.lensflare.depth > 0xFFF0){
        skydome->__vars.lensflare.alpha = t3d_lerp(skydome->__vars.lensflare.alpha, 255.0f, 0.4f);
    }   else skydome->__vars.lensflare.alpha = t3d_lerp(skydome->__vars.lensflare.alpha, 0.0f, 0.4f);


    color_t suncolor = skydome->sun.direct;
    suncolor.a = (uint8_t)skydome->__vars.lensflare.alpha;

    if(skydome->__vars.lensflare.alpha > 25.0f){
            rdpq_set_prim_color(suncolor);

            rdpq_sprite_blit(skydome->__vars.lensflare.sprites[0], xpos - (skydome->__vars.lensflare.sprites[0]->width / 2), ypos - (skydome->__vars.lensflare.sprites[0]->height / 2), NULL);
            float xpos2 = (display_get_width() - xpos);
            float ypos2 = (display_get_height() - ypos);
            rdpq_sprite_blit(skydome->__vars.lensflare.sprites[1], xpos2 - (skydome->__vars.lensflare.sprites[1]->width / 2), ypos2 - (skydome->__vars.lensflare.sprites[1]->height / 2), NULL);
            xpos = (xpos + xpos2) / 2;
            ypos = (ypos + ypos2) / 2;
            rdpq_sprite_blit(skydome->__vars.lensflare.sprites[2], xpos - (skydome->__vars.lensflare.sprites[2]->width / 2), ypos - (skydome->__vars.lensflare.sprites[2]->height / 2), NULL);
            xpos = (xpos + xpos2) / 2;
            ypos = (ypos + ypos2) / 2;
            rdpq_sprite_blit(skydome->__vars.lensflare.sprites[2], xpos - (skydome->__vars.lensflare.sprites[2]->width / 2), ypos - (skydome->__vars.lensflare.sprites[2]->height / 2), NULL);
    }
}


void skydome_free(skydome_t* skydome){
    if(skydome->__vars.model) t3d_model_free(skydome->__vars.model);
    if(skydome->__vars.texmain) sprite_free(skydome->__vars.texmain);
    if(skydome->__vars.texclouds) sprite_free(skydome->__vars.texclouds);
    if(skydome->__vars.modelblock) rspq_block_free(skydome->__vars.modelblock);
    if(skydome->__vars.lensflare.sprites[0]) sprite_free(skydome->__vars.lensflare.sprites[0]);
    if(skydome->__vars.lensflare.sprites[1]) sprite_free(skydome->__vars.lensflare.sprites[1]);
    if(skydome->__vars.lensflare.sprites[2]) sprite_free(skydome->__vars.lensflare.sprites[2]);
    skydome->__vars.model = NULL;
    skydome->__vars.texmain = NULL;
    skydome->__vars.texclouds = NULL;
    skydome->__vars.modelblock = NULL;
    skydome->__vars.lensflare.sprites[0] = NULL;
    skydome->__vars.lensflare.sprites[1] = NULL;
    skydome->__vars.lensflare.sprites[2] = NULL;
    free(skydome);
}