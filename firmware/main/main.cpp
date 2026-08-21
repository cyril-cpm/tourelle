
#include "esp_log.h"
#include "Settingator.h"
#include "CustomType.hpp"
#include "driver/gpio.h"
#include "Led.h"
#include "HP_ESP32Servo.h"
#include "STServo.h"

Settingator& STR = Settingator::GetInstance();

#define PWM_0_PIN GPIO_NUM_27
#define PWM_1_PIN GPIO_NUM_14
#define PWM_2_PIN GPIO_NUM_12

Servo pwm0(PWM_0_PIN);
Servo pwm1(PWM_1_PIN);
Servo pwm2(PWM_2_PIN);

STServoHandler STS(26, 25, 1000000, UART_NUM_2);
STServo servo(0x01);

STR_UInt16 pos(0, "POS");

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
	pos.SetCallback([](){ servo.SetTargetPosition(pos); });
	gpio_config_t pushButtonConfig = {
		.pin_bit_mask =
				(1ULL << HID_0) +
				(1ULL << HID_1),
		.mode = GPIO_MODE_INPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_ENABLE,
		.intr_type = GPIO_INTR_DISABLE,
	};

	ESP_ERROR_CHECK(gpio_config(&pushButtonConfig));

	ESP_ERROR_CHECK(gpio_install_isr_service(0));

	ESP_ERROR_CHECK(gpio_isr_handler_add(HID_0, buttonHandler, &hid0Pressed));
	ESP_ERROR_CHECK(gpio_isr_handler_add(HID_1, buttonHandler, &hid1Pressed));

	ESP_ERROR_CHECK(gpio_set_intr_type(HID_0, GPIO_INTR_POSEDGE));
	ESP_ERROR_CHECK(gpio_set_intr_type(HID_1, GPIO_INTR_POSEDGE));

	for (auto i = 0; i < LS_0_LEN; i++)
		Led::GetInstance().Strip0()[i] = RGB(255, 0, 0);

	for (auto i = 0; i < LS_1_LEN; i++)
		Led::GetInstance().Strip1()[i] = RGB(0, 0, 255);


	InitCores();
	STR.begin();

	pwm0.write(10);
	pwm1.write(90.0f);
	pwm2.write(170.0f);

	servo.SetMaxAngle(32737);
	servo.SetTargetPosition(8192);

	ESP_LOGI("MAIN", "TEST");
	while (true)
	{
		Led::GetInstance().Show();
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

