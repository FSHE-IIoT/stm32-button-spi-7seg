/*
 * btn_counter.c
 *
 *  Created on: Jul 23, 2025
 *      Author: FSHI_IIoT
 */
#include "stm32f4xx_hal.h"
#include "btn_counter.h"

// Number of digits in 7-segment encoding table
// Количество цифр в таблице кодирования 7-сегментного индикатора
#define NUMS 10

/**
 * @brief 7-segment display encoding table (common anode)
 * Таблица кодирования для 7-сегментного индикатора (общий анод)
 * Bits: 0-G, 1-F, 2-E, 3-D, 4-C, 5-B, 6-A
 * Биты: 0-G, 1-F, 2-E, 3-D, 4-C, 5-B, 6-A
 */
const uint8_t NUMS_TABLE[NUMS] =
{
	0x3F, // 0 - segments A,B,C,D,E,F / сегменты A,B,C,D,E,F
	0x06, // 1 - segments B,C / сегменты B,C
	0x5B, // 2 - segments A,B,D,E,G / сегменты A,B,D,E,G
	0x4F, // 3 - segments A,B,C,D,G / сегменты A,B,C,D,G
	0x66, // 4 - segments B,C,F,G / сегменты B,C,F,G
	0x6D, // 5 - segments A,C,D,F,G / сегменты A,C,D,F,G
	0x7D, // 6 - segments A,C,D,E,F,G / сегменты A,C,D,E,F,G
	0x07, // 7 - segments A,B,C / сегменты A,B,C
	0x7F, // 8 - all segments / все сегменты
	0x6F  // 9 - segments A,B,C,D,F,G / сегменты A,B,C,D,F,G
};

// Module variables / Переменные модуля
static GPIO_TypeDef * port=NULL;			// LED GPIO port / Порт GPIO светодиода
static GPIO_TypeDef * units_port=NULL;		// Units digit port / Порт разряда единиц
static GPIO_TypeDef * tens_port=NULL;		// Tens digit port / Порт разряда десятков
static GPIO_TypeDef * hundreds_port=NULL;	// Hundreds digit port / Порт разряда сотен
static GPIO_TypeDef * sc_port=NULL;			// Chip select port / Порт выбора чипа

static uint16_t pin = ~0;					// LED pin / Пин светодиода
static uint16_t btn_pin = ~0;				// Button pin / Пин кнопки
static uint16_t units_pin = ~0;				// Units digit pin / Пин разряда единиц
static uint16_t tens_pin = ~0;				// Tens digit pin / Пин разряда десятков
static uint16_t hundreds_pin = ~0;			// Hundreds digit pin / Пин разряда сотен
static uint16_t sc_pin = ~0;				// Chip select pin / Пин выбора чипа

static uint16_t counter = 0;				// Button press counter / Счётчик нажатий кнопки
static SPI_HandleTypeDef *spi = NULL;		// SPI handler pointer / Указатель на обработчик SPI


/**
 * @brief Initialize LED control
 * Инициализация управления светодиодом
 * @param _port GPIO port for LED / Порт GPIO для светодиода
 * @param _pin GPIO pin for LED / Пин GPIO для светодиода
 */
void led_init(GPIO_TypeDef* _port, uint16_t _pin)
{
	port = _port;
	pin = _pin;
}


/**
 * @brief Toggle LED state
 * Переключение состояния светодиода
 */
static void toggle_led()
{
	if (port)
	{
		HAL_GPIO_TogglePin(port, pin);
	}
}


/**
 * @brief Initialize button control
 * Инициализация управления кнопкой
 * @param _pin GPIO pin for button / Пин GPIO для кнопки
 */
void btn_init(uint16_t _pin)
{
    btn_pin = _pin;
}


/**
 * @brief Initialize SPI transmission
 * Инициализация передачи по SPI
 * @param _spi SPI handler / Обработчик SPI
 * @param _sc_port GPIO port for chip select / Порт GPIO для выбора чипа
 * @param _sc_pin GPIO pin for chip select / Пин GPIO для выбора чипа
 */
