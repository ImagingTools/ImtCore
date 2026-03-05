# JQ

This repository contains the **JQ** library used by ImtCore.

## Origin

This library was extracted from the ImtCore repository (`3rdParty/JQ`) 
to enable independent version control and easier dependency management.

**Note**: This repository uses **Git LFS** (Large File Storage) for binary files (*.dll, *.lib, *.so, *.exe, etc.). Make sure you have Git LFS installed before cloning.

## Usage

This repository is integrated into ImtCore as a git submodule:

```bash
git submodule add https://github.com/ImagingTools/JQ.git 3rdParty/JQ
```

## Cloning

To clone this repository with LFS files:

```bash
git lfs install  # If not already installed
git clone https://github.com/ImagingTools/JQ.git
```

## Updating

To update this library in ImtCore:

1. Make changes in this repository
2. Commit and push changes
3. In ImtCore repository:
   ```bash
   cd 3rdParty/JQ
   git pull origin main
   cd ..\..
   git add 3rdParty/JQ
   git commit -m "Update JQ submodule"
   git push
   ```

## License

See LICENSE file if present, or refer to the original library's licensing terms.
