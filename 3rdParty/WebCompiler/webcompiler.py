import numbers
import os
import sys
import shutil
import subprocess
import datetime

print("Script RUN!")


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
    for i in range(1, len(sys.argv) - 3, 2):
        copy(sys.argv[i], sys.argv[i + 1])

    GraphQLRequestPath = ""
    BIN_WEB_DIST_DIR = ""
    BIN_WEB_DIR = ""
    for dir in sys.argv:
        if dir.replace("\\", "/").endswith("ImtCore/Include/imtqml/Qml/imtqml"):
            GraphQLRequestPath = dir + "/GraphQLRequest.js"
        if dir.replace("\\", "/").endswith("ImtCore/Bin/web/dist"):
            BIN_WEB_DIST_DIR = dir
        if dir.replace("\\", "/").endswith("ImtCore/Bin/web"):
            BIN_WEB_DIR = dir
    if os.path.isfile(GraphQLRequestPath):
        if not os.path.isdir(BIN_WEB_DIST_DIR):
            print("Bin/web/dist directory not found!")
        else:
            print("copy from ", GraphQLRequestPath, " to ", BIN_WEB_DIST_DIR)
            shutil.copy(GraphQLRequestPath, BIN_WEB_DIST_DIR)
    else:
        print("GraphQLRequest.js file not found!")
    if not os.path.isdir(BIN_WEB_DIR):
        print("Bin/web directory not found!")
    else:
    # dir_build = sys.argv[2]
        python_path = sys.argv[len(sys.argv) - 1]
        process = subprocess.Popen(f'{python_path}  qmlcore/build.py', shell=True, cwd=BIN_WEB_DIR)
        process.wait()
        build_from = sys.argv[len(sys.argv) - 3]
        build_to = sys.argv[len(sys.argv) - 2]
        copy(build_from, build_to)
print("End script!")
