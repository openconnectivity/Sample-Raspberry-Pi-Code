from envirophat import light, weather, motion, analog, leds

def readTemperature():
    temperature = weather.temperature()
    print "temperature value (C) = ", temperature
    return float(temperature)

def readPressure():
    pressure = weather.pressure()
    print "air pressure value (Pascals) = ", pressure
    return float(pressure)

def readAltitude(qnh):
    altitude = weather.altitude(qnh)
    print "altitude value (m) = ", pressure
    return float(pressure)

def readADC(channel):
    voltage = analog.read(channel)
    print "voltage value = ", voltage
    return float(voltage)

def readMagnetometer():
    magneticField = motion.magnetometer()
    print "magnetic field value = ", magneticField
    return magneticField

def readAccelerometer():
    acceleration = motion.accelerometer()
    print "acceleration value = ", acceleration
    return acceleration

def readHeading():
    heading = motion.heading()
    print "heading value = ", heading
    return heading

def readRawHeading():
    rawHeading = motion.raw_heading()
    print "raw heading value = ", rawHeading
    return rawHeading

def readBrightness():
    brightness = light.light()
    print "brightness value = ", brightness
    return long(brightness)

def readRGB():
    rgb = light.rgb()
    print "RGB color value = ", rgb
    return rgb

def readRawRGB():
    rgb = light.raw()
    print "Raw RGB color value = ", rgb
    return rgb

def writeLEDs(state):
    if (state):
        leds.on()
    else:
        leds.off()
    return 0
