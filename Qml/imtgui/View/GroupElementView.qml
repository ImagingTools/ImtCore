import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
    id: container;

    width: 700;
    height: 100;

    border.width: 1;
    border.color: Style.borderColor;

    radius: 5;

    color: Style.baseColor;

    clip: true;

    property int contentHeight: -1;

    function calcHeight(){
        let h = 0;

        for (let i = 0; i < children.length; i++){
            if (children[i].height >= 0){
                h += children[i].height;

                if (i > 0){
                    h -= children[i].border.width
                }
            }
        }

        container.contentHeight = h;
        container.height = container.contentHeight;
    }

    Component.onCompleted: {
        for (let i = 0; i < children.length; i++){
            children[i].radius = 0;

            children[i].anchors.left = container.left;
            children[i].anchors.right = container.right;

            if (i == 0){
                children[i].anchors.top = container.top;
                children[i].radius = container.radius;
            }

            if (i == children.length - 1){
                children[i].radius = container.radius;
            }

            if (i > 0){
                children[i].anchors.top = children[i - 1].bottom;
                children[i].anchors.topMargin = -children[i].border.width;
            }

            children[i].heightChanged.connect(container.calcHeight);
        }

        calcHeight();
    }
}


