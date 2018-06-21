from envirophat import light, weather, motion, analog, leds

def readADC(channel):
    voltage = analog.read(channel)
    print "voltage value = ", voltage
    return float(voltage)

def writeLEDs(state):
    if (state):
        leds.on()
    else:
        leds.off()
    return 0
