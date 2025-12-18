import subprocess
import sys
import os
from pathlib import Path

def CountCode():
    SSbasedir = Path("Special-Spoon/src")
    SSdirs = ["Core", "ECS", "Editor", "System", "Utils"]
    SSfree = ["Spoon.h"]
    WSfree = ["buildme.py", "CodeCounter.py"]

    print("Preparing cloc file...")
    outputfile = "clocfile.txt"
    with open(outputfile, 'w') as clocfile:

        print("Counting Special-Spoon...")
        for directory in SSdirs:
            clocdir = SSbasedir / directory
            Cloc(clocdir, clocfile)

        print("Counting free files...")
        for free in SSfree:
            clocdir = SSbasedir / free
            Cloc(clocdir, clocfile)

        for free in WSfree:
            Cloc(free, clocfile)
    print(f"CLOC output written to {outputfile}")
    print("Success")
def Cloc(path, output):

    pathstr = str(path)
    output.write(f"--- Counting : {pathstr} \n")

    try:
        result = subprocess.run(["cloc", pathstr], capture_output=True, text=True)
        output.write(result.stdout + "\n")
    except subprocess.CalledProcessError as e:
        errormsg = f"ERROR: failed to cloc at path: {pathstr}\n"
        print(errormsg)
        output.write(errormsg)

if __name__ == "__main__":

    CountCode()