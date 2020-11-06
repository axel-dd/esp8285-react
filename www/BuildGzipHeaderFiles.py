#!/usr/bin/python
from glob import glob
import os
import gzip
import shutil
from pathlib import Path

def createGzipHeaderFile(pathName):
    # gzip file
    gzipPathName = pathName + '.gz'
    headerPathName = str(Path(pathName).parent.parent / 'src' / (Path(gzipPathName).name + '.h'))
    with open(pathName, 'rb') as f_in:
        with gzip.open(gzipPathName, 'wb', compresslevel=9) as f_out:
            shutil.copyfileobj(f_in, f_out)

    # build a C array from gzip compressed file
    fhr = open(gzipPathName, 'rb')
    fhw = open(headerPathName, 'w')

    ba = bytearray(fhr.read())
    fileVarName =  Path(gzipPathName).name.replace('.','_').upper()
    fhw.write(f'#define {fileVarName}_SIZE {len(ba)}\n')
    fhw.write(f'const uint8_t {fileVarName}[] PROGMEM = {{\n')

    byteColumn = 1
    for byte in ba:
        fhw.write(f"0x{byte:02x},")
        if byteColumn > 10:
            fhw.write('\n')
            byteColumn = 1
        else:
            byteColumn += 1

    fhw.seek(fhw.tell()-1, 0)
    fhw.write('};')

    fhr.close()
    fhw.close()
    os.remove(gzipPathName)

def main():
    fileTypes = glob('*.html') + glob('*.js')
    for file in fileTypes:
        createGzipHeaderFile(str(Path(file).resolve()))

if __name__ == "__main__":
    main()