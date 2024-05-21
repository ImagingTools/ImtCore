import argparse
import os
import pathlib
import re
import subprocess
import sys


def GetSvnVersion(dirPath):
    svnProcess = subprocess.Popen(
        ['svn', 'info', '--show-item', 'last-changed-revision', dirPath],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE)
    svnProcessOutput = svnProcess.communicate()
    svnRevision = svnProcessOutput[0].decode()
    svnRevision = svnRevision.replace('\n', '')
    svnRevision = svnRevision.replace('\t', '')
    svnRevision = svnRevision.replace('\r', '')

    return svnRevision


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Helper script to upgrade partitura")
    parser.add_argument('-D', '--working-directory', required=False,
                        help='The directory in which you want to find version template files')
    parser.add_argument('-F', '--filename', required=False,
                        help='Output file name')

    args = parser.parse_args()

    if (not args.working_directory is None) and (len(args.working_directory) > 0):
        workingDirectory = args.working_directory
    else:
        workingDirectory = os.path.dirname(os.path.realpath(__file__))

    if (not args.filename is None) and (len(args.filename) > 0):
        filename = args.filename
    else:
        filename = 'output.txt'

    svnRevision = GetSvnVersion(workingDirectory)

    with open(filename, 'w') as f:
        print(svnRevision, file=f)

    # sys.stdout.write(svnRevision)
