#ifndef CONFIG_H
#define CONFIG_H

// -------------------------------
// LoRa Configuration
// -------------------------------
// G0(INT)->2, RST->9, CS->10, MOSI->51, MISO->50, SCK->52
constexpr uint8_t LoRa_INT = 2;
constexpr uint8_t LoRa_RST = 9;
constexpr uint8_t LoRa_CS = 10;

// -------------------------------
// 7 Segment Display Configurations
// -------------------------------
constexpr uint8_t SevSD_DIN = 42;
constexpr uint8_t SevSD_CLK = 44;
constexpr uint8_t SevSD_CS = 46;
constexpr uint8_t NUM_SevSD = 4;

// -------------------------------
// LCD Screen Configurations
// -------------------------------
constexpr uint8_t LCD_SDA = 20;
constexpr uint8_t LCD_SCK = 21;  // This is the SCL pin
constexpr uint8_t NUM_LCD = 2;

// Added the specific I2C addresses for your two screens here:
// constexpr uint16_t LCD_Address[NUM_LCD] = {0x3E, 0x27};


// -------------------------------
// Switch and Button Inputs
// -------------------------------
// Note: Ensure NUM_SERVO and NUM_EMATCH are defined in your packets.h
constexpr uint8_t Pin_Servos[4] = { 30, 31, 32, 33 };
constexpr uint8_t Pin_Solenoid = 34;
constexpr uint8_t Pin_Ematches[2] = { 35, 36 };
constexpr uint8_t Pin_Arm = 37;
constexpr uint8_t Pin_AbortSwitch = 38;
constexpr uint8_t Pin_LoadCellTare = 39;
constexpr uint8_t Pin_SpareTare = 40;

#endif