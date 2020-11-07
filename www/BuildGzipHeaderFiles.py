#!/usr/bin/python
from glob import glob
import os
import gzip
import shutil
from pathlib import Path

Headers_h = Path('../src/WebServerFiles/Headers.h').resolve()
FileRoutings_inl = Path('../src/WebServerFiles/FileRoutings.inl').resolve()

def createGzipHeaderFiles(fileName, contentType):
    # gzip file
    gzipFileName = fileName + '.gz'
    with open(fileName, 'rb') as f_in:
        with gzip.open(gzipFileName, 'wb', compresslevel=9) as f_out:
            shutil.copyfileobj(f_in, f_out)

    # build a C byte array from gzip compressed file
    headerFileName = fileName + '.h'
    headerPathAbs = Headers_h.parent / headerFileName
    headerPathAbs.parent.mkdir(parents=True, exist_ok=True)
    with open(gzipFileName, 'rb') as f_in:
        with open(str(headerPathAbs), 'w') as f_out:
            ba = bytearray(f_in.read())

            fileVarName =  Path(fileName).as_posix().replace('/','__').replace('.','_').upper()
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

    os.remove(gzipFileName)

    # add include to new created header file
    with open(Headers_h, 'a') as f:
        f.write(f'#include "{str(Path(headerFileName).as_posix())}"\n')

    # add file routing macro
    with open(FileRoutings_inl, 'a') as f:
        f.write(f'FILE_ROUTING({fileVarName})\n')


def main():
    # make files empty
    with open(Headers_h, 'w') as f:
        pass
    with open(FileRoutings_inl, 'w') as f:
        pass

    # html files
    for fileName in glob('**/*.html', recursive=True):
        createGzipHeaderFiles(fileName, 'text/html')

    # javascript files
    for fileName in glob('**/*.js', recursive=True):
        createGzipHeaderFiles(fileName, 'application/javascript')


if __name__ == "__main__":
    main()