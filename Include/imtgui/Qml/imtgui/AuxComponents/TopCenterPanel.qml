import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Rectangle {
    id: topCenterPanel;

    clip: true;

    property Item topPanel;

    property string decoratorSource;

    onDecoratorSourceChanged: {
        if (topCenterPanel.decoratorSource){
            customLoader.source = topCenterPanel.decoratorSource;
        }
    }

    onWidthChanged: {
        if (topCenterPanel.decoratorSource){
            customLoader.item.width = topCenterPanel.width;
        }
    }

    Item {
        id: mainItem;

        anchors.left: parent.left;
        anchors.leftMargin: 10;
        anchors.right: parent.right;
        anchors.rightMargin: 10;

        height: parent.height;

        Loader {
            id: customLoader;

            onLoaded: {
                console.log("customLoader onLoaded");
                customLoader.item.width = topCenterPanel.width;
                customLoader.item.height = topCenterPanel.height;
            }

//            onItemChanged: {
//                if (customLoader.item){
//                    customLoader.item.width = topCenterPanel.width;
//                    customLoader.item.height = topCenterPanel.height;
//                }
//            }
        }
    }
}

