////////////////////////////////////////MUTI THREAD AND MUTI CORE VERSION////////////////////////////////////////
// #include <stdio.h>
// #include <stdint.h>
// #include "esp_log.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "esp_timer.h"

// static const char *TAG = "MAIN";

// // เก็บเวลา (ms) และสถานะว่าเสร็จหรือยัง
// static volatile int64_t g_time_int   = 0;
// static volatile int64_t g_time_float = 0;
// static volatile int64_t g_time_io    = 0;
// static volatile bool    g_done_int   = false;
// static volatile bool    g_done_float = false;
// static volatile bool    g_done_io    = false;

// void task_int(void *arg)
// {
//     int64_t start = esp_timer_get_time();   

//     uint32_t sum = 0;
//     for (int i = 0; i < 200000; i++) {
//         sum += i * 3;
//     }

//     int64_t end = esp_timer_get_time();
//     g_time_int = (end - start) / 1000;    
//     g_done_int = true;

//     ESP_LOGI("INT", "Finished, time = %lld ms (sum=%u)", g_time_int, sum);
//     vTaskDelete(NULL); 
// }

// void task_float(void *arg)
// {
//     int64_t start = esp_timer_get_time();

//     float x = 1.0f;
//     for (int i = 0; i < 120000; i++) {
//         x = x * 1.00001f + 0.000001f;
//     }

//     int64_t end = esp_timer_get_time();
//     g_time_float = (end - start) / 1000;
//     g_done_float = true;

//     ESP_LOGI("FLOAT", "Finished, time = %lld ms (x=%.3f)", g_time_float, x);
//     vTaskDelete(NULL);
// }

// void task_io(void *arg)
// {
//     int64_t start = esp_timer_get_time();

//     // จำลอง IO ช้า ๆ ~1 วินาที
//     for (int i = 0; i < 10; i++) {
//         vTaskDelay(pdMS_TO_TICKS(100));
//     }

//     int64_t end = esp_timer_get_time();
//     g_time_io = (end - start) / 1000;
//     g_done_io = true;

//     ESP_LOGI("IO", "Finished, time = %lld ms", g_time_io);
//     vTaskDelete(NULL);
// }

// // มอนิเตอร์: รอทุก task เสร็จ -> สรุปรอบเดียว
// void task_monitor(void *arg)
// {
//     ESP_LOGI("MON", "Waiting for all tasks to finish...");

//     // รอจนทั้งสามตัว set flag ว่าเสร็จ
//     while (!(g_done_int && g_done_float && g_done_io)) {
//         vTaskDelay(pdMS_TO_TICKS(10));  // เช็คทุก 10 ms
//     }

//     ESP_LOGI("MON", "==== SUMMARY (one run) ====");
//     ESP_LOGI("MON", " INT   : %lld ms", g_time_int);
//     ESP_LOGI("MON", " FLOAT : %lld ms", g_time_float);
//     ESP_LOGI("MON", " IO    : %lld ms", g_time_io);

//     int64_t max = g_time_int;
//     if (g_time_float > max) max = g_time_float;
//     if (g_time_io    > max) max = g_time_io;

//     ESP_LOGI("MON", " Max time (bottleneck) = %lld ms", max);
//     ESP_LOGI("MON", "===========================");

   
//     vTaskDelete(NULL);
// }

// void app_main(void)
// {
//     ESP_LOGI(TAG, "Starting ONE-SHOT Multithread Test!");

//     // สร้าง 3 งาน + มอนิเตอร์ (pin core แบบเดิมได้เลย)
//     xTaskCreatePinnedToCore(task_int,     "INT",     4096, NULL, 3, NULL, 0);
//     xTaskCreatePinnedToCore(task_float,   "FLOAT",   4096, NULL, 3, NULL, 1);
//     xTaskCreatePinnedToCore(task_io,      "IO",      4096, NULL, 3, NULL, 0);
//     xTaskCreatePinnedToCore(task_monitor, "MONITOR", 4096, NULL, 2, NULL, 1);
// }

////////////////////////////////////////SiNGLE THREAD VERSION////////////////////////////////////////

// #include <stdio.h>
// #include <stdint.h>
// #include "esp_log.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "esp_timer.h"

// static const char *TAG = "MAIN2";

// void app_main(void)
// {
//     ESP_LOGI(TAG, "Starting SINGLE-THREAD test...");

//     // --- จับเวลาเริ่มทั้งหมด ---
//     int64_t start_all = esp_timer_get_time();

//     // ========================
//     // 1) งาน INT (คำนวณจำนวนเต็ม)
//     // ========================
//     int64_t s1 = esp_timer_get_time();

//     uint32_t sum = 0;
//     for (int i = 0; i < 200000; i++) {
//         sum += i * 3;
//     }

//     int64_t e1 = esp_timer_get_time();
//     int64_t t_int = (e1 - s1) / 1000;  // ms

//     // =========================
//     // 2) งาน FLOAT (คำนวณทศนิยม)
//     // =========================
//     int64_t s2 = esp_timer_get_time();

//     float x = 1.0f;
//     for (int i = 0; i < 120000; i++) {
//         x = x * 1.00001f + 0.000001f;
//     }

//     int64_t e2 = esp_timer_get_time();
//     int64_t t_float = (e2 - s2) / 1000;  // ms

//     // =========================
//     // 3) งาน IO (จำลอง I/O ช้า ๆ)
//     // =========================
//     int64_t s3 = esp_timer_get_time();

//     // จำลองงาน I/O โดยการ delay รวม ~1 วินาที
//     for (int i = 0; i < 10; i++) {
//         vTaskDelay(pdMS_TO_TICKS(100));  // 100 ms x 10 = 1000 ms
//     }

//     int64_t e3 = esp_timer_get_time();
//     int64_t t_io = (e3 - s3) / 1000;  // ms

//     // --- จับเวลาทั้งหมดเสร็จ ---
//     int64_t end_all = esp_timer_get_time();
//     int64_t t_total = (end_all - start_all) / 1000;  // ms

//     // =========================
//     // สรุปผลรอบเดียว
//     // =========================
//     ESP_LOGI(TAG, "===== SINGLE-THREAD RESULT =====");
//     ESP_LOGI(TAG, " INT   : %lld ms (sum=%u)", t_int, sum);
//     ESP_LOGI(TAG, " FLOAT : %lld ms (x=%.4f)", t_float, x);
//     ESP_LOGI(TAG, " IO    : %lld ms", t_io);
//     ESP_LOGI(TAG, " TOTAL : %lld ms", t_total);
//     ESP_LOGI(TAG, "================================");

//     // กันโปรแกรมหลุดออกจาก app_main ทันที
//     while (1) {
//         vTaskDelay(pdMS_TO_TICKS(1000));
//     }
// }

//////////////////////////////////////SINGLE CORE AND MUTI THREAD VERSION////////////////////////////////////////
#include <stdio.h>
#include <stdint.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

static const char *TAG = "Main3";

// ตัวแปรเก็บผลลัพธ์
volatile int64_t g_time_int = 0;
volatile int64_t g_time_float = 0;
volatile int64_t g_time_io = 0;

volatile bool g_done_int = false;
volatile bool g_done_float = false;
volatile bool g_done_io = false;

void task_int(void *arg)
{
    int64_t start = esp_timer_get_time();

    uint32_t sum = 0;
    for (int i = 0; i < 200000; i++) {
        sum += i * 3;
    }

    int64_t end = esp_timer_get_time();
    g_time_int = (end - start) / 1000;
    g_done_int = true;

    ESP_LOGI("INT", "Finished on core %d, time = %lld ms", 
             xPortGetCoreID(), g_time_int);

    vTaskDelete(NULL);
}

void task_float(void *arg)
{
    int64_t start = esp_timer_get_time();

    float x = 1.0f;
    for (int i = 0; i < 120000; i++) {
        x = x * 1.00001f + 0.000001f;
    }

    int64_t end = esp_timer_get_time();
    g_time_float = (end - start) / 1000;
    g_done_float = true;

    ESP_LOGI("FLOAT", "Finished on core %d, time = %lld ms",
             xPortGetCoreID(), g_time_float);

    vTaskDelete(NULL);
}

void task_io(void *arg)
{
    int64_t start = esp_timer_get_time();

    for (int i = 0; i < 10; i++) {
        vTaskDelay(pdMS_TO_TICKS(100));  // simulate IO wait
    }

    int64_t end = esp_timer_get_time();
    g_time_io = (end - start) / 1000;
    g_done_io = true;

    ESP_LOGI("IO", "Finished on core %d, time = %lld ms",
             xPortGetCoreID(), g_time_io);

    vTaskDelete(NULL);
}

void task_monitor(void *arg)
{
    ESP_LOGI("MON", "Waiting for tasks... core=%d", xPortGetCoreID());

    while (!(g_done_int && g_done_float && g_done_io)) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    ESP_LOGI("MON", "========= SUMMARY (Single-Core Multithread) =========");
    ESP_LOGI("MON", " INT   : %lld ms", g_time_int);
    ESP_LOGI("MON", " FLOAT : %lld ms", g_time_float);
    ESP_LOGI("MON", " IO    : %lld ms", g_time_io);

    int64_t max_time = g_time_int;
    if (g_time_float > max_time) max_time = g_time_float;
    if (g_time_io > max_time) max_time = g_time_io;

    ESP_LOGI("MON", " Total time (single core) ≈ %lld ms (bottleneck)", max_time);
    ESP_LOGI("MON", "=======================================================");

    vTaskDelete(NULL);
}

void app_main(void)
{
    ESP_LOGI(TAG, "Starting Single-Core Multithread test!");

    // ทุก Task รันบน Core 0
    xTaskCreatePinnedToCore(task_int,     "INT",     4096, NULL, 3, NULL, 0);
    xTaskCreatePinnedToCore(task_float,   "FLOAT",   4096, NULL, 3, NULL, 0);
    xTaskCreatePinnedToCore(task_io,      "IO",      4096, NULL, 3, NULL, 0);
    xTaskCreatePinnedToCore(task_monitor, "MONITOR", 4096, NULL, 2, NULL, 0);
}
