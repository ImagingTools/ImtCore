import QtQuick 2.12
import Acf 1.0;
import imtqml 1.0
import imtcontrols 1.0


Flickable{
    id: swipeView;

    clip: true;
    boundsBehavior: Flickable.StopAtBounds;
    contentHeight: orientation == Qt.Vertical ? (height * count) : height;
    contentWidth: orientation == Qt.Horizontal ? (width * count) : width;

    property int orientation: Qt.Horizontal;
    property int count: children[0].children.length;
    property int currentIndex: 0;
    property int limit: 40;

    property real _movementStartContentX: 0;
    property real _movementStartContentY: 0;

    Component.onCompleted: {
        console.log("swipe_children.length:: ", children[0].children.length)
        for(let i = 0; i < children[0].children.length; i++){
            if(orientation == Qt.Horizontal){
                children[0].children[i].height = height;
                children[0].children[i].width = height;
                children[0].children[i].x = i* height;
                children[0].children[i].y = 0;
            }
            else if(orientation == Qt.Vertical){
                children[0].children[i].height = width;
                children[0].children[i].width = width;
                children[0].children[i].y = i* width;
                children[0].children[i].x = 0;
            }
        }
    }

    onContentXChanged: {
        if(orientation == Qt.Horizontal){
            //console.log("contentX:: ", contentX);
            let coeff = contentX - _movementStartContentX >= 0 ? 1 : -1;
            if(coeff * (contentX - (currentIndex * height )) >= 0) {
                cancelFlick();
            }
        }
    }

    onContentYChanged: {
        if(orientation == Qt.Vertical){
            //console.log("contentY:: ", contentY);
            let coeff = contentY - _movementStartContentY >= 0 ? 1 : -1;
            if(coeff * (contentY - (currentIndex * width )) >= 0) {
                cancelFlick();
            }
        }
    }

    onMovementEnded: {
        if(orientation == Qt.Horizontal){
            let coeff = contentX - _movementStartContentX >= 0 ? 1 : -1;

            if(coeff * (contentX - (currentIndex * height + coeff * limit)) < 0){
                setCoordAnim.from = contentX;
                setCoordAnim.to = currentIndex * height;
                setCoordAnim.start();

            }
            else{
                setCoordAnim.from = contentX;
                setCoordAnim.to = (currentIndex + coeff * 1) * height;
                setCoordAnim.start();

                if(coeff > 0){
                    currentIndex++;
                }
                else{
                    currentIndex--;
                }
            }
            _movementStartContentX = currentIndex * height;
        }

        else if(orientation == Qt.Vertical){
            let coeff = contentY - _movementStartContentY >= 0 ? 1 : -1;

            if(coeff * (contentY - (currentIndex * width + coeff * limit)) < 0){
                setCoordAnim.from = contentY;
                setCoordAnim.to = currentIndex * width;
                setCoordAnim.start();
            }

            else {
                setCoordAnim.from = contentY;
                setCoordAnim.to = (currentIndex + coeff * 1) * width;
                setCoordAnim.start();

                if(coeff > 0){
                    currentIndex++;
                }
                else{
                    currentIndex--;
                }
            }
            _movementStartContentY = currentIndex * width;

        }
    }


    NumberAnimation {
        id: setCoordAnim;

        target: swipeView;
        property: swipeView.orientation == Qt.Horizontal ? "contentX" : "contentY";
        duration: 200;
        from: 0;
        to:0;
    }

}

