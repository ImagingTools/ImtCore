import numbers
import os
import sys
import shutil
import subprocess
import datetime

print("WEB COMPILER RUN!")


def copy(folder_from, folder_to):
    print("copy from ", folder_from, " to ", folder_to)
    for f in os.listdir(folder_from):
        if os.path.isdir(os.path.join(folder_from, f)):
            shutil.copytree(os.path.join(folder_from, f), os.path.join(folder_to, f), ignore=shutil.ignore_patterns('qmldir'), dirs_exist_ok=True)
        if os.path.isfile(os.path.join(folder_from, f)):
            if f != 'qmldir':
                if not os.path.isdir(os.path.join(folder_to)):
                    os.mkdir(os.path.join(folder_to))
                shutil.copy(os.path.join(folder_from, f), os.path.join(folder_to, f))

if len(sys.argv) > 3:
    print("start copy")
    for i in range(1, len(sys.argv) - 3, 2):
        copy(sys.argv[i], sys.argv[i + 1])


print("WEB COMPILER COMPLETED!")
