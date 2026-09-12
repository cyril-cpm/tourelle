
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

STR_UInt16 pos(0, "POS");
STR_Float pwm0Pos(0.0f, "0_POS");
STR_Float pwm1Pos(0.0f, "1_POS");
STR_Float pwm2Pos(0.0f, "2_POS");

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

	pwm0Pos.SetCallback([]() {  pwm0.write(pwm0Pos); });
	pwm1Pos.SetCallback([]() { pwm1.write(pwm1Pos); });
	pwm2Pos.SetCallback([]() { pwm2.write(pwm2Pos); });

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

