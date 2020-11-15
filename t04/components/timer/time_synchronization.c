#include "timer.h"

void time_synchronization(void *arg) {
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();

    while(1) {
        // wait for time to be set
        time_t now = 0;
        struct tm timeinfo = { 0 };
        int retry = 0;

        const int retry_count = 10;
        while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
        time(&now);
        localtime_r(&now, &timeinfo);

        char strftime_buf[64];
        printf("hour:%d min:%d sec:%d\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
        printf("%s\n", strftime_buf);
        vTaskDelay(1000);
    }
}