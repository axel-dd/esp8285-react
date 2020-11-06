#!/usr/bin/python
import os

def main():
    fh = open('jquery.min.gz', 'rb')
    fhw = open("../src/jquery.min.gz.cpp", "w+")

    fhw.write("#include <Arduino.h>\n\n\
const uint8_t JQueryMinGzFile[] PROGMEM = { ")

    ba = bytearray(fh.read())
    for byte in ba:
        fhw.write(hex(byte))
        fhw.write(",")

    fhw.seek(fhw.tell()-1,0)
    fhw.truncate()
    fhw.write("}; ")


    fh.close()
    fhw.close()

if __name__ == "__main__":
    main()