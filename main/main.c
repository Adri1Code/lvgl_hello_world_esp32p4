#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bsp/esp-bsp.h"    //bsp = board support package
#include "bsp/display.h"
#include "lvgl.h"

static const char *TAG = "MAIN";

void app_main(void)
{ 
    // 1. Configuration de l'affichage (reprise de ton code fonctionnel)
    bsp_display_cfg_t cfg = {
        .lvgl_port_cfg = ESP_LVGL_PORT_INIT_CONFIG(),
        .buffer_size = BSP_LCD_DRAW_BUFF_SIZE,
        .double_buffer = BSP_LCD_DRAW_BUFF_DOUBLE,
        .flags = {
            .buff_dma = true,
            .buff_spiram = false,
            .sw_rotate = true,
        }
    };
    
    // 2. Initialisation du matériel
    lv_display_t *disp = bsp_display_start_with_config(&cfg);
    bsp_display_backlight_on();
    bsp_display_brightness_set(100);

    if (disp != NULL) {
        bsp_display_rotate(disp, LV_DISPLAY_ROTATION_180);
    }

    // 3. Création de l'interface graphique
    bsp_display_lock(0); // On verrouille LVGL pour manipuler les objets

    lv_obj_t *scr = lv_scr_act(); // Récupère l'écran actif


    // Slider en haut de l'ecran
    lv_obj_t *slider = lv_slider_create(scr);
    lv_slider_set_range(slider, 0, 100);
    lv_slider_set_value(slider, 100, LV_ANIM_OFF);
    lv_obj_set_width(slider, lv_pct(80));           // 80% of screen width
    lv_obj_align(slider, LV_ALIGN_TOP_MID, 0, 20);  // top and center


    // Création du texte
    lv_obj_t *label = lv_label_create(scr);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_44, 0);
    lv_label_set_text(label, "Hello World !");
    
    // Style : Utilisation de la police par défaut (souvent Montserrat 14 ou 20)
    // On centre l'objet sur l'écran
    lv_obj_center(label);

    bsp_display_unlock(); // On libère LVGL

    ESP_LOGI(TAG, "Hello World affiche avec succes.");

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}