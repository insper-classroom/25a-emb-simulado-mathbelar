/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h> 
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/irq.h"

const int sensor1_trig = 13;
const int sensor1_echo = 12;
volatile uint64_t tempo_inicial_1; // tem que ser volatile para ser atualizado no callback
volatile uint64_t tempo_final_1;


const int sensor2_trig = 19;
const int sensor2_echo = 18;
volatile uint64_t tempo_inicial_2;
volatile uint64_t tempo_final_2;
// trigger envia
// echo recebe

void sensor_Callback(uint gpio, uint32_t events){
    if(gpio == sensor1_echo){
        if(events & GPIO_IRQ_EDGE_RISE){
            tempo_inicial_1 = time_us_64(); 
        }
        if(events & GPIO_IRQ_EDGE_FALL){
            tempo_final_1 = time_us_64();
        }
    }
    if(gpio == sensor2_echo){
        if(events & GPIO_IRQ_EDGE_RISE){
            tempo_inicial_2 = time_us_64(); 
        }
        if(events & GPIO_IRQ_EDGE_FALL){
            tempo_final_2 = time_us_64();
        }
    }


}

int main() {
    stdio_init_all();

    gpio_init(sensor1_trig);
    gpio_set_dir(sensor1_trig, GPIO_OUT);
    gpio_init(sensor1_echo);
    gpio_set_dir(sensor1_echo, GPIO_IN);

    gpio_set_irq_enabled_with_callback( sensor1_echo, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &sensor_Callback);

    gpio_init(sensor2_trig);
    gpio_set_dir(sensor2_trig, GPIO_OUT);
    gpio_init(sensor2_echo);
    gpio_set_dir(sensor2_echo, GPIO_IN);

    gpio_set_irq_enabled_with_callback( sensor2_echo, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &sensor_Callback);

    while (true) {

        gpio_put(sensor1_trig, 0); //low
        sleep_us(10);
        gpio_put(sensor1_trig, 1); //high
        sleep_us(10);
        gpio_put(sensor1_trig, 0);
        sleep_us(10);

        gpio_put(sensor2_trig, 0); //low
        sleep_us(10);
        gpio_put(sensor2_trig, 1); //high
        sleep_us(10);
        gpio_put(sensor2_trig, 0);
        sleep_us(10);

        if (tempo_final_1 > tempo_inicial_1) {
            int dist_1 = (tempo_final_1 - tempo_inicial_1) * 0.034 / 2; 
            printf("Sensor 1 - dist: %d cm\n", dist_1);
        } else {
            printf("Sensor 1 - dist: FALHA\n");
        }

        if (tempo_final_2 > tempo_inicial_2) {
            int dist_2 = (tempo_final_2 - tempo_inicial_2) * 0.034 / 2; 
            printf("Sensor 2 - dist: %d cm\n", dist_2);
        } else {
            printf("Sensor 2 - dist: FALHA\n");
        }

        sleep_ms(100);
      
    }

    return 0;
}
