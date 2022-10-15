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
    for i in range(1, len(sys.argv) - 3, 2):
        copy(sys.argv[i], sys.argv[i + 1])

    GRAPHQL_REQUEST_PATH = ""
    BUILD_DIR = sys.argv[len(sys.argv) - 1]
    BUILD_DIST_DIR = BUILD_DIR + "/Resources"
    print("BUILD_DIR", BUILD_DIR)

    for dir in sys.argv:
        if dir.replace("\\", "/").endswith("ImtCore/Include/imtqml/Qml/imtqml"):
            GRAPHQL_REQUEST_PATH = dir + "/GraphQLRequest.js"

    if os.path.isfile(GRAPHQL_REQUEST_PATH):
        print("copy from ", GRAPHQL_REQUEST_PATH, " to ", BUILD_DIST_DIR)
        shutil.copy(GRAPHQL_REQUEST_PATH, BUILD_DIST_DIR)
    else:
        print("GraphQLRequest.js file not found!")

#    python_path = sys.argv[len(sys.argv) - 1]
#    print("python_path", python_path)
#    process = subprocess.Popen(f'npm run compile {BUILD_DIR}/src', shell=True, cwd=BUILD_DIR)
#    process.wait()
#    shutil.copy(os.path.join(f'{BUILD_DIR}/src', 'jqml.full.js'), os.path.join(f'{BUILD_DIR}/Resources', 'jqml.full.js'))
#    build_from = sys.argv[len(sys.argv) - 3]
#    build_to = sys.argv[len(sys.argv) - 2]
#    copy(build_from, build_to)
print("WEB COMPILER COMPLETED!")
