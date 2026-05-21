#!/bin/zsh

cd $(dirname $0)
$QTDIR/bin/lupdate ../../../Include ../../../Qml ../../../AuxInclude -target-language kk_KZ -ts ImtCore_kk_KZ.ts -recursive -locations relative
