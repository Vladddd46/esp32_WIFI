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
        synchronized_time = (timeinfo.tm_hour * 3600) + (timeinfo.tm_min * 60) + timeinfo.tm_sec;
        current_time = synchronized_time;
        vTaskDelay(1000);
    }
}

