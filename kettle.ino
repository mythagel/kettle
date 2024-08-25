#include <Button2.h>
#include <Rotary.h>
#include <LEDMatrixDriver.hpp>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <AutoPID.h>
#include <avr/sleep.h>

//#define PID

#ifdef PID
#include <TimerOne.h>
#endif

constexpr int kEncoderAPin = 15;      // A1
constexpr int kEncoderBPin = 14;      // A0
constexpr int kEncoderButtonPin = 16; // A2
constexpr int kLEDChipSelect = 8;
constexpr int kOnewirePin = 5;
constexpr int kPWMPin = 9;

constexpr int kEncoderClicks = 4;
constexpr int kSensorResolution = 9;
constexpr int kSensorConversionMs = 750 / (1 << (12 - kSensorResolution));

constexpr int kMinTemp = 50;
constexpr int kMaxTemp = 100;
constexpr int kDefaultTemp = 74;

constexpr uint32_t kIdleOffMs = 30 * 1000;
constexpr uint32_t kSetTempMs = 2 * 1000;
constexpr uint32_t kHeatingHoldMs = 30 * 60 * 1000;
constexpr uint32_t kDisplayChangeMs = 5 * 1000;

constexpr unsigned kPIDOutputMin = 0;
constexpr unsigned kPIDOutputMax = 1023;
constexpr double KP = 5.8;
constexpr double KI = 0.015;
constexpr double KD = 38.0;

constexpr uint16_t kDigits[] = {
    0b0010101111101010,
    0b0010110010010111,
    0b0010101001010111,
    0b0110001011001110,
    0b0001011101111001,
    0b0111100110001110,
    0b0011100110101011,
    0b0111001001010010,
    0b0010101010101110,
    0b0110101011001110,
};

double previousTemperature;
double currentTemperature;
double targetTemperature;
#ifdef PID
double pidOutput;
#else
bool pidOutput;
#endif

enum class State
{
    Idle,
    IdleOff,
    SetTemp,
    Heating,
    Error,
};
State state;
uint32_t stateChange;
uint32_t sensorReadStart;

bool displayTargetTemperature = true;
uint32_t displayChange;
bool waitForNextTemperature = true;

Rotary encoder;
Button2 button;
OneWire oneWire(kOnewirePin);
DallasTemperature sensor(&oneWire);

LEDMatrixDriver ledMatrix(1, kLEDChipSelect);

#ifdef PID
AutoPID pid(&currentTemperature, &targetTemperature, &pidOutput, kPIDOutputMin, kPIDOutputMax, KP, KI, KD);
#else
AutoPIDRelay pid(&currentTemperature, &targetTemperature, &pidOutput, 5000, KP, KI, KD);
#endif

void setup()
{
    Serial.begin(9600);
    Serial.println("Starting");
    
    encoder.begin(kEncoderAPin, kEncoderBPin, kEncoderClicks, kMinTemp, kMaxTemp, kDefaultTemp);
    encoder.setChangedHandler(rotate);

    button.begin(kEncoderButtonPin);
    button.setTapHandler(click);
    button.setLongClickHandler(longClick);

    attachInterrupt(digitalPinToInterrupt(kEncoderButtonPin), clickInterrupt, CHANGE);

    sensor.begin();
    sensor.setResolution(kSensorResolution);
    sensor.setWaitForConversion(false);
    sensor.requestTemperatures();
    sensorReadStart = millis();

    ledMatrix.setEnabled(true);
    ledMatrix.setIntensity(5);

#ifdef PID
    Timer1.initialize(100000);
#else
    pinMode(kPWMPin, OUTPUT);
    digitalWrite(kPWMPin, HIGH);
#endif

    state = State::Idle;
    stateChange = millis();

    targetTemperature = kDefaultTemp;
    previousTemperature = kDefaultTemp;
    currentTemperature = kDefaultTemp;

    pid.setBangBang(4);
    pid.setTimeStep(500);
}

void loop()
{
    encoder.loop();
    button.loop();

    switch (state)
    {
        case State::Idle:
        {
            uint32_t deltaMs = millis() - stateChange;
            if (deltaMs > kIdleOffMs)
            {
                setState(State::IdleOff);
                break;
            }
            break;
        }
        case State::IdleOff:
        {
            delay(50);    // Delay to allow serial output to be read

            sleep_enable();
            set_sleep_mode(SLEEP_MODE_IDLE);
            sleep_mode();

            // Device will wake here
            sleep_disable();
            break;
        }
        case State::SetTemp:
        {
            if (millis() - stateChange > kSetTempMs)
            {
                setState(State::Heating);
                break;
            }
            break;
        }
        case State::Heating:
        {
            if (millis() - stateChange > kHeatingHoldMs)
            {
                setState(State::Idle);
                break;
            }

            if (millis() - sensorReadStart > kSensorConversionMs)
            {
                previousTemperature = currentTemperature;
                currentTemperature = sensor.getTempCByIndex(0);
                sensor.requestTemperatures();
                sensorReadStart = millis();
                if (currentTemperature != previousTemperature)
                    Serial.print("Current temperature: "); Serial.println(currentTemperature);
                waitForNextTemperature = false;
            }
            else if (waitForNextTemperature)
            {
                break;
            }

            if (currentTemperature == DEVICE_DISCONNECTED_C)
            {
                Serial.println("Temperature sensor disconnected");
                setState(State::Error);
                break;
            }

            if (currentTemperature <= 0)
            {
                Serial.println("Implausible temperature");
                setState(State::Error);
                break;
            }

            if (currentTemperature >= 103.0)
            {
                Serial.println("Overtemperature");
                setState(State::Error);
                break;
            }

            pid.run();
#ifdef PID
            Timer1.pwm(kPWMPin, pidOutput);
#else
            digitalWrite(kPWMPin, pidOutput ? LOW : HIGH);
#endif
            break;
        }
        case State::Error:
        {
            // TODO
            break;
        }
    }

    updateDisplay();
}

void setState(State newState)
{
    if (state == State::Error)
    {
        Serial.print(toString(state)); Serial.print(" -> "); Serial.println(toString(state));
        return;
    }
    else if (state == State::Heating)
    {
#ifdef PID
        Timer1.pwm(kPWMPin, 0);
#else
        digitalWrite(kPWMPin, HIGH);
#endif
    }

    if (state != newState)
        Serial.print(toString(state)); Serial.print(" -> "); Serial.println(toString(newState));

    state = newState;
    stateChange = millis();

    if (state == State::Heating)
    {
        Serial.print("Heating to "); Serial.println(targetTemperature);
        sensor.requestTemperatures();
        sensorReadStart = millis();
        waitForNextTemperature = true;

        displayTargetTemperature = false;
        displayChange = millis();
    }
}

void drawDigit(int n, int x, int y)
{
    //Serial.print("drawDigit "); Serial.print(n); Serial.print(" "); Serial.print(x); Serial.print(" "); Serial.println(y);
    if (n < 0 || n > 9)
        return;
    uint16_t digit = kDigits[n] << 1;
    for (unsigned y_pos = 0; y_pos < 5; ++y_pos)
    {
        for (unsigned x_pos = 0; x_pos < 3; ++x_pos)
        {
            if (digit & 0x8000)
                ledMatrix.setPixel(x + x_pos, y + y_pos, true);
            digit <<= 1;
        }
    }
}

void drawDigits(int n)
{
    if (n >= 100)
    {
        int d = n / 100;
        d = d % 10;
    }

    if (n >= 10)
    {
        int d = n / 10;
        d = d % 10;
        drawDigit(d, 1, 0);
    }

    int d = n;
    d = d % 10;
    drawDigit(d, 5, 0);
}

void updateDisplay()
{
    ledMatrix.clear();

    switch (state)
    {
        case State::Idle:
        {
            for (unsigned y = 6; y < 8; ++y)
            {
                for (unsigned x = 0; x < 8; ++x)
                    ledMatrix.setPixel(x, y, true);
            }
            break;
        }
        case State::IdleOff:
            break;
        case State::SetTemp:
        {
            drawDigits(targetTemperature);

            for (unsigned y = 6; y < 8; ++y)
            {
                for (unsigned x = 0; x < 4; ++x)
                    ledMatrix.setPixel(x, y, true);
            }
            break;
        }
        case State::Heating:
        {
            if (millis() - displayChange > kDisplayChangeMs)
            {
                displayTargetTemperature = ! displayTargetTemperature;
                displayChange = millis();
            }

            if (displayTargetTemperature)
            {
                drawDigits(targetTemperature);

                for (unsigned y = 6; y < 8; ++y)
                {
                    for (unsigned x = 0; x < 4; ++x)
                        ledMatrix.setPixel(x, y, true);
                }
            }
            else
            {
                drawDigits(currentTemperature);

                for (unsigned y = 6; y < 8; ++y)
                {
                    for (unsigned x = 4; x < 8; ++x)
                        ledMatrix.setPixel(x, y, true);
                }
            }
            break;
        }
        case State::Error:
        {
            for (unsigned y = 0; y < 8; ++y)
            {
                for (unsigned x = 0; x < 8; ++x)
                    ledMatrix.setPixel(x, y, ((y % 2) == 0) ? ((x % 2) == 0) : ((x % 2) != 0));
            }
            break;
        }
    }

    ledMatrix.display();
}

void rotate(Rotary& encoder)
{
    setState(State::SetTemp);
    targetTemperature = encoder.getPosition();
}
 
void clickInterrupt()
{
}
void click(Button2& button)
{
    Serial.println("Click");
    // woken by interrupt
}

void longClick(Button2& button)
{
    Serial.println("Long click");
    if (state == State::Idle)
        setState(State::Heating);
    else
        setState(State::Idle);
}

const char* toString(State state)
{
    switch (state)
    {
        case State::Idle:
            return "Idle";
        case State::IdleOff:
            return "IdleOff";
        case State::SetTemp:
            return "SetTemp";
        case State::Heating:
            return "Heating";
        case State::Error:
            return "Error";
    }
    return "UNKNOWN";
}