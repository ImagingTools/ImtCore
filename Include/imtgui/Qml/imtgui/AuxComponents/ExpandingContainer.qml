import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

Item {
    id: expandingContainer;

    clip: true;

    property int startWidth: 0;
    property int startHeight: 0;
    property int finishWidth: 0;
    property int finishHeight: 0;

    property int duration: 200;

    property bool isOpen: false;

    function open(){
        if(!isOpen){
            if(startWidth <= finishWidth){
                animWidthTo.start();
            }
            if(startHeight <= finishHeight){
                animHeightTo.start();
            }
        }
    }

    function close(){
        if(isOpen){
            if(startWidth <= finishWidth){
                animWidthFrom.start();
            }
            if(startHeight <= finishHeight){
                animHeightFrom.start();
            }
        }
    }


    NumberAnimation {
        id: animWidthTo;

        target: expandingContainer
        property: "width";
        duration: expandingContainer.duration;
        from: expandingContainer.startWidth;
        to: expandingContainer.finishWidth;
        onFinished: {
            expandingContainer.isOpen = true;
        }

    }

    NumberAnimation {
        id: animWidthFrom;

        target: expandingContainer
        property: "width";
        duration: expandingContainer.duration;
        from: expandingContainer.finishWidth;
        to: expandingContainer.startWidth;
        onFinished: {
            expandingContainer.isOpen = false;
        }


    }

    NumberAnimation {
        id: animHeightTo;

        target: expandingContainer
        property: "height";
        duration: expandingContainer.duration;
        from: expandingContainer.startHeight;
        to: expandingContainer.finishHeight;
        onFinished: {
            expandingContainer.isOpen = true;
        }

    }

    NumberAnimation {
        id: animHeightFrom;

        target: expandingContainer
        property: "height";
        duration: expandingContainer.duration;
        from: expandingContainer.finishHeight;
        to: expandingContainer.startHeight;
        onFinished: {
            expandingContainer.isOpen = false;
        }
    }
}

