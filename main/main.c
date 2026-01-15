#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bsp/esp-bsp.h"    
#include "bsp/display.h"
#include "lvgl.h"

#define SCREEN_WIDTH 1024
#define SCREE_HEIGHT 600

static const char *TAG = "APP_MAIN";

static lv_obj_t *screen1 = NULL;
static lv_obj_t *screen2 = NULL;

// Modifie la luminosite selon le changement sur le slider
static void brightness_slider_event_cb(lv_event_t* event)
{
    lv_obj_t *slider = lv_event_get_target(event);
    uint32_t value = lv_slider_get_value(slider);

    bsp_display_brightness_set(value);

    ESP_LOGI(TAG, "Luminositee ajustee a : %ld%%", value);
}

// Changement d'ecran avec pression du doigt
static void screen_long_press_event_cb()
{
    lv_point_t point;
    lv_indev_get_point(lv_indev_get_act(), &point);

    if (point.x > SCREEN_WIDTH / 2)
    {
        ESP_LOGI(TAG, "Appui long detecte sur la droite - Ecran 1->2");
        lv_scr_load_anim(screen2, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
    }
    else
    {
        ESP_LOGI(TAG, "Appui long detecte sur la gauche - Ecran 2->1");
        lv_scr_load_anim(screen1, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0, false);
    }
}


void app_main(void)
{ 
    // Configuration de l'affichage
    bsp_display_cfg_t cfg = {
        .lvgl_port_cfg = ESP_LVGL_PORT_INIT_CONFIG(),   // parametres de base pour LVGL
        .buffer_size = BSP_LCD_DRAW_BUFF_SIZE,          // quantite de memoire allouee pour rendu graphique
        .double_buffer = BSP_LCD_DRAW_BUFF_DOUBLE,      // rendu plus fluide
        .flags = {
            .buff_dma = true,                           // transferts memoire sans CPU
            .buff_spiram = false,                       // utilise RAM interne plutot que SPIRAM
            .sw_rotate = true,                          // pivot de l'affichage
        }
    };
    
    // Initialisation du materiel
    lv_display_t *disp = bsp_display_start_with_config(&cfg);
    bsp_display_backlight_on();
    bsp_display_brightness_set(100);

    if (disp != NULL) {
        bsp_display_rotate(disp, LV_DISPLAY_ROTATION_180);
    }

    // Creation de l'interface graphique
    bsp_display_lock(0);                                // verrouillage de LVGL pour manipuler les objets

    // Creation du premier screen 
    screen1 = lv_obj_create( NULL );                     
    screen1 = lv_scr_act();

    // Slider en haut de l'ecran
    lv_obj_t *slider = lv_slider_create(screen1);       // creer un slider
    lv_slider_set_range(slider, 0, 100);                // range du slider
    lv_slider_set_value(slider, 100, LV_ANIM_OFF);      // valeur initiale
    lv_obj_set_width(slider, lv_pct(80));               // 80% de la largeur de l'ecran
    lv_obj_align(slider, LV_ALIGN_TOP_MID, 0, 20);      // aligne en haut au centre 

    // Evenement : utilisateur change le slider 
    lv_obj_add_event_cb(slider, brightness_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t *slider_label = lv_label_create(screen1);      
    lv_label_set_text(slider_label, "Brightness");
    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

    lv_obj_t *label = lv_label_create(screen1);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_44, 0);
    lv_label_set_text(label, "Hello World !");
    lv_obj_center(label);                                // centrer le texte

    // Changer de screen
    lv_obj_add_event_cb(screen1, screen_long_press_event_cb, LV_EVENT_LONG_PRESSED, NULL);

    // Creation du deuxieme ecran
    screen2 = lv_obj_create( NULL ); 

    lv_obj_t *label2 = lv_label_create(screen2);
    lv_obj_set_style_text_font(label2, &lv_font_montserrat_44, 0);
    lv_label_set_text(label2, "Screen 2");
    lv_obj_center(label2);

    // Changer de screen
    lv_obj_add_event_cb(screen2, screen_long_press_event_cb, LV_EVENT_LONG_PRESSED, NULL);

    bsp_display_unlock();                                // deverouillage de LVGL

    ESP_LOGI(TAG, "Hello World affiche avec succes.");

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}