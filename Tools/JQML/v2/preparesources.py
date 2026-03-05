#!/usr/bin/env python3
# SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
"""
JQML v2 — Prepare QML sources for compilation.

Copies QML source directories into a build directory tree so the compiler
has a single, flat source root to work with.

Usage
-----
    preparesources.py <src1> <dst1> [<src2> <dst2> ...]

Each pair of arguments names a source directory (or file) and the
destination directory into which it should be copied.  Directories are
created as needed.  An existing destination directory is **not** removed
before copying; files are overwritten individually so that unchanged files
keep their modification time.

This script is called by both the CMake and QMake build systems before
the Node.js compiler runs.  See Config/CMake/WebCompiler.cmake and
Config/QMake/WebCompiler.pri for integration details.
"""

import sys
import os
import shutil


def copy_item(src, dst_dir):
    """Copy a single file or directory tree into dst_dir."""
    name     = os.path.basename(src)
    dst_path = os.path.join(dst_dir, name)

    if os.path.isfile(src):
        shutil.copy2(src, dst_path)
        return

    if os.path.isdir(src):
        # Merge: copy individual items rather than replacing the whole tree.
        os.makedirs(dst_path, exist_ok=True)
        for entry in os.listdir(src):
            entry_src = os.path.join(src, entry)
            if os.path.isfile(entry_src):
                shutil.copy2(entry_src, os.path.join(dst_path, entry))
            elif os.path.isdir(entry_src):
                sub_dst = os.path.join(dst_path, entry)
                if os.path.exists(sub_dst):
                    shutil.rmtree(sub_dst)
                shutil.copytree(entry_src, sub_dst)
        return

    print(f"Warning: source path does not exist, skipping: {src}", file=sys.stderr)


def prepare_sources(args):
    """
    Process alternating source/destination pairs from *args*.

    Parameters
    ----------
    args : list[str]
        Flat list ``[src1, dst1, src2, dst2, ...]``.
    """
    if len(args) % 2 != 0:
        print(
            "Error: expected an even number of arguments (source/destination pairs).\n"
            f"       Got {len(args)} argument(s).",
            file=sys.stderr,
        )
        sys.exit(1)

    for i in range(0, len(args), 2):
        src = args[i]
        dst = args[i + 1]

        if not os.path.exists(src):
            print(f"Warning: source path does not exist, skipping: {src}", file=sys.stderr)
            continue

        os.makedirs(dst, exist_ok=True)
        copy_item(src, dst)
        print(f"  {src}  →  {dst}")


if __name__ == "__main__":
    prepare_sources(sys.argv[1:])
