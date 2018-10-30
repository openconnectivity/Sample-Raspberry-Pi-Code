import time

import explorerhat

def readAnalog(channel):
    if 0 < channel < 5:
        voltage = explorerhat.analog[channel - 1].read()
    else:
        voltage = 0
        print "Specified analog channel is out of range."
    print "voltage value = ", voltage
    return float(voltage)

def handleAnalogInput(input, voltage):
    channel = input.name()
    print("Analog input handler channel {} changed to {} volts.".format(channel, voltage))

def startObserveAnalogInput(channel):
    explorerhat.analog[channel - 1].changed(handleAnalogInput, 0.1)

def stopObserveAnalogInput(channel):
    explorerhat.analog[channel - 1].clear_events()

def readInput(channel):
    if 0 < channel < 5:
        state = explorerhat.input[channel - 1].read()
    else:
        state = False
        print "Specified input channel is out of range."
    print "input value = ", state
    return int(state)

def handleInput(input):
    state = input.read()
    channel = input.name()
    print("Input handler channel {} changed to {}.".format(channel, state))

def startObserveInput(channel):
    explorerhat.input[channel - 1].changed(handleInput, 25)


def stopObserveInput(channel):
    explorerhat.input[channel - 1].clear_events()

def readTouch(channel):
    if 0 < channel < 9:
        state = explorerhat.touch[channel - 1].is_pressed()
    else:
        state = False
        print "Specified touch channel is out of range."
    print "touch value = ", state
    return int(state)

def handleTouch(channel, event):
    if event == 'press':
        print "press"
    if event == 'release':
        print "release"
    print("Touch handler channel {} changed to {}.".format(channel, event))

def startObserveTouch(channel):
    explorerhat.touch[channel - 1].pressed(handleTouch)
    explorerhat.touch[channel - 1].released(handleTouch)

def stopObserveTouch(channel):
    explorerhat.touch[channel - 1].clear_events()

def writeOutput(channel, state):
    if 0 < channel < 5:
        explorerhat.output[channel - 1].write(state)
    else:
        print "Specified output channel is out of range."
    return 0

def writeLight(channel, state):
    if 0 < channel < 5:
        explorerhat.light[channel - 1].write(state)
    else:
        print "Specified light channel is out of range."
    return 0

def writeMotor(channel, motorSpeed):
    if 0 < channel < 3:
        if -100 < motorSpeed < 100:
            explorerhat.motor[channel - 1].speed(motorSpeed)
        else:
            print "Speicified motor speed is out of range (-100 < x < 100)."
    else:
        print "Specified motor channel is out of range."
    return 0

def waitForEvent(dummy):
    print "waiting..."
    explorerhat.pause()
