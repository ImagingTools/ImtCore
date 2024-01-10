import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

ControlBase{
    id: busyIndicator;

    decorator: Style.busyIndicatorDecorator;

    function start(){
        console.log("BusyIndicator start");

        stop();

        visible = true;
    }

    function stop(){
        console.log("BusyIndicator stop");
        visible = false;
    }

}

