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

def readInput(channel):
    if 0 < channel < 5:
        state = explorerhat.input[channel - 1].read()
    else:
        state = False
        print "Specified input channel is out of range."
    print "Input value = ", state
    return int(state)

def readTouch(channel):
    if 0 < channel < 9:
        state = explorerhat.touch[channel - 1].read()
    else:
        state = False
        print "Specified touch channel is out of range."
    print "touch value = ", state
    return int(state)

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
