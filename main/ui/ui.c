// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.1
// LVGL version: 8.3.6
// Project name: SquareLine_Project

#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////

// SCREEN: ui_Screen1
void ui_Screen1_screen_init(void);
lv_obj_t * ui_Screen1;
lv_obj_t * ui_Image3;
lv_obj_t * ui_Label1;

// SCREEN: ui_Screen2
void ui_Screen2_screen_init(void);
lv_obj_t * ui_Screen2;
lv_obj_t * ui_Label2;
lv_obj_t * ui_Label3;
lv_obj_t * ui_Label4;
lv_obj_t * ui_Label5;
lv_obj_t * ui_Label6;
lv_obj_t * ui_Label7;
lv_obj_t * ui_Label8;
lv_obj_t * ui_Label9;
lv_obj_t * ui_Label10;
lv_obj_t * ui_Label11;
lv_obj_t * ui_Label12;
lv_obj_t * ui_Label13;
lv_obj_t * ui_Label14;
lv_obj_t * ui_Label15;
lv_obj_t * ui_Label16;
lv_obj_t * ui_Label17;
lv_obj_t * ui_S2_Label1;
lv_obj_t * ui_S2_Label2;
lv_obj_t * ui_Panel1;
lv_obj_t * ui_Panel2;
lv_obj_t * ui_Panel3;
lv_obj_t * ui_Panel4;
lv_obj_t * ui_Panel5;
lv_obj_t * ui_Panel6;
lv_obj_t * ui_Panel7;
lv_obj_t * ui_Panel8;
lv_obj_t * ui_Panel10;
lv_obj_t * ui_Spinner1;

void ui_Screen3_screen_init(void);
lv_obj_t * ui_Screen3;
lv_obj_t * ui_Image1;
lv_obj_t * ui_Label20;

//lv_obj_t * ui_Image2;
lv_obj_t * ui____initial_actions0;
//const lv_img_dsc_t * ui_imgset_btc_logo[1] = {&ui_img_btc_logo1_png};
//const lv_img_dsc_t * ui_imgset_wifi_logo[2] = {&ui_img_wifi_logo2_png, &ui_img_wifi_logo3_png};

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=0
    #error "LV_COLOR_16_SWAP should be 0 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////

///////////////////// SCREENS ////////////////////

void ui_init(void)
{
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ui_Screen1_screen_init();
    ui_Screen2_screen_init();
    ui_Screen3_screen_init();
    ui____initial_actions0 = lv_obj_create(NULL);
    //lv_disp_load_scr(ui_Screen3);
}
