#!/bin/bash

cd "$(dirname "$0")"
FILE="../../Partitura/ImtCoreVoce.arp/VersionInfo.acc.xtrsvn"

git fetch --prune --unshallow 2>/dev/null

REV=$(git rev-list --count origin/master 2>/dev/null)
if [ -z "$REV" ]; then
    REV=$(git rev-list --count HEAD 2>/dev/null)
fi
if [ -z "$REV" ]; then
    echo "Failed to compute revision count."
    exit 1
fi

git diff-index --quiet HEAD --
if [ $? -eq 0 ]; then
    DIRTY=0
else
    DIRTY=1
fi

echo "Git revision: $REV, dirty: $DIRTY"
echo "Processing file: $FILE"

OUT="${FILE%.xtrsvn}"

sed -e "s/\\\$WCREV\\\$/$REV/g" -e "s/\\\$WCMODS?1:0\\\$/$DIRTY/g" "$FILE" > "$OUT"

echo "Wrote $OUT with WCREV=$REV and WCMODS=$DIRTY"
