idf_component_register(SRCS 
                        "main.c"
                        "Drivers/LED/led.c"
                        "Drivers/LCD/lcd.c"
                        "Drivers/LCD/lcd_init.c"
                        "Drivers/delay/delay.c"
                        "Drivers/button/button.c"
                        "./celeste/celeste.c"

                    INCLUDE_DIRS 
                                "./Drivers/LED"
                                "./Drivers/LCD"
                                "./Drivers/delay"
                                "./Drivers/button"
                                "./Drivers"
                                "./celeste"
                    )