void transmit_init(SPI_HandleTypeDef* _spi, GPIO_TypeDef * _sc_port, uint16_t _sc_pin)
{
	spi = _spi;
	sc_port = _sc_port;
	sc_pin = _sc_pin;
}


/**
 * @brief Transmit number to 7-segment display via SPI
 * Передача числа на 7-сегментный индикатор по SPI
 * @param transmit_nums Number to transmit (0-9) / Число для передачи (0-9)
 */
void transmit(uint16_t transmit_nums)
{
	// Activate chip select / Активировать выбор чипа
    HAL_GPIO_WritePin(sc_port, sc_pin, GPIO_PIN_SET);
    // Get 7-segment code from table / Получить код из таблицы для 7-сегментного индикатора
    uint8_t num = NUMS_TABLE[transmit_nums % NUMS]; // Для предотвращения переполнения
    // Transmit data via SPI / Передать данные по SPI
    HAL_SPI_Transmit(spi, &num, 1, 1);
    // Deactivate chip select / Деактивировать выбор чипа
    HAL_GPIO_WritePin(sc_port, sc_pin, GPIO_PIN_RESET);
}


/**
 * @brief Initialize display configuration
 * Инициализация конфигурации дисплея
 * @param display_config Display configuration structure / Структура конфигурации дисплея
 */
void display_init(Display_InitTypeDef *display_config)
{
    units_port = display_config->units_port;
    units_pin = display_config->units_pin;
    tens_port = display_config->tens_port;
    tens_pin = display_config->tens_pin;
    hundreds_port = display_config->hundreds_port;
    hundreds_pin = display_config->hundreds_pin;
}


/**
 * @brief Get current counter value
 * Получить текущее значение счётчика
 * @return Current counter value / Текущее значение счётчика
 */
uint16_t get_counter(void)
{
	return counter;
}


/**
 * @brief Update display with current number using multiplexing
 * Обновление дисплея текущим числом с использованием мультиплексирования
 * @param number Number to display (0-999) / Число для отображения (0-999)
 */
void update_display(uint16_t number)
{
    uint8_t digits[3];

    // Extract digits from number / Извлечь цифры из числа
    digits[0] = number % 10;          // Units / Единицы
    digits[1] = (number / 10) % 10;   // Tens / Десятки
    digits[2] = (number / 100) % 10;  // Hundreds / Сотни

    // Display hundreds digit / Отобразить сотни
    HAL_GPIO_WritePin(hundreds_port, hundreds_pin, GPIO_PIN_SET);
    transmit(digits[2]);
    HAL_Delay(1);
    HAL_GPIO_WritePin(hundreds_port, hundreds_pin, GPIO_PIN_RESET);

    // Display tens digit / Отобразить десятки
    HAL_GPIO_WritePin(tens_port, tens_pin, GPIO_PIN_SET);
    transmit(digits[1]);
    HAL_Delay(1);
    HAL_GPIO_WritePin(tens_port, tens_pin, GPIO_PIN_RESET);

    // Display units digit / Отобразить единицы
    HAL_GPIO_WritePin(units_port, units_pin, GPIO_PIN_SET);
    transmit(digits[0]);
    HAL_Delay(1);
    HAL_GPIO_WritePin(units_port, units_pin, GPIO_PIN_RESET);
}


/**
 * @brief EXTI callback function for button press
 * Функция обратного вызова EXTI для нажатия кнопки
 * @param GPIO_Pin GPIO pin that triggered interrupt / Пин GPIO, вызвавший прерывание
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == btn_pin)
	{
		toggle_led();						// Toggle LED on button press / Переключить светодиод при нажатии
		counter++;							// Increment counter / Увеличить счётчик

		// Reset counter if exceeds 999 / Сбросить счётчик, если превысил 999
	    if (counter > 999) counter = 0;
	}
}
