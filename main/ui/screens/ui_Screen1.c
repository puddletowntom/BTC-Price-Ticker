// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.1
// LVGL version: 8.3.6
// Project name: SquareLine_Project

#include "../ui.h"

void ui_Screen1_screen_init(void)
{
    ui_Screen1 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    // ui_Image8 = lv_img_create(ui_Screen1);
    // lv_img_set_src(ui_Image8, &ui_img_btc_logo1_png);
    // lv_obj_set_width(ui_Image8, LV_SIZE_CONTENT);   /// 40
    // lv_obj_set_height(ui_Image8, LV_SIZE_CONTENT);    /// 53
    // lv_obj_set_x(ui_Image8, 143);
    // lv_obj_set_y(ui_Image8, -3);
    // lv_obj_set_align(ui_Image8, LV_ALIGN_CENTER);
    // lv_obj_add_flag(ui_Image8, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    // lv_obj_clear_flag(ui_Image8, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Label1 = lv_label_create(ui_Screen1);
    lv_obj_set_width(ui_Label1, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label1, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label1, LV_ALIGN_CENTER);
    //lv_label_set_text(ui_Label1, "26415");
    lv_obj_set_style_text_font(ui_Label1, &ui_font_MontBig, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Image11 = lv_img_create(ui_Screen1);
    lv_img_set_src(ui_Image11, &ui_img_wifi_logo3_png);
    lv_obj_set_width(ui_Image11, LV_SIZE_CONTENT);   /// 44
    lv_obj_set_height(ui_Image11, LV_SIZE_CONTENT);    /// 44
    lv_obj_set_x(ui_Image11, 152);
    lv_obj_set_y(ui_Image11, -139);
    lv_obj_set_align(ui_Image11, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Image11, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_Image11, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

}
