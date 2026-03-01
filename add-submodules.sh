#!/bin/bash

# Script to add all 3rdParty libraries as git submodules
# Run this after creating and populating all GitHub repositories

set -e

GITHUB_ORG="ImagingTools"
REPO_PREFIX=""

# Libraries to add as submodules
LIBRARIES=(
    "EJ"
    "JQ"
    "JQML"
    "JQML2"
    "QXlsx"
    "WebCompiler"
    "boost"
    "icu"
    "libmdbx"
    "mongoc"
    "mongocxx"
    "nodejs"
    "openssl"
    "pybind11"
    "quazip"
    "zlib"
    "zstd"
)

echo "Adding submodules..."

for lib in "${LIBRARIES[@]}"; do
    repo_url="https://github.com/${GITHUB_ORG}/${lib}.git"
    submodule_path="3rdParty/${lib}"
    
    echo "Adding ${lib}..."
    git submodule add "${repo_url}" "${submodule_path}"
done

echo "Initializing and updating submodules..."
git submodule init
git submodule update

echo "Done! All submodules added."
echo "Don't forget to commit the changes:"
echo "  git commit -m 'Convert 3rdParty libraries to git submodules'"
