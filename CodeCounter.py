import subprocess
import sys
import os
from pathlib import Path

def CountCode():
    SSbasedir = Path("Special-Spoon/src")
    SSdirs = ["Core", "ECS", "Editor", "System", "Utils"]
    SSfree = ["Spoon.h"]
    WSfree = ["buildme.py", "CodeCounter.py"]
    excludeDirs = ["--exclude-dir=SystemFont"]

    total_path = []
    for directory in SSdirs:
        total_path.append(str(SSbasedir / directory))
    for path in SSfree:
        total_path.append(str(SSbasedir / path))
    for script in WSfree:
        total_path.append(str(script))

    print("Preparing cloc file...")
    outputfile = "clocfile.txt"
    with open(outputfile, 'w') as clocfile:

        print("Counting Special-Spoon...")
        for directory in SSdirs:
            clocdir = SSbasedir / directory
            Cloc(clocdir, clocfile, excludeDirs)

        print("Counting free files...")
        for free in SSfree:
            clocdir = SSbasedir / free
            Cloc(clocdir, clocfile, excludeDirs)

        for free in WSfree:
            Cloc(free, clocfile, excludeDirs)

        print("Printing summary...")
        Summary(total_path, clocfile, excludeDirs)

    print(f"CLOC output written to {outputfile}")
    print("Success")

def Cloc(path, output, excludes):

    pathstr = str(path)
    excludestr = str(excludes)
    output.write(f"--- Counting : {pathstr} \n")
    command = ["cloc"] + excludes + [pathstr]
    try:
        result = subprocess.run(command, capture_output=True, text=True)
        output.write(result.stdout + "\n")
    except subprocess.CalledProcessError as e:
        errormsg = f"ERROR: failed to cloc at path: {pathstr}\n"
        print(errormsg)
        output.write(errormsg)

def Summary(allpaths, output, excludes):

    excludestr = str(excludes)
    output.write(f"--- Summing --- \n")
    command = ["cloc"] + excludes + allpaths
    try:
        result = subprocess.run(command, capture_output=True, text=True)
        output.write(result.stdout + "\n")
    except subprocess.CalledProcessError as e:
        errormsg = f"ERROR: failed to cloc at path: {pathstr}\n"
        print(errormsg)
        output.write(errormsg)

if __name__ == "__main__":

    CountCode()