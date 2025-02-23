#include <RTClib.h>
#include <TimeAlarms.h>
#include <SoftwareSerial.h>
#include <Bounce2.h>

#include "sunrise_data.h"

#define SUNRISE 0
#define SUNSET 1

#define PIN_SOFTWARE_RX 10
#define PIN_SOFTWARE_TX 3
#define PIN_RELAY 4
#define PIN_BUTTON 2
#define PIN_LED 13

SoftwareSerial softwareSerial(PIN_SOFTWARE_RX, PIN_SOFTWARE_TX);
RTC_DS1307 clock;
DateTime sunrise, sunset;
Bounce button;

bool status;
int previousButtonState;

void setup() {
    pinMode(PIN_RELAY, OUTPUT);
    pinMode(PIN_LED, OUTPUT);
    pinMode(PIN_BUTTON, INPUT_PULLUP);

    softwareSerial.begin(9600);
    Serial.begin(9600);

    button = Bounce();
    button.attach(PIN_BUTTON);
    button.interval(10);

    if (!clock.begin()) {
        Serial.println("Couldn't find RTC");
        digitalWrite(PIN_LED, HIGH);
        while (1);
    }
    
    if (!clock.isrunning()) {
        clock.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    Alarm.alarmRepeat(0, 0, 1, setupTodayAlarms);

    setupTodayAlarms();

    uint32_t now = clock.now().unixtime();
    if (now >= sunrise.unixtime() and now <= sunset.unixtime()) {
        enableRelay();
    }

    previousButtonState = button.read();
}

void loop() {
    if (softwareSerial.available()) {
        processSerialCommand(softwareSerial.readString(), &softwareSerial);
    }
    else if (Serial.available()) {
       processSerialCommand(Serial.readString(), &Serial);
    }

    processButton();

    Alarm.delay(0);
}

void enableRelay() {
    digitalWrite(PIN_RELAY, HIGH);
    status = true;
}

void disableRelay() {
    digitalWrite(PIN_RELAY, LOW);
    status = false;
}

void setupTodayAlarms() {
    int dayNumber = calculateDayOfYear(clock.now());

    sunrise = DateTime( pgm_read_word(alarms[dayNumber][SUNRISE]));
    Alarm.alarmOnce(sunrise.hour(), sunrise.minute(), sunrise.second(), enableRelay);

    sunset = DateTime(pgm_read_word(&alarms[dayNumber][SUNSET]));
    Alarm.alarmOnce(sunset.hour(), sunset.minute(), sunset.second(), disableRelay);
}

int calculateDayOfYear(DateTime dt) {
    char daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Check for a leap year
    if (dt.year() % 4 == 0) {
        if (dt.year() % 100 != 0) {
            daysInMonth[1] = 29;
        } else {
            if (dt.year() % 400 == 0) {
                daysInMonth[1] = 29;
            }
        }
    }

    int day_of_year = dt.day();
    for (int i = 0; i < dt.month() - 1; i++) {
        day_of_year += daysInMonth[i];
    }

    return day_of_year;
}

void processSerialCommand(String command, Stream* response_stream) {
    if (command.equals("off")) {
        response_stream->println("Payload disabled");
        disableRelay();
    }
    if (command.equals("on")) {
        response_stream->println("Payload enabled");
        enableRelay();
    }
    if (command.equals("status")) {
        response_stream->println("Relay status: " + status? "On" : "Off");
    }
    if (command.equals("time")) {
        response_stream->println(clock.now().timestamp());
    }
    if (command.equals("alarms")) {
        char buffer[12];
        sprintf(buffer, "On: %02d:%02d", sunrise.hour(), sunrise.minute());
        response_stream->println(buffer);
        sprintf(buffer, "Off: %02d:%02d",sunset.hour(), sunset.minute());
        response_stream->println(buffer);
    }
}

void processButton() {
    button.update();
    int currentButtonState = button.read();

    if (currentButtonState != previousButtonState && currentButtonState == HIGH) {
        status ? disableRelay() : enableRelay();
    }
    previousButtonState = currentButtonState;
}
