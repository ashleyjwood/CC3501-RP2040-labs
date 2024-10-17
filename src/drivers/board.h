#pragma once

// GPIO Pins
const int TX{0};
const int RX{1};
const int ACCEL_SCLK{2};
const int ACCEL_MOSI{3};
const int ACCEL_MISO{4};
const int ACCEL_CS{5};
const int ACCEL_INT{6};
const int ACCEL_INT2{7};
const int GPIO8_UART1_TX{8};
const int GPIO9_UART1_RX{9};
const int GPIO10_UART1_CTS{10};
const int GPIO11_UART1_RTS{11};
const int SERVO_PIN{12};
const int LED_PIN{14};
const int SWITCH_PIN{15};
const int ACCEL_SDA{16};
const int ACCEL_SCL{17};
const int MIC_OUTPUT{26};
const int GPIO27_ADC1{27};
const int GPIO28_ADC2{28};
const int GPIO29_ADC3{29};

// Accelerometer
const auto ACCEL_I2C_INSTANCE = i2c0; // Define the I2C instance (i2c0 or i2c1)
const int ACCEL_I2C_ADDRESS{0x19};

// Other
const int NUMBER_OF_LEDS{12};
