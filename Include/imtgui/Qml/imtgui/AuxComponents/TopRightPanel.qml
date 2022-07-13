import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Rectangle {
    id: topRightPanel;


    property Item topPanel;

    property string decoratorSource;
    onDecoratorSourceChanged: {
        console.log("topRightPanel onDecoratorSourceChanged")
        console.log("height", parent.height)
        console.log("width", parent.width)
        if (topRightPanel.decoratorSource){
            decoratorLoader.source = topRightPanel.decoratorSource;
        }
    }

    onWidthChanged: {
        if (decoratorLoader.item){
            decoratorLoader.item.width = topRightPanel.width;
        }
    }

    onHeightChanged: {
        if (decoratorLoader.item){
            decoratorLoader.item.height = topRightPanel.height;
        }
    }

//    Rectangle {
//        anchors.fill: parent;
//        color: "red";
//    }

    Loader {
        id: decoratorLoader;

        onItemChanged: {
            if (decoratorLoader.item){
                decoratorLoader.item.width = topRightPanel.width;
                decoratorLoader.item.height = topRightPanel.height;
            }
        }
    }
}

