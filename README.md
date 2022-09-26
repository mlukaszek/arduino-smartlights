
# Arduino smart lights controller

## DEPRECATION WARNING
Note: this is an old project of mine. While functional, I do not expect to update it anymore.
It has been replaced by another ESPHome based custom PCB that does the same and more (basically ticking all the boxes listed in the Future plans section of this project).

[![Build Status](https://travis-ci.org/mlukaszek/arduino-smartlights.svg?branch=master)](https://travis-ci.org/mlukaszek/arduino-smartlights)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## What is it

This is a simple project that has been developed to control lights in my house. It uses an Arduino which controls relays
that turn mains voltage to invididual circuits for light sources located in various locations in the building.

## User interface

At the moment, the only user interface that can be used to toggle the switches are momentary push buttons installed on walls.

The controller is able to perform different actions, based on how long the button was pressed.
See [Mapping](#mapping) for more details. 

Alternatively - it is also possible to use text commands that can be passed via serial, however this is a very limited
and low level interface that I have been primarily using while debugging.

There is nothing that would make adding other interfaces imposible and the project will be extended to add support
for commands sent over WiFi (e.g. from an mobile app on a smartphone, or via protocols like MQTT).
See [Future Plans](#future-plans) for more information.  

## Customization and configuration

The operation of the system can be customized by editing the `Rules.json` file which defines what connections
are used. This file is used as an input to a `RuleGenerator.py` script which generates `Rules.h`
header file that the firmware includes in the build process.

### Defining rules

There are two main sections of the JSON file with rules:
* `expanders` is a list all expanders that your setup uses and specifies how cables are connected. 
* `mapping` is a definition of relation between triggers (like holding a button down) and effects
  (like toggling the state of a relay to turn it on or off).

So, the structure is something like:
```
{
  "expanders" : [
    // all expanders that you use are listed here 
  ],

  "mapping": {
    // mapping between input events and actions
    // that change output states lives here
  }
}
```

### Expanders

An single expander is an object that has values defined for 3 keys - `address`, `inputs` and `outputs`.

Address is an integer and means the address of the MCP23017 chip instance (ranging between 0 and 7,
without the chip-specific part of the value, 0x20).

Inputs and output are lists of (up to) 8 strings which become name identifiers for connections - something
you can reference in section describing mapping.

The names are later mangled by a script that generates a C++ header from it, so it's best to stick to a
`latin.characters.separated.by.a.dot` convention.

An example single expander definition can look like this:

```
{
  "expanders": [
    {
      "address": 0,
      "inputs": [
        "ground.entrance",
        "ground.kitchen",
        "ground.livingroom.left",
        "ground.livingroom.right",
        "ground.bedroom",
        "upstairs.room.left",
        "upstairs.room.right",
        "upstairs.bathroom"
      ],
      "outputs": [
        "hallway.ceiling",
        "kitchen.table",
        "ground.ledstrip.side",
        "ground.bedroom",
        "kitchen.ceiling",
        "upstairs.room.left",
        "upstairs.room.right",
        "upstairs.bathroom"
      ]
    }
  ],
...
}
```

### Mapping

The program currently recognizes 3 types of events that can be triggered with push buttons - short,
medium and long button press events.

| Event | Duration |
|-------|----------|
|short press|released under 600 ms|
|medium press|released between 600 ms and 3 seconds|
|long press|pressed for longer than 3 seconds|

The actions that you can map to those events are as follows:

* toggle an output state (= turn ON if it is OFF, or turn it OFF if it's ON)
* turn an output ON and set a timer, which will turn it OFF automatically when it expires. The expiration timeout value is configurable.
* turn all outputs OFF.

Timers come in two flavors - resettable or cancellable. First type means that you can reset the counter to the original value by provoking the event again while the timer is running, effectively prolonging the time the output in ON. The latter is the opposite - if the timer event happens while the timer is running, the timer value is immediately shortened to 0, effectively turning the output OFF.
You can set any timer expiry time to multiples of 30 second period.

By default, timers are set to 30 seconds and are cancellable.

There can only be a single effect of a single type of an event per input. This means that, for example,
pressing and releasing a push button connected to first input pin of an expander can only do one of 3 actions
listed above - say, toggle an output pin.
However, you can define other action for another event associated with the same push button
(e.g., medium press can turn all outputs off).

Chaining actions is currently not possible, and that's by design. The original intent was to use this project
as a simple execution unit with as little logic as possible, letting more advanced logic live in another device,
which would give orders to this basic controller.

#### Example mapping

Assuming the pin connections as above (refer to section explaining expanders), an example mapping can look like this:

```
{
...
  "mapping": {
    "ground.kitchen": {
      "short":  "toggle:kitchen.ceiling",
      "medium": "toggle:kitchen.table"
    },

    "ground.livingroom.middle": {
        "short": "timer:ground.ledstrip.side,2,on",
    },
    
    "ground.bedroom": {
      "short": "toggle:ground.bedroom",
      "long": "off"
    },
    
    "ground.entrance": "hallway.ceiling"
  }
}
```

##### Defining the mapping object

* Keys are input names and should be strings.
* Values are objects, defining mapping between trigger types (`"short"`, `"medium"` or `"long"`) and actions.
* Actions are strings, which consist of output names prefixed with `toggle:` or `timer:`, indicating type of action.
* There is also one special value that you can use for an action: `"off"` which means turn all outputs off.
* You can use optional arguments for `timer:` entries. These should be comma separated, no whitespaces, and follow the output name. The first argument is expiration value (in multiplies of 30s period), and the second argument is either `on` or `off`, for cancellable and resettable timers respectively. In the example below the timer is set to 1 minute and is resettable. If you skip arguments, they default to `1` for expiry time (meaning 30 seconds) and `off`, meaning a cancellable timer.
* Since the most common usage is mapping a short press to a toggle action, there is a shorter way of defining such entries: the value can simply be a string with output name (with no prefix or arguments).

#### Regenerating mapping

Once you modify the `Rules.json` file, you should regenerate the header file Arduino code uses.
To do that, use a Python script like so:

```
RuleGenerator.py -c Rules.json -o Rules.h
```

For quick changes, it's also possible to edit the header file by hand - it uses human-readable macros.
However, if you do that make sure the rules for a single input are ordered from longest time of holding
the button to shortest - it's an optimisation that makes the interpreter code simpler.

## Hardware requirements

The program needs an Arduino to run. I have used Arduino Nano, but any other variant could be used - the only feature
that must be available is I2C which is used to interface with port expanders - which control relays, and scan inputs
to detect changes of button states.

In fact, this does not need to be a device using AVR chip. There is an early support for ESP8266 running Arduino
present, and my plan is to switch to a board using this uC eventually to get more functionality.

### Sample hardware

> Warning: handle mains voltage with caution. It can be lethal if you don't know what you're doing.

A working prototype that currently controls lights in my house uses 5 identical, custom designed PCBs
that have connectors exposing pins for:
- power (a few 5V and GND pins),
- I2C SDA and CLK lines,
- 8 digital inputs connected to push buttons,
- 8 outputs conected to relay coils.

The components I used:
- Arduino Nano, soldered to one of the five boards
- MCP23017 port expanders (1 for each board, each with different address selected)
- ULN2803 chips to be able to provide enough current for driving coils (1 for each board)
- a few resistors and capacitors for each board

More details of the hardware, including the schematics will come.
It is a fairly standard application of MCP23017 and ULN2803 though.

The boards are inside a case which can be mounted on a DIN rail. I chose relays with 5V coils, which have
compatible DIN mounting available as well, so they can be installed in the same place where the controller
and 4 supporting boards are.

All 5 boards are connected to the same 5V power supply (also in DIN-compatible case), and are connected to
shared I2C data and clock lines.

Each of the PCBs have connectors for 8 inputs lines from push buttons. Buttons share a ground connection,
and holding them down them pulls input pins of expanders low. Finally, coils of the relay share a 5V connection,
and relays are switched on by setting an output pin low, so the current can flow.

## Software

The environment I currently use for editing is Visual Studio 2017, with Visual Micro add-on.
It is also possible to use Platform I/O, and it's used for Travis continous integration. This is probably the easiest and quickest way to build everything.

Currently, the project files that are available in the repository may contain paths that are specific
to the folder structure I use on my PC, however I will try to clean this up as I go.

### Dependencies

There are two 3rd party libraries used:

* `Adafruit-MCP23017-Library` to simplify communication with MCP23017 chips
* `Adafruit-Sleepydog` to simplify using a Watchdog on AVR Arduinos. Not needed for ESP8266.

Both should be available in your Arduino environment for the project to build.

### Program initialization

In setup phase, the program checks which port expanders are available (i.e., respond to I2C communication).
Since I used MCP23017 chips, there can be maximum 8 expanders in the system.

All detected port expanders are configured to use `PORTA` for inputs, and `PORTB` for outputs.
It is currently not possible to split the 16 available GPIO pins unevenly, so there can be maximum of
64 inputs and 64 outputs configured. All input pins are pulled high by default.

### Running loop

There is a single run of the loop every 15 ms, which I call a *tick*.
Debouncing of input pins is done implicitly, by ignoring changes which last shorter than a single tick.

There are just a few objects that are used to realise the logic: `InputMonitor` is used to for polling
the state of all input pins, and it uses `RuleInterpreter` to determine if an action is required.
If so, `RuleInterpreter` returns a `Command` which can be executed by `OutputSetter`, which is a class that toggles
output pins.

Actions are performed as soon as it's clear what the trigger is - this means that if a button only has one rule
defined for short press, the action can be executed if a button is held for longer than debounce time.
Where there are more actions mapped to the same button, then it's possible that an action is executed on
*releasing* of the button.

For more details, refer to the code.

## Future plans

* [ ] Switch to ESP8266
* [ ] Utilize WiFi for OTA updates
* [ ] Add MQTT interface for both state reporting and remote control
* [ ] Time of day awareness, and dawn/dusk triggers
* [ ] Integrate with [Home Assistant](https://home-assistant.io/)
* [ ] Integrate with Google Home for voice control
