
#include "esp_log.h"
#include "Settingator.h"
#include "CustomType.hpp"
#include "driver/gpio.h"
#include "Led.h"
#include "HP_ESP32Servo.h"
#include "STServo.h"

Settingator& STR = Settingator::GetInstance();

#define PWM_0_PIN GPIO_NUM_12
#define PWM_1_PIN GPIO_NUM_14
#define PWM_2_PIN GPIO_NUM_27

Servo pwm0(PWM_0_PIN, true);
Servo pwm1(PWM_1_PIN, true);
Servo pwm2(PWM_2_PIN, true);

STServoHandler STS(26, 25, 1000000, UART_NUM_2);
STServo servo(0x01);

STR_UInt8 shoot(0, "SHOOT");

TimerHandle_t shooting0Timer;
TimerHandle_t shooting1Timer;
TimerHandle_t shooting2Timer;

#define HID_0 GPIO_NUM_16
#define HID_1 GPIO_NUM_17

bool hid1Pressed = false;
bool hid0Pressed = false;

static void IRAM_ATTR buttonHandler(void* args)
{
	if (args)
		*((bool*)args) = true;
}

extern "C" void app_main(void)
{
	gpio_config_t pushButtonConfig = {
		.pin_bit_mask =
				(1ULL << HID_0) +
				(1ULL << HID_1),
		.mode = GPIO_MODE_INPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_ENABLE,
		.intr_type = GPIO_INTR_DISABLE,
	};

	// ESP_ERROR_CHECK(gpio_config(&pushButtonConfig));
	//
	// ESP_ERROR_CHECK(gpio_install_isr_service(0));
	//
	// ESP_ERROR_CHECK(gpio_isr_handler_add(HID_0, buttonHandler, &hid0Pressed));
	// ESP_ERROR_CHECK(gpio_isr_handler_add(HID_1, buttonHandler, &hid1Pressed));
	//
	// ESP_ERROR_CHECK(gpio_set_intr_type(HID_0, GPIO_INTR_POSEDGE));
	// ESP_ERROR_CHECK(gpio_set_intr_type(HID_1, GPIO_INTR_POSEDGE));

	pwm0.begin();
	pwm1.begin();
	pwm2.begin();

	InitCores();
	STR.begin();

	servo.SetMaxAngle(32737);
	servo.SetTargetPosition(8192);

	shooting0Timer = xTimerCreate(
			"shooting0Timer",
			pdMS_TO_TICKS(1000),
			pdFALSE,
			nullptr,
			[](TimerHandle_t timer) {
				pwm0.write(0.0f);
			}
		);

	shooting1Timer = xTimerCreate(
			"shooting1Timer",
			pdMS_TO_TICKS(1000),
			pdFALSE,
			nullptr,
			[](TimerHandle_t timer) {
				pwm1.write(0.0f);
			}
		);

	shooting2Timer = xTimerCreate(
			"shooting2Timer",
			pdMS_TO_TICKS(1000),
			pdFALSE,
			nullptr,
			[](TimerHandle_t timer) {
				pwm2.write(0.0f);
			}
		);

	shoot.SetCallback([]() {
			if (shoot & 0b001)
			{
				pwm0.write(25.0f);
				xTimerStart(shooting0Timer, 0);
			}
			
			if (shoot & 0b010)
			{
				pwm1.write(25.0f);
				xTimerStart(shooting1Timer, 0);
			}

			if (shoot & 0b10)
			{
				pwm2.write(25.0f);
				xTimerStart(shooting2Timer, 0);
			}
		});

	ESP_LOGI("MAIN", "TEST");
	while (true)
	{
		STR.Update();

		if (hid0Pressed)
		{
			hid0Pressed = false;
			ESP_LOGI("BUTTON", "HID_0");
		}
		if (hid1Pressed)
		{
			hid1Pressed = false;
			ESP_LOGI("BUTTON", "HID_1");
		}
	}
}

