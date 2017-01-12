import os
import sys
import json
import argparse
import subprocess
import binascii
import tempfile
from collections import deque

"""
Rule definitions, a.k.a. Trigger->Command mapping

Odd bytes: Trigger definitions
7 6 5 4 3 2 1 0
t t a a a i i i

Even bytes: Command definitions
7 6 5 4 3 2 1 0
e e a a a o o o

iii = input pin
ooo = output pin
aaa = expander address

tt = trigger
00 - short press
01 - medium press (over 2s)
10 - long press (over 5s)

ee = effect
01 = timer reset, on for next 60 seconds
10 = toggle
11 = all off
"""
    
def generate_enums(expanders, out):
    address = 0
    inputs = []
    outputs = []
    
    for expander in expanders:
        start = " = %d << SHIFT_PORT" % address
        i = 0
        for input in expander["inputs"]:
            inputs.append("/* a%dp%d */ przycisk_%s%s,\n" % (address, i, input.replace(".", "_"), start if i == 0 else ""))
            i += 1
            
        i = 0
        for output in expander["outputs"]:
            outputs.append("/* a%dp%d */ punkt_%s%s,\n" % (address, i, output.replace(".", "_"), start if i == 0 else ""))
            i += 1
        
        address += 1

    out.write("constexpr char SHIFT_PORT = 3;\n")
    out.write("constexpr char SHIFT_TRIGGER = 6;\n")
    out.write("constexpr char SHIFT_ACTION = 6;\n\n")
        
    out.write("enum Inputs {\n")    
    for input in inputs:
        out.write(input)
    out.write("};\n\n")
    
    out.write("enum Outputs {\n")    
    for output in outputs:
        out.write(output)
    out.write("};\n\n")
    
    out.write("enum Triggers {\n")
    out.write("  ShortPress,\n")
    out.write("  MediumPress, // 2-5 seconds\n")
    out.write("  LongPress, // over 5 seconds\n")
    out.write("};\n\n")
    
    out.write("enum Actions {\n")
    out.write("  TimerReset = 1,  // keep on for next 60 seconds\n")
    out.write("  Toggle,\n")
    out.write("  AllOff,\n")
    out.write("};\n\n")
    
    out.write("#define WHEN_PRESSED_SHORT(X) (ShortPress << SHIFT_TRIGGER | przycisk_##X)\n")
    out.write("#define WHEN_PRESSED_MEDIUM(X) (MediumPress << SHIFT_TRIGGER | przycisk_##X)\n")
    out.write("#define WHEN_PRESSED_LONG(X) (LongPress << SHIFT_TRIGGER | przycisk_##X)\n")
    out.write("#define TOGGLE(X) (Toggle << SHIFT_ACTION | punkt_##X)\n")
    out.write("#define TIMER_RESET(X) (TimerReset << SHIFT_ACTION | punkt_##X)\n")
    out.write("#define ALL_OFF(x) (AllOff << SHIFT_ACTION)\n\n")

def _(value):
    return value.replace(".", "_").replace("toggle:", "").replace("timer:", "")

def generate_mapping(key, value, out):
    bytes = 0
    if type(value) is not dict:
        out.write(" WHEN_PRESSED_SHORT(%s)," % _(key))
        out.write(" TOGGLE(%s),\n" % _(value))
        bytes = 2
    else:
        # Multiple possible actions for the same input need to be ordered
        # from the longest to the shortest press, as this is how rule execution
        # logic is written for simplicity (to save program space in firmware)
        ordered = deque()
        for trigger, action in value.iteritems():
            line = ""
            order = 0
            bytes += 2
            assert trigger in ("short", "medium", "long"), "Unknown trigger '%s' for key %s." % (trigger, _(key))
            if "short" == trigger:
                trigger = "WHEN_PRESSED_SHORT"
                order = 2
            elif "medium" == trigger:
                trigger = "WHEN_PRESSED_MEDIUM"
                order = 1
            else:
                trigger = "WHEN_PRESSED_LONG"
            line += " %s(%s), " % (trigger, _(key))
                        
            output = _(action)
            if action.startswith("toggle:"):                
                action = "TOGGLE"
            elif action.startswith("timer:"):
                action = "TIMER_RESET"
            else:
                action = "ALL_OFF"
                output = "AllOff"
            line += "%s(%s),\n" % (action, output)
            
            toFront = order != 2
            if 1 == order and len(output) != 0 and "LONG" in output[0]:
                toFront = False
            
            if toFront:
                ordered.appendleft(line)
            else:
                ordered.append(line)

        for line in ordered:
            out.write(line)
    return bytes

def interpret(out, expanders):
    i = 0
    text = ""
    bytes = binascii.unhexlify(''.join(out.split()))

    print "Got the following output (%d rules, %d bytes):" % (len(bytes)/2, len(bytes))
    print out
    print
    print "This is how I interpret it:"

    for byte in bytes:
        if i % 2 == 0: # Trigger
            text = "%02d: When " % (i/2 + 1)
            trigger = (0b11000000 & ord(byte)) >> 6
            port = (0b00111000 & ord(byte)) >> 3
            pin = (0b111 & ord(byte))
            what = [expander["inputs"] for expander in expanders if expander["address"] == port][0][pin]
            
            text += "pin %d of port %d (%s) is " % (pin, port, what)
            if 0 == trigger:
                text += "pressed shortly"
            elif 1 == trigger:
                text += "held a bit"
            else:
                text += "held for a long time"
        else: # Action
            text += ", "
            action = (0b11000000 & ord(byte)) >> 6
            port = (0b00111000 & ord(byte)) >> 3
            pin = (0b111 & ord(byte))
            what = [expander["outputs"] for expander in expanders if expander["address"] == port][0][pin]            
            
            if 0 == action:
                text += "toggle "
            elif 1 == action:
                text += "reset timer for "
            else:
                text += "turn all off"

            if action <= 1:
                text += "pin %d of port %d (%s)" % (pin, port, what)
            print text
        i += 1
       
def compile(source, output):        
    print "Compiling %s to %s" % (source, output)
    return subprocess.call(["g++", source, "-o", output, "-I."])

def verify(rules, expanders):
    source = tempfile.NamedTemporaryFile(suffix = ".cpp", delete=False)
    source.write('#include <iomanip>\n#include <iostream>\n#include "%s"\n' % rules)
    source.write("""
int main() {
    unsigned i = 0;
    for (unsigned char *byte = const_cast<unsigned char*>(rules); i < rulesSize; ++byte) {
        std::cout << std::setw(2) << std::setfill('0') << std::hex << (0xFF & (int)(*byte)) << ((++i % 8 == 0) ? '\\n' : ' ');
    }
}
        """)
    source.close()

    binary = os.path.splitext(source.name)[0]
    if os.name == "nt":
        binary += ".exe"
    
    compile(source.name, binary)
    os.remove(source.name)

    out = subprocess.check_output(binary, shell=True)
    os.remove(binary)

    interpret(out, expanders)

    
def main(args = ()):
    if os.name == "nt":
        os.environ["PATH"] += os.pathsep + "c:/winbuilds/bin"

    parser = argparse.ArgumentParser()
    parser.add_argument("-c", "--config", help="JSON configuration file", required=True)
    parser.add_argument("-o", "--output", help="Output file with rule definitions", required=True)
    parser.add_argument("-v", "--verify", help="Self-check, build a sample to interpret rules", action="store_true")
    opts = parser.parse_args(args)
    
    config = None
    with open(opts.config, "r") as configFile:
        config = json.load(configFile)

    with open(opts.output, "w") as out:
        out.write("""
#ifndef _RULES_H_
#define _RULES_H_

#ifdef __AVR__
#include <avr/pgmspace.h>
#else
#define PROGMEM
#endif

""")

        generate_enums(config["expanders"], out)
        out.write("const PROGMEM unsigned char rules[] = {\n")
        i = 0
        for key, value in config["mapping"].iteritems():
            i += generate_mapping(key, value, out)

        out.write("\n};\n\n")
        out.write("const int rulesSize = %d;\n\n" % i)
        out.write("#endif // _RULES_H_")
    print "Generated header with rules: %s" % opts.output

    if opts.verify:
        print "Attempting to verify correctness of generated rules..."
        verify(opts.output, config["expanders"])

if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
