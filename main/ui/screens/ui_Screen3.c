#include "../ui.h"

void ui_Screen3_screen_init(void)
{
    ui_Screen3 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen3, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Image1 = lv_img_create(ui_Screen3);
    lv_img_set_src(ui_Image1, &ui_img_btc_logo_png);
    lv_obj_set_width(ui_Image1, LV_SIZE_CONTENT);   /// 113
    lv_obj_set_height(ui_Image1, LV_SIZE_CONTENT);    /// 150
    lv_obj_set_x(ui_Image1, -2);
    lv_obj_set_y(ui_Image1, -24);
    lv_obj_set_align(ui_Image1, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Image1, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_Image1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Label20 = lv_label_create(ui_Screen3);
    lv_obj_set_width(ui_Label20, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label20, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label20, -5);
    lv_obj_set_y(ui_Label20, 94);
    lv_obj_set_align(ui_Label20, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label20, "Setting up...");
}