SET(SOURCES screens/ui_Screen1.c
    screens/ui_Screen2.c
    screens/ui_Screen3.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    images/ui_img_smallwps_png.c
    images/ui_img_btc_logo_png.c
    fonts/ui_font_Display24.c
    fonts/ui_font_Display48.c
    fonts/ui_font_MontBig.c)

add_library(ui ${SOURCES})
