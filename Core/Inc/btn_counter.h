/*
 * btn_counter.h
 *
 *  Created on: Jul 23, 2025
 *      Author: FSHI_IIoT
 */

#ifndef INC_BTN_COUNTER_H_
#define INC_BTN_COUNTER_H_

/**
 * @brief Display configuration structure
 * Структура конфигурации дисплея
 */
typedef struct {
    GPIO_TypeDef *units_port; 		/**< GPIO port for units digit / Порт GPIO для разряда единиц */
    uint16_t units_pin;				/**< GPIO pin for units digit / Пин GPIO для разряда единиц */
    GPIO_TypeDef *tens_port;		/**< GPIO port for tens digit / Порт GPIO для разряда десятков */
    uint16_t tens_pin;				/**< GPIO pin for tens digit / Пин GPIO для разряда десятков */
    GPIO_TypeDef *hundreds_port;	/**< GPIO port for hundreds digit / Порт GPIO для разряда сотен */
    uint16_t hundreds_pin;			/**< GPIO pin for hundreds digit / Пин GPIO для разряда сотен */
} Display_InitTypeDef;

// Function prototypes / Прототипы функций

/**
 * @brief Initialize LED control
 * Инициализация управления светодиодом
 * @param _port GPIO port for LED / Порт GPIO для светодиода
 * @param _pin GPIO pin for LED / Пин GPIO для светодиода
 */
void led_init(GPIO_TypeDef *_port, uint16_t _pin);

/**
 * @brief Initialize button control
 * Инициализация управления кнопкой
 * @param _pin GPIO pin for button / Пин GPIO для кнопки
 */
void btn_init(uint16_t _pin);

/**
 * @brief Initialize display with configuration
 * Инициализация дисплея с конфигурацией
 * @param display_config Display configuration structure / Структура конфигурации дисплея
 */
void display_init(Display_InitTypeDef *display_config);

/**
 * @brief Update display with a number
 * Обновление дисплея числом
 * @param number Number to display (0-999) / Число для отображения (0-999)
 */
void update_display(uint16_t number);

/**
 * @brief Initialize SPI transmission
 * Инициализация передачи по SPI
 * @param spi SPI handler / Обработчик SPI
 * @param _sc_port GPIO port for chip select / Порт GPIO для выбора чипа
 * @param _sc_pin GPIO pin for chip select / Пин GPIO для выбора чипа
 */
void transmit_init(SPI_HandleTypeDef* spi, GPIO_TypeDef * _sc_port, uint16_t _sc_pin);

/**
 * @brief Get current counter value
 * Получить текущее значение счётчика
 * @return Current counter value / Текущее значение счётчика
 */
uint16_t get_counter(void);


#endif /* INC_BTN_COUNTER_H_ */
