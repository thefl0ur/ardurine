## Ardu[ino][tange]rine 🍊

Artificial sun (with quirks) or an overengineered way to turn a lamp on and off at a given time.


## ⚠️ IMPORTANT ⚠️

This project uses 220 volts. Be smart and follow safety rules. It's not production-grade and could be dangerous for you or those around you. Use it at your own risk.

It's not even guaranteed to work after all – currently, there is no hardware available to validate it.

### How it works
Controlled via relay attached to Arduino, lamp goes on and off at speciefic time to simulate sunrise and sunset.

Every midnight, the `setupTodayAlarms` event fires, fills the `sunrise` and `sunset` variables with corresponding values, and attaches single-use alarms to turn the lamp on or off.

### How to generate sunrise_data.h

Sunrise data is powered by [SunriseSunset.io](https://sunrisesunset.io/)

1. In `fetch_data.py` modify `req_params` for adjust location
2. Call `python3 fetch_data.py` (`requests` package is needed)

### Software

* [RTClib](https://github.com/adafruit/RTClib)
* [TimeAlarms](https://github.com/PaulStoffregen/TimeAlarms)
* [Bounce2](https://github.com/thomasfredericks/Bounce2)

### Hardware

For wiring see source code.

* Ardunio (Uno)
* DS1307-based RTC
* HC-05 for blueooth
* Push button
* Relay module


### Communication

#### LED

There is a single LED that is normally off and lights up only if it fails to find the real-time clock.

#### Button

The button allows manually turning the lamp on and off but does not affect the alarms.

#### Serial

Both hardware and software serial operate at 9600 baud.

List of commands:
* `off` – turns the lamp off
* `on` – turns the lamp on
* `status` – prints the current status of the lamp, mostly for debugging
* `time` – prints the current time in `YYYY-MM-DDTHH:MM:SS` format
* `alarms` – prints two lines with the scheduled on and off alarm times


