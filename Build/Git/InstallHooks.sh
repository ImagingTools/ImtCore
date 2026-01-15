#!/bin/bash

cd "$(dirname "$0")"
cp ./post-merge ../../.git/hooks/post-merge
chmod +x ../../.git/hooks/post-merge
echo "Hook installed!"
