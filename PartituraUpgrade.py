import pathlib
import os
import subprocess
import re
import argparse
import sys
from datetime import datetime

# ============================================================
# Diagnostics
# ============================================================

DEBUG = True

def debug(msg):
    if DEBUG:
        print(f'[DEBUG] {msg}', flush=True)

def run(cmd, cwd=None):
    debug(f'Executing command: {" ".join(cmd)}')
    debug(f'Command cwd: {cwd}')

    try:
        result = subprocess.run(
            cmd,
            cwd=cwd,
            capture_output=True,
            text=True
        )
    except FileNotFoundError:
        debug(f'Executable not found: {cmd[0]}')
        raise

    debug(f'Return code: {result.returncode}')

    if result.stdout:
        debug(f'stdout:\n{result.stdout}')

    if result.stderr:
        debug(f'stderr:\n{result.stderr}')

    return result

# ============================================================
# VCS detection
# ============================================================

def DetectVCS(dirPath):
    """Detect VCS ONLY in the given directory"""
    path = pathlib.Path(dirPath)

    debug(f'DetectVCS: checking {path}')

    if (path / '.git').exists():
        debug('Detected Git (.git found)')
        return 'git'

    if (path / '.svn').exists():
        debug('Detected SVN (.svn found)')
        return 'svn'

    debug('No VCS detected')
    return None

# ============================================================
# Git
# ============================================================

def GetGitVersion(dirPath):
    debug(f'GetGitVersion: {dirPath}')

    result = run(['git', 'rev-list', '--count', 'HEAD'], cwd=dirPath)
    if result.returncode != 0:
        raise RuntimeError('git rev-list failed')

    revision = result.stdout.strip()
    debug(f'Git revision: {revision}')
    return revision

def GetGitInfo(dirPath):
    debug(f'GetGitInfo: {dirPath}')
    info = {}

    result = run(['git', 'rev-parse', '--short', 'HEAD'], cwd=dirPath)
    info['hash'] = result.stdout.strip()

    result = run(['git', 'log', '-1', '--format=%ci'], cwd=dirPath)
    info['date'] = result.stdout.strip()

    result = run(['git', 'diff-index', '--quiet', 'HEAD', '--'], cwd=dirPath)
    info['modified'] = '1' if result.returncode != 0 else '0'

    debug(f'Git info: {info}')
    return info

# ============================================================
# SVN
# ============================================================

def GetSvnVersion(dirPath):
    debug(f'GetSvnVersion: {dirPath}')

    result = run(
        ['svn', 'info', '--show-item', 'last-changed-revision', dirPath]
    )

    if result.returncode != 0:
        raise RuntimeError('svn info failed')

    revision = result.stdout.strip()
    debug(f'SVN revision: {revision}')
    return revision

def GetSvnInfo(dirPath):
    debug(f'GetSvnInfo: {dirPath}')
    info = {}

    result = run(
        ['svn', 'info', '--show-item', 'last-changed-date', dirPath]
    )
    info['date'] = result.stdout.strip()

    result = run(['svn', 'status', '-q', dirPath])
    info['modified'] = '1' if result.stdout.strip() else '0'

    info['hash'] = GetSvnVersion(dirPath)

    debug(f'SVN info: {info}')
    return info

# ============================================================
# Version resolver
# ============================================================

def GetVersionInfo(dirPath):
    vcs = DetectVCS(dirPath)

    if vcs == 'git':
        print(f'Detected Git repository at {dirPath}')
        revision = GetGitVersion(dirPath)
        info = GetGitInfo(dirPath)

    elif vcs == 'svn':
        print(f'Detected SVN repository at {dirPath}')
        revision = GetSvnVersion(dirPath)
        info = GetSvnInfo(dirPath)

    else:
        print(f'WARNING: No VCS detected at {dirPath}, using defaults')
        revision = '0'
        info = {'hash': 'unknown', 'date': 'unknown', 'modified': '0'}

    return revision, info, vcs

# ============================================================
# Main
# ============================================================

def main():
    parser = argparse.ArgumentParser(
        description="Helper script to upgrade partitura (SVN + Git support)"
    )
    parser.add_argument(
        '-D', '--working-directory',
        help='Directory to search for *.acc.xtrsvn'
    )
    parser.add_argument(
        '--force-vcs',
        choices=['svn', 'git'],
        help='Force specific VCS'
    )
    parser.add_argument(
        '--debug',
        action='store_true',
        help='Enable debug output'
    )

    args = parser.parse_args()

    global DEBUG
    if args.debug:
        DEBUG = True

    workingDirectory = (
        args.working_directory
        if args.working_directory
        else os.path.dirname(os.path.realpath(__file__))
    )

    debug(f'Python executable: {sys.executable}')
    debug(f'Python version: {sys.version}')
    debug(f'Current CWD: {os.getcwd()}')
    debug(f'Script path: {__file__}')
    debug(f'Lookup starts at: {workingDirectory}')
    debug(f'Arguments: {sys.argv}')

    print(f'Lookup starts at: {workingDirectory}')

    debug('Searching for *.acc.xtrsvn files')
    xtrAttrFileList = list(
        pathlib.Path(workingDirectory).rglob('*.acc.xtrsvn')
    )
    debug(f'Found {len(xtrAttrFileList)} template file(s)')

    filesProcessed = 0
    vcsUsed = None

    for xtrAttrFilePath in xtrAttrFileList:
        print(f'\nProcessing: {xtrAttrFilePath}')

        try:
            targetDir = xtrAttrFilePath.parents[2]
            debug(f'Target directory: {targetDir}')

            if args.force_vcs:
                debug(f'Forcing VCS: {args.force_vcs}')
                if args.force_vcs == 'git':
                    revision = GetGitVersion(targetDir)
                    info = GetGitInfo(targetDir)
                    vcs = 'git'
                else:
                    revision = GetSvnVersion(targetDir)
                    info = GetSvnInfo(targetDir)
                    vcs = 'svn'
            else:
                revision, info, vcs = GetVersionInfo(targetDir)

            if vcsUsed is None:
                vcsUsed = vcs

            with open(xtrAttrFilePath, 'r', encoding='utf-8') as f:
                data = f.read()

            outputFilePath = re.sub(r'\.xtrsvn$', '', str(xtrAttrFilePath))
            print(f'Creating {outputFilePath}')

            data = data.replace('$WCREV$', revision + 10000)
            data = data.replace('$WCDATE$', info['date'])
            data = data.replace('$WCMODS$', info['modified'])
            data = data.replace('$WCRANGE$', info['hash'])
            data = data.replace(
                '$WCNOW$',
                datetime.now().strftime('%Y-%m-%d %H:%M:%S')
            )

            with open(outputFilePath, 'w', encoding='utf-8') as f:
                print(f'Writing {outputFilePath}')
                f.write(data)

            filesProcessed += 1

        except Exception:
            print('[ERROR] Exception while processing file')
            import traceback
            traceback.print_exc()
            return 2

    print('\n' + '=' * 60)
    debug(f'filesProcessed = {filesProcessed}')
    debug(f'vcsUsed = {vcsUsed}')

    if filesProcessed > 0:
        vcsName = vcsUsed.upper() if vcsUsed else 'UNKNOWN'
        print(f'Successfully processed {filesProcessed} file(s) using {vcsName}')
    else:
        print('No files were processed (this is NOT an error)')

    return 0

# ============================================================
# Entry point
# ============================================================

if __name__ == '__main__':
    try:
        sys.exit(main())
    except Exception:
        print('[FATAL] Unhandled exception')
        import traceback
        traceback.print_exc()
        sys.exit(3)
