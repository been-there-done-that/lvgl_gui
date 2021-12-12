
/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define _DEFAULT_SOURCE /* needed for usleep() */
#define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain" issue*/

#include <stdlib.h>
#include <unistd.h>
#include "lvgl/lvgl.h"

#include "lv_drivers/display/monitor.h"


/**********************
 *  STATIC PROTOTYPES
 **********************/
static void pc_window_simulator(void);
static int tick_thread(void *data);
static void spinner_wheel(void);


/**********************
 *  STATIC VARIABLES
 **********************/
#  define MONITOR_HOR_RES     1280
#  define MONITOR_VER_RES     720


int main(int argc, char **argv)
{
  (void)argc; /*Unused*/
  (void)argv; /*Unused*/

  /*Initialize LVGL*/
  lv_init();

  /*Initialize the HAL (display, input devices, tick) for LVGL*/
  pc_window_simulator();

  spinner_wheel();

  while (1)
  {
    /* Periodically call the lv_task handler.
       * It could be done in a timer interrupt or an OS task too.*/
    lv_timer_handler();
    usleep(5 * 1000);
  }

  return 0;
}

/**********************
 *   CUSTOM FUNCTIONS CREATED BY ME
 **********************/

void spinner_wheel(void)
{
  /*Create a spinner*/
  lv_obj_t *spinner = lv_spinner_create(lv_scr_act(), 100, 60);
  lv_obj_set_size(spinner, 100, 100);
  lv_obj_align(spinner, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
  // lv_obj_center(spinner);
}



/**
 * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
 * library
 */
static void pc_window_simulator(void)
{
  /* Use the 'monitor' driver which creates window on PC's monitor to simulate a display*/
  monitor_init();

  /* Tick init.
   * You have to call 'lv_tick_inc()' in periodically to inform LittelvGL about
   * how much time were elapsed Create an SDL thread to do this*/
  SDL_CreateThread(tick_thread, "tick", NULL);

  /*Create a display buffer*/
  static lv_disp_draw_buf_t disp_buf1;
  static lv_color_t buf1_1[MONITOR_HOR_RES * 100];
  static lv_color_t buf1_2[MONITOR_HOR_RES * 100];
  lv_disp_draw_buf_init(&disp_buf1, buf1_1, buf1_2, MONITOR_HOR_RES * 100);

  /*Create a display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv); /*Basic initialization*/
  disp_drv.draw_buf = &disp_buf1;
  disp_drv.flush_cb = monitor_flush;
  disp_drv.hor_res = MONITOR_HOR_RES;
  disp_drv.ver_res = MONITOR_VER_RES;
  disp_drv.antialiasing = 1;

  lv_disp_t *disp = lv_disp_drv_register(&disp_drv);
}


/**
 * A task to measure the elapsed time for LVGL
 * @param data unused
 * @return never return
 */
static int tick_thread(void *data)
{
  (void)data;

  while (1)
  {
    SDL_Delay(5);
    lv_tick_inc(5); /*Tell LittelvGL that 5 milliseconds were elapsed*/
  }

  return 0;
}
