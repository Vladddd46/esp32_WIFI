#include "timer.h"

/* @ Synchronize time with SNTP server.
 * Gets time from remote server with time-zone and sets 
 * current time.
 * To change/set time-zone use `tz_set arg` command. 
 */
void time_synchronization(void *arg) {
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();

    time_t now;
    int retry;
    const int retry_count = 10;

    synchronized_time = 0;
    while(1) {
        // wait for time to be set
        now = 0;
        struct tm timeinfo = { 0 };
        retry = 0;
        while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
        time(&now);
        localtime_r(&now, &timeinfo);
        // char strftime_buf[64];
        // strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
        // printf("%s\n", strftime_buf);
        // printf("hour:%d min:%d sec:%d\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        synchronized_time = (timeinfo.tm_hour * 3600) + (timeinfo.tm_min * 60) + timeinfo.tm_sec;
        current_time = synchronized_time;
        // printf("=%d\n", synchronized_time);
        vTaskDelay(1000);
    }
}

