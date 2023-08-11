SET(SOURCES screens/ui_Screen1.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    images/ui_img_btc_logo1_png.c
    images/ui_img_wifi_logo3_png.c
    images/ui_img_wifi_logo2_png.c
    fonts/ui_font_Display24.c
    fonts/ui_font_Display48.c)

add_library(ui ${SOURCES})
