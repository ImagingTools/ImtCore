#!/bin/zsh

cd $(dirname $0)
$QTDIR/bin/lupdate ../../../Include ../../../Qml ../../../AuxInclude -target-language ru_RU -ts ImtCore_ru_RU.ts -recursive -locations relative