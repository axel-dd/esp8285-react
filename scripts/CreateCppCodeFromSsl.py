#!/usr/bin/python
from glob import glob
import os
import sys
import gzip
import shutil
from pathlib import Path
import re

# returns a C++ compatible define name based on input fileName
def getFileVarName(fileName):
    fileVarName = re.compile('[^a-zA-Z0-9]').sub('_', fileName).upper()
    return fileVarName


# builds a C++ header file which includes the file content and other defines
def buildCppHeaderFile(fileName):
    # build a C byte array from gzip compressed file
    headerFilePath = CppSslDir / (fileName + '.h')
    headerFilePath.parent.mkdir(parents=True, exist_ok=True)
    with open(fileName, 'rb') as f_in:
        with open(headerFilePath, 'w') as f_out:
            ba = bytearray(f_in.read())

            fileVarName = getFileVarName(fileName)
            f_out.write(f'const char {fileVarName}_DATA[] PROGMEM = {{\n')

            bytesInLineWritten = 0
            for byte in ba:
                f_out.write(f"0x{byte:02x},")
                bytesInLineWritten += 1
                if bytesInLineWritten >= 10:
                    f_out.write('\n')
                    bytesInLineWritten = 0

            f_out.seek(f_out.tell()-1, 0)
            f_out.write('};')

    return str(headerFilePath)

# creates C++ source code from input file
def createCppCode(fileName):
    headerFileName = buildCppHeaderFile(fileName)

    # add include to new created header file
    includePath = Path(headerFileName).relative_to(CppSslDir).as_posix()
    with open(CppSslDir / 'FileIncludes.h', 'a') as f:
        f.write(f'#include "{includePath}"\n')


# we want to stay compatible with esp8285 1MB flash layout used by Tasmota firmware
# so we can't use a filesystem partition
# we need to embed the javascript app into the firmware flash
# this script create C++ source code from each file in repo-root/www/dist
def main():
    global SslDir, CppSslDir

    SslDir = (Path(sys.argv[0]).parent.parent / 'ssl').resolve()
    CppSslDir = (Path(sys.argv[0]).parent.parent / 'src/ssl').resolve()

    # clear existing files
    if CppSslDir.exists():
        shutil.rmtree(CppSslDir)

    # change to the SSL directory
    os.chdir(SslDir)

    # create cpp code from key and cert files in repo-root/ssl
    # to repo-root/src/ssl
    for fileName in glob('*.crt'):
        createCppCode(fileName)

    for fileName in glob('*.key'):
        createCppCode(fileName)


if __name__ == "__main__":
    main()