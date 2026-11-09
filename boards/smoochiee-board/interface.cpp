#include "core/powerSave.h"

#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <soc/adc_channel.h>
#include <soc/soc_caps.h>

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/

// Power handler for battery detection
#include <Wire.h>
#include <XPowersLib.h>
XPowersPPM PPM;

void _setup_gpio() {

    pinMode(UP_BTN, INPUT_PULLUP); // Добавил INPUT_PULLUP для внутреннего подтягивания
    pinMode(SEL_BTN, INPUT_PULLUP);
    pinMode(DW_BTN, INPUT_PULLUP);
    pinMode(R_BTN, INPUT_PULLUP);
    pinMode(L_BTN, INPUT_PULLUP);
    //  pinMode(BAT_PIN, INPUT);
    pinMode(CC1101_SS_PIN, OUTPUT);
    pinMode(NRF24_SS_PIN, OUTPUT);

    digitalWrite(CC1101_SS_PIN, HIGH);
    digitalWrite(NRF24_SS_PIN, HIGH);
    // Starts SPI instance for CC1101 and NRF24 with CS pins blocking communication at start

    bruceConfig.rfModule = CC1101_SPI_MODULE;
    bruceConfig.irRx = RXLED;
    Wire.setPins(GROVE_SDA, GROVE_SCL);
    Wire.begin();
    bool pmu_ret = false;

    pmu_ret = PPM.init(Wire, GROVE_SDA, GROVE_SCL, BQ25896_SLAVE_ADDRESS);
    if (pmu_ret) {
        PPM.setSysPowerDownVoltage(3300);
        PPM.setInputCurrentLimit(3250);
        Serial.printf("getInputCurrentLimit: %d mA\n", PPM.getInputCurrentLimit());
        PPM.disableCurrentLimitPin();
        PPM.setChargeTargetVoltage(4208);
        PPM.setPrechargeCurr(64);
        PPM.setChargerConstantCurr(832);
        PPM.getChargerConstantCurr();
        Serial.printf("getChargerConstantCurr: %d mA\n", PPM.getChargerConstantCurr());
        PPM.enableADCMeasure();
        PPM.enableCharge();
        PPM.enableOTG();
        PPM.disableOTG();
    }
}

/***************************************************************************************
** Function name: getBattery()
** location: display.cpp
** Description:   Delivers the battery value from 1-100+
***************************************************************************************/
int getBattery() {
    uint8_t percent;
    uint8_t _batAdcCh = ADC1_CHANNEL_7; // GPIO8
    uint8_t _batAdcUnit = 1;

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten((adc1_channel_t)_batAdcCh, ADC_ATTEN_DB_12);
    static esp_adc_cal_characteristics_t *adc_chars = nullptr;
    static constexpr int BASE_VOLATAGE = 3600;
    adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(
        (adc_unit_t)_batAdcUnit, ADC_ATTEN_DB_12, ADC_WIDTH_BIT_12, BASE_VOLATAGE, adc_chars
    );
    int raw;
    raw = adc1_get_raw((adc1_channel_t)_batAdcCh);
    uint32_t volt = esp_adc_cal_raw_to_voltage(raw, adc_chars);

    float mv = volt * 2;
    percent = (mv - 3300) * 100 / (float)(4150 - 3350);

    return (percent < 0) ? 0 : (percent >= 100) ? 100 : percent;
}
/*********************************************************************
** Function: setBrightness
** location: settings.cpp
** set brightness value
**********************************************************************/
void _setBrightness(uint8_t brightval) {
    if (brightval == 0) {
        analogWrite(TFT_BL, brightval);
    } else {
        int bl = MINBRIGHT + round(((255 - MINBRIGHT) * brightval / 100));
        analogWrite(TFT_BL, bl);
    }
}

/*********************************************************************
** Function: InputHandler
** Handles the variables PrevPress, NextPress, SelPress, AnyKeyPress and EscPress
** Поддержка авто-листания при удержании > 1.5 секунд с интервалом 0.75с
**********************************************************************/
void InputHandler(void) {
    static unsigned long pressStartTimeUp = 0;
    static unsigned long pressStartTimeDown = 0;
    static unsigned long pressStartTimeLeft = 0;
    static unsigned long pressStartTimeRight = 0;
    static unsigned long pressStartTimeSel = 0;

    static unsigned long lastAutoRepeatUp = 0;
    static unsigned long lastAutoRepeatDown = 0;
    static unsigned long lastAutoRepeatLeft = 0;
    static unsigned long lastAutoRepeatRight = 0;
    static unsigned long lastAutoRepeatSel = 0;

    static bool wasPressedUp = false;
    static bool wasPressedDown = false;
    static bool wasPressedLeft = false;
    static bool wasPressedRight = false;
    static bool wasPressedSel = false;

    const unsigned long holdThreshold = 900;      // 1.5 секунды для удержания
    const unsigned long autoRepeatInterval = 150; // 0.75 секунды между повторными срабатываниями

    unsigned long now = millis();

    // Кнопки активны LOW, внутренний подтягивающий резистор включён
    bool currUp = digitalRead(UP_BTN) == LOW;
    bool currDown = digitalRead(DW_BTN) == LOW;
    bool currLeft = digitalRead(L_BTN) == LOW;
    bool currRight = digitalRead(R_BTN) == LOW;
    bool currSel = digitalRead(SEL_BTN) == LOW;

    // Лямбда для обработки кнопки с авто-листанием
    auto handleButton = [&](bool currState,
                            bool &wasPressed,
                            unsigned long &pressStartTime,
                            unsigned long &lastAutoRepeat,
                            void (*onPress)()) {
        if (currState) {
            if (!wasPressed) {
                // Первое нажатие
                pressStartTime = now;
                wasPressed = true;
                onPress(); // Одноразовое срабатывание
                lastAutoRepeat = now;
            } else {
                // Удержание
                if (now - pressStartTime > holdThreshold) {
                    if (now - lastAutoRepeat > autoRepeatInterval) {
                        lastAutoRepeat = now;
                        onPress();
                    }
                }
            }
        } else {
            // Кнопка отпущена
            wasPressed = false;
        }
    };

    // Обработка каждой кнопки
    handleButton(currUp, wasPressedUp, pressStartTimeUp, lastAutoRepeatUp, []() {
        if (!wakeUpScreen()) AnyKeyPress = true;
        UpPress = true;
        PrevPagePress = true;
    });

    handleButton(currDown, wasPressedDown, pressStartTimeDown, lastAutoRepeatDown, []() {
        if (!wakeUpScreen()) AnyKeyPress = true;
        DownPress = true;
        NextPagePress = true;
    });

    handleButton(currLeft, wasPressedLeft, pressStartTimeLeft, lastAutoRepeatLeft, []() {
        if (!wakeUpScreen()) AnyKeyPress = true;
        PrevPress = true;
    });

    handleButton(currRight, wasPressedRight, pressStartTimeRight, lastAutoRepeatRight, []() {
        if (!wakeUpScreen()) AnyKeyPress = true;
        NextPress = true;
    });

    handleButton(currSel, wasPressedSel, pressStartTimeSel, lastAutoRepeatSel, []() {
        if (!wakeUpScreen()) AnyKeyPress = true;
        SelPress = true;
    });

    // Одновременное нажатие L и R - EscPress без авто-листания
    if (currLeft && currRight) {
        EscPress = true;
        NextPress = false;
        PrevPress = false;
    }
}

/*********************************************************************
** Function: powerOff
** location: mykeyboard.cpp
** Turns off the device (or try to)
**********************************************************************/
void powerOff() {
    esp_sleep_enable_ext0_wakeup((gpio_num_t)SEL_BTN, BTN_ACT);
    esp_deep_sleep_start();
}

/*********************************************************************
** Function: checkReboot
** location: mykeyboard.cpp
** Btn logic to turnoﬀ the device (name is odd btw)
**********************************************************************/
void checkReboot() {
    int countDown;
    /* Long press power off */
    if (digitalRead(L_BTN) == BTN_ACT && digitalRead(R_BTN) == BTN_ACT) {
        uint32_t time_count = millis();
        while (digitalRead(L_BTN) == BTN_ACT && digitalRead(R_BTN) == BTN_ACT) {
            // Display poweroff bar only if holding button
            if (millis() - time_count > 500) {
                tft.setTextSize(1);
                tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
                countDown = (millis() - time_count) / 1000 + 1;
                if (countDown < 4)
                    tft.drawCentreString("PWR OFF IN " + String(countDown) + "/3", tftWidth / 2, 12, 1);
                else {
                    tft.fillScreen(bruceConfig.bgColor);
                    while (digitalRead(L_BTN) == BTN_ACT || digitalRead(R_BTN) == BTN_ACT);
                    delay(200);
                    powerOff();
                }
                delay(10);
            }
        }

        // Clear text after releasing the button
        delay(30);
        tft.fillRect(60, 12, tftWidth - 60, tft.fontHeight(1), bruceConfig.bgColor);
    }
}

/***************************************************************************************
** Function name: isCharging()
** Description:   Determines if the device is charging
***************************************************************************************/
// Проверка процесса зарядки по уровню на GPIO3 (например, CHG/STAT пин)
#define CHARGE_STAT_PIN 3
bool isCharging() {
    pinMode(CHARGE_STAT_PIN, INPUT);
    // Теперь: HIGH — зарядка, LOW — не заряжается
    return digitalRead(CHARGE_STAT_PIN) == HIGH;
}
