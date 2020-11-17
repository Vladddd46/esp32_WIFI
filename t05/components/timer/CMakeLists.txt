idf_component_register(SRCS "tz_set.c" "digital_clock.c" "time_synchronization.c"
                    INCLUDE_DIRS "."
                    REQUIRES libmx utils sh1106 nvs_flash connect_to_wifi input)