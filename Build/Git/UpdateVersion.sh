#!/bin/bash

cd "$(dirname "$0")"
FILE="../../Partitura/ImtCoreVoce.arp/VersionInfo.acc.xtrsvn"

git fetch --prune --unshallow 2>/dev/null

# Get revision count from origin/master or HEAD
# Note: 'master' is hard-coded to match the Windows version behavior
REV=$(git rev-list --count origin/master 2>/dev/null)
if [ -z "$REV" ]; then
    REV=$(git rev-list --count HEAD 2>/dev/null)
fi
if [ -z "$REV" ]; then
    echo "Failed to compute revision count."
    exit 1
fi

REV_OFFSET=$((REV + 10000))

# Check if working directory is dirty
git diff-index --quiet HEAD --
if [ $? -eq 0 ]; then
    DIRTY=0
else
    DIRTY=1
fi

echo "Git revision: $REV (version: $REV_OFFSET), dirty: $DIRTY"
echo "Processing file: $FILE"

OUT="${FILE%.xtrsvn}"

# Replace placeholders: $WCREV$ with revision count, $WCMODS?1:0$ with dirty flag
# REV and DIRTY are numeric values from git, so they are safe to use directly in sed
sed -e "s/\\\$WCREV\\\$/$REV_OFFSET/g" -e "s/\\\$WCMODS?1:0\\\$/$DIRTY/g" "$FILE" > "$OUT"

echo "Wrote $OUT with WCREV=$REV_OFFSET and WCMODS=$DIRTY"
