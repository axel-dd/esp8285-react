#!/usr/bin/python
from glob import glob
import os
import gzip
import shutil
from pathlib import Path
import re

# gzip fileName and returns the path to the compressed file
def compressFile(fileName):
    fileNameCompressed = fileName + '.gz'
    with open(fileName, 'rb') as f_in:
        with open(fileNameCompressed, 'wb') as f_out:
            f_out.write(gzip.compress(f_in.read(), compresslevel=9, mtime=0))

    return fileNameCompressed


# returns a C++ compatible define name based on input fileName
def getFileVarName(fileName):
    fileVarName = re.compile('[^a-zA-Z0-9]').sub('_', fileName).upper()
    return fileVarName


# file extension -> http content type
def getContentType(fileName):
    contentTypes = {
        '.html': 'text/html',
        '.js':   'application/javascript',
        '.css':  'text/css',
        '.svg':  'image/svg+xml',
        '.png':  'image/png',
        '.ico':  'image/x-icon'
    }
    return contentTypes[(Path(fileName).suffix.lower())]

# builds a C++ header file which includes the file content and other defines
# needed by the AsyncWebserver file routing
def buildCppHeaderFile(fileName, fileNameCompressed):
    # build a C byte array from gzip compressed file
    headerFilePath = WwwCppDir / (fileName + '.h')
    headerFilePath.parent.mkdir(parents=True, exist_ok=True)
    with open(fileNameCompressed, 'rb') as f_in:
        with open(headerFilePath, 'w') as f_out:
            ba = bytearray(f_in.read())

            fileVarName = getFileVarName(fileName)
            contentType = getContentType(fileName)
            uri = '/'
            if fileName.lower() != 'index.html':
                uri += Path(fileName).as_posix()

            f_out.write(f'#define {fileVarName}_URI "{uri}"\n')
            f_out.write(f'#define {fileVarName}_TYPE "{contentType}"\n')
            f_out.write(f'#define {fileVarName}_SIZE {len(ba)}\n')
            f_out.write(f'const uint8_t {fileVarName}_DATA[] PROGMEM = {{\n')

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
    fileNameCompressed = compressFile(fileName)
    headerFileName = buildCppHeaderFile(fileName, fileNameCompressed)
    os.remove(fileNameCompressed)

    # add include to new created header file
    includePath = Path(headerFileName).relative_to(WwwCppDir).as_posix()
    with open(WwwCppDir / 'FileIncludes.h', 'a') as f:
        f.write(f'#include "{includePath}"\n')

    # add file routing macro
    fileVarName = getFileVarName(fileName)
    with open(WwwCppDir / 'FileRoutings.inl', 'a') as f:
        f.write(f'FILE_ROUTING({fileVarName})\n')


# we want to stay compatible with esp8285 1MB flash layout used by Tasmota firmware
# so we can't use a filesystem partition
# we need to embed the reactjs app into the firmware flash
# this script create C++ source code from each file in repo-root/www/build
def main():
    global ReactAppDir, WwwCppDir

    ReactAppDir = Path('../www/build').resolve()
    WwwCppDir = Path('../src/www').resolve()

    # clear existing files
    if WwwCppDir.exists():
        shutil.rmtree(WwwCppDir)

    # change to react app directory
    os.chdir(ReactAppDir)

    # create cpp code from all files in repo-root/www
    # to repo-root/src/www

    # html files
    for fileName in glob('**/*.html', recursive=True):
        createCppCode(fileName)

    # javascript files
    for fileName in glob('**/*.js', recursive=True):
        createCppCode(fileName)

    # css files
    for fileName in glob('**/*.css', recursive=True):
        createCppCode(fileName)

    # svg files
    for fileName in glob('**/*.svg', recursive=True):
        createCppCode(fileName)

    # png files
    for fileName in glob('**/*.png', recursive=True):
        createCppCode(fileName)

    # ico files
    for fileName in glob('**/*.ico', recursive=True):
        createCppCode(fileName)


if __name__ == "__main__":
    main()