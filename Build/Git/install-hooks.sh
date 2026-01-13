#!/bin/bash
cp ./post-merge ../../.git/hooks/post-merge
chmod +x ../../.git/hooks/post-merge
echo "Hook installed!"