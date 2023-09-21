#include "../ui.h"

void ui_Screen2_screen_init(void)
{
    ui_Screen2 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Image3 = lv_img_create(ui_Screen2);
    lv_img_set_src(ui_Image3, &ui_img_smallwps_png);
    lv_obj_set_width(ui_Image3, LV_SIZE_CONTENT);   /// 150
    lv_obj_set_height(ui_Image3, LV_SIZE_CONTENT);    /// 167
    lv_obj_set_x(ui_Image3, 2);
    lv_obj_set_y(ui_Image3, -46);
    lv_obj_set_align(ui_Image3, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Image3, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_Image3, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_S2_Label1 = lv_label_create(ui_Screen2);
    lv_obj_set_width(ui_S2_Label1, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_S2_Label1, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_S2_Label1, 3);
    lv_obj_set_y(ui_S2_Label1, 79);
    lv_obj_set_align(ui_S2_Label1, LV_ALIGN_CENTER);
    lv_label_set_text(ui_S2_Label1, "Press the WPS button ");

    ui_S2_Label2 = lv_label_create(ui_Screen2);
    lv_obj_set_width(ui_S2_Label2, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_S2_Label2, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_S2_Label2, 0);
    lv_obj_set_y(ui_S2_Label2, 101);
    lv_obj_set_align(ui_S2_Label2, LV_ALIGN_CENTER);
    lv_label_set_text(ui_S2_Label2, "on your Wi-Fi Router");

}