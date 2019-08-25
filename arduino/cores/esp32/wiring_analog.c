/*
  This file is part of the Arduino NINA firmware.
  Copyright (c) 2018 Arduino SA. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <driver/ledc.h>

#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "wiring_analog.h"

void analogWrite(uint32_t pin, uint32_t value)
{
  periph_module_enable(PERIPH_LEDC_MODULE);

  ledc_timer_config_t timerConf = {
    .bit_num = LEDC_TIMER_10_BIT,
    .freq_hz = 1000,
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .timer_num = (pin / 7),
  };
  ledc_timer_config(&timerConf);

  ledc_channel_config_t ledc_conf = {
    .channel = (pin % 7),
    .duty = (value << 2),
    .gpio_num = pin,
    .intr_type = LEDC_INTR_DISABLE,
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .timer_sel = (pin / 7)
  };
  ledc_channel_config(&ledc_conf);
}

#define DEFAULT_VREF  1100
#define NO_OF_SAMPLES 10
static esp_adc_cal_characteristics_t *adc_chars;
static const adc_atten_t atten = ADC_ATTEN_DB_0;
static const adc_unit_t unit = ADC_UNIT_1;

uint16_t analogRead(uint8_t pin)
{
  adc1_channel_t channel = (adc1_channel_t) pin;
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(channel, atten);

  if(adc_chars == NULL)
  {
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
  }
  uint32_t adc_reading = 0;
  adc_reading = adc1_get_raw(channel);

  return (uint16_t)adc_reading;
}
