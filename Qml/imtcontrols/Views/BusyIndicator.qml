import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

ControlBase{
    id: busyIndicator;

    decorator: Style.busyIndicatorDecorator;

    function start(){
        stop();

        visible = true;
    }

    function stop(){
        visible = false;
    }
}

