
#include "esp_log.h"
#include "Settingator.h"
#include "CustomType.hpp"
#include "driver/gpio.h"
#include "Led.h"

#define R_LED GPIO_NUM_12
#define B_LED GPIO_NUM_27
#define Y_LED GPIO_NUM_25
#define G_LED GPIO_NUM_32

#define R_BUTTON GPIO_NUM_13
#define B_BUTTON GPIO_NUM_14
#define Y_BUTTON GPIO_NUM_26
#define G_BUTTON GPIO_NUM_33

#define R_NOTIF 0x01
#define B_NOTIF 0x02
#define Y_NOTIF 0x03
#define G_NOTIF 0x04

#define HID_0 GPIO_NUM_16
#define HID_1 GPIO_NUM_17

Settingator& STR = Settingator::GetInstance();

STR_Bool redLight(false, "RED_LIGHT");
STR_Bool blueLight(false, "BLUE_LIGHT");
STR_Bool yellowLight(false, "YELLOW_LIGHT");
STR_Bool greenLight(false, "GREEN_LIGHT");

bool redPressed = false;
bool bluePressed = false;
bool yellowPressed = false;
bool greenPressed = false;
bool hid1Pressed = false;
bool hid0Pressed = false;

static void IRAM_ATTR buttonHandler(void* args)
{
	if (args)
		*((bool*)args) = true;
}

extern "C" void app_main(void)
{
	gpio_config_t ledButtonConfig = {
		.pin_bit_mask =
				(1ULL << R_LED) +
				(1ULL << B_LED) +
				(1ULL << Y_LED) +
				(1ULL << G_LED),
		.mode = GPIO_MODE_OUTPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_ENABLE,
		.intr_type = GPIO_INTR_DISABLE,
	};

	ESP_ERROR_CHECK(gpio_config(&ledButtonConfig));

	gpio_config_t pushButtonConfig = {
		.pin_bit_mask =
				(1ULL << R_BUTTON) +
				(1ULL << B_BUTTON) +
				(1ULL << Y_BUTTON) +
				(1ULL << G_BUTTON) +
				(1ULL << HID_0) +
				(1ULL << HID_1),
		.mode = GPIO_MODE_INPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_ENABLE,
		.intr_type = GPIO_INTR_DISABLE,
	};

	ESP_ERROR_CHECK(gpio_config(&pushButtonConfig));

	ESP_ERROR_CHECK(gpio_install_isr_service(0));

	ESP_ERROR_CHECK(gpio_isr_handler_add(R_BUTTON, buttonHandler, &redPressed));
	ESP_ERROR_CHECK(gpio_isr_handler_add(B_BUTTON, buttonHandler, &bluePressed));
	ESP_ERROR_CHECK(gpio_isr_handler_add(Y_BUTTON, buttonHandler, &yellowPressed));
	ESP_ERROR_CHECK(gpio_isr_handler_add(G_BUTTON, buttonHandler, &greenPressed));
	ESP_ERROR_CHECK(gpio_isr_handler_add(HID_0, buttonHandler, &hid0Pressed));
	ESP_ERROR_CHECK(gpio_isr_handler_add(HID_1, buttonHandler, &hid1Pressed));

	ESP_ERROR_CHECK(gpio_set_intr_type(R_BUTTON, GPIO_INTR_POSEDGE));
	ESP_ERROR_CHECK(gpio_set_intr_type(B_BUTTON, GPIO_INTR_POSEDGE));
	ESP_ERROR_CHECK(gpio_set_intr_type(Y_BUTTON, GPIO_INTR_POSEDGE));
	ESP_ERROR_CHECK(gpio_set_intr_type(G_BUTTON, GPIO_INTR_POSEDGE));
	ESP_ERROR_CHECK(gpio_set_intr_type(HID_0, GPIO_INTR_POSEDGE));
	ESP_ERROR_CHECK(gpio_set_intr_type(HID_1, GPIO_INTR_POSEDGE));

	redLight.SetCallback(
			[](){ ESP_ERROR_CHECK(gpio_set_level(R_LED, redLight)); }
		);

	blueLight.SetCallback(
			[](){ ESP_ERROR_CHECK(gpio_set_level(B_LED, blueLight)); }
		);

	yellowLight.SetCallback(
			[](){ ESP_ERROR_CHECK(gpio_set_level(Y_LED, yellowLight)); 
		});

	greenLight.SetCallback(
			[](){ ESP_ERROR_CHECK(gpio_set_level(G_LED, greenLight)); }
		);

	for (auto i = 0; i < LS_0_LEN; i++)
		Led::GetInstance().Strip0()[i] = RGB(255, 0, 0);

	for (auto i = 0; i < LS_1_LEN; i++)
		Led::GetInstance().Strip1()[i] = RGB(0, 0, 255);

	InitCores();
	STR.begin();

	ESP_LOGI("MAIN", "TEST");
	while (true)
	{
		Led::GetInstance().Show();
		STR.Update();

		if (redPressed)
		{
			redPressed = false;
			STR.SendNotif(R_NOTIF);
			ESP_LOGI("BUTTON", "RED");
		}
		if (bluePressed)
		{
			bluePressed = false;
			STR.SendNotif(B_NOTIF);
			ESP_LOGI("BUTTON", "BLUE");
		}
		if (yellowPressed)
		{
			yellowPressed = false;
			STR.SendNotif(Y_NOTIF);
			ESP_LOGI("BUTTON", "YELLOW");
		}
		if (greenPressed)
		{
			greenPressed = false;
			STR.SendNotif(G_NOTIF);
			ESP_LOGI("BUTTON", "GREEN");
		}
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

