import time

import automationhat

def readADC(channel):
    if automationhat.is_automation_hat():
        automationhat.enable_auto_lights(True)
    if channel == 1:
        voltage = automationhat.analog.one.read()
    elif channel == 2:
        voltage = automationhat.analog.two.read()
    elif channel == 3:
        voltage = automationhat.analog.three.read()
    elif channel == 4:
        voltage = automationhat.analog.four.read()
    else:
        voltage = 0
        print "Specified analog channel is out of range."
    print "voltage value = ", voltage
    return float(voltage)

def readInput(channel):
    if automationhat.is_automation_hat():
        automationhat.enable_auto_lights(True)
    if channel == 1:
        state = automationhat.input.one.read()
    elif channel == 2:
        state = automationhat.input.two.read()
    elif channel == 3:
        state = automationhat.input.three.read()
    else:
        state = False
        print "Specified input channel is out of range."
    print "Input value = ", state
    return int(state)

def writeOutput(channel, state):
    if automationhat.is_automation_hat():
        automationhat.enable_auto_lights(True)
    if channel == 1:
        automationhat.output.one.write(state)
    elif channel == 2:
        automationhat.output.two.write(state)
    elif channel == 3:
        automationhat.output.three.write(state)
    else:
        print "Specified output channel is out of range."
    return 0

# Automation pHat only has one relay
def writeRelay(channel, state):
    if automationhat.is_automation_hat():
        automationhat.enable_auto_lights(True)
    if channel == 1:
        automationhat.relay.one.write(state)
    elif channel == 2:
        if automationhat.is_automation_hat():
            automationhat.relay.two.write(state)
    elif channel == 3:
        if automationhat.is_automation_hat():
            automationhat.relay.three.write(state)
    else:
        print "Specified relay channel is out of range."
    return 0

# The below functions are only available on Automation Hat (not pHat)
def writePowerLight(state):
    if automationhat.is_automation_hat():
        automationhat.enable_auto_lights(False)
        automationhat.light.power.write(state)
    return 0

def writeCommsLight(state):
    if automationhat.is_automation_hat():
        automationhat.enable_auto_lights(False)
        automationhat.light.comms.write(state)
    return 0

def writeWarnLight(state):
    if automationhat.is_automation_hat():
        automationhat.enable_auto_lights(False)
        automationhat.light.warn.write(state)
    return 0

def writeAutoLightMode(state):
    if automationhat.is_automation_hat():
        automationhat.enable_auto_lights(state)
    return 0

def writeAnalogAutoLightMode(channel, state):
    if automationhat.is_automation_hat():
        if channel == 1:
            automationhat.analog.one.auto_light(state)
        elif channel == 2:
            automationhat.analog.two.auto_light(state)
        elif channel == 3:
            automationhat.analog.three.auto_light(state)
        else:
            print "Specified analog light channel is out of range."
    return 0

def writeAnalogLight(channel, state):
    if automationhat.is_automation_hat():
        automationhat.enable_auto_lights(False)
        if channel == 1:
            automationhat.analog.one.light.write(state)
        elif channel == 2:
            automationhat.analog.two.light.write(state)
        elif channel == 3:
            automationhat.analog.three.light.write(state)
        else:
            print "Specified analog light channel is out of range."
    return 0

def writeInputLight(channel, state):
    if automationhat.is_automation_hat():
        automationhat.enable_auto_lights(False)
        if channel == 1:
            automationhat.input.one.light.write(state)
        elif channel == 2:
            automationhat.input.two.light.write(state)
        elif channel == 3:
            automationhat.input.three.light.write(state)
        else:
            print "Specified input light channel is out of range."
    return 0

def writeOutputLight(channel, state):
    if automationhat.is_automation_hat():
        automationhat.enable_auto_lights(False)
        if channel == 1:
            automationhat.output.one.light.write(state)
        elif channel == 2:
            automationhat.output.two.light.write(state)
        elif channel == 3:
            automationhat.output.three.light.write(state)
        else:
            print "Specified output light channel is out of range."
    return 0

def writeRelayLight(channel, state):
    if automationhat.is_automation_hat():
        automationhat.enable_auto_lights(False)
        if channel == 1:
            automationhat.relay.one.light.write(state)
        elif channel == 2:
            if automationhat.is_automation_hat():
                automationhat.relay.two.light.write(state)
        elif channel == 3:
            if automationhat.is_automation_hat():
                automationhat.relay.three.light.write(state)
        else:
            print "Specified relay light channel is out of range."
    return 0

def test(dummy):
    automationhat.enable_auto_lights(False)

    automationhat.light.on()

    for analog in automationhat.analog:
        analog.light.on()

    for output in automationhat.output:
        output.light.on()

    for input in automationhat.input:
        input.light.on()

    time.sleep(1)

    automationhat.light.off()

    for analog in automationhat.analog:
        analog.light.off()

    for output in automationhat.output:
        output.light.off()

    for input in automationhat.input:
        input.light.off()

    automationhat.enable_auto_lights(True)

    if automationhat.is_automation_hat():
        automationhat.light.power.write(1)

    for i in range(4):
        if automationhat.is_automation_hat():
            automationhat.light.comms.toggle()
            automationhat.light.warn.toggle()

        automationhat.relay.one.toggle()

        if automationhat.is_automation_hat():
            automationhat.relay.two.toggle()
            automationhat.relay.three.toggle()
        automationhat.output.toggle()

        print(automationhat.analog.read())

        time.sleep(0.5)

    for i in range(4):
        print(automationhat.input.read())
        time.sleep(0.5)

    if automationhat.is_automation_hat():
        automationhat.light.power.write(0)

    return 0
