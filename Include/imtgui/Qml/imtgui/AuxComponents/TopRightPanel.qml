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

    Loader {
        id: decoratorLoader;

        onItemChanged: {
            if (decoratorLoader.item){
                console.log("topRightPanel");
                console.log("topRightPanel.width", topRightPanel.width);
                console.log("topRightPanel.height", topRightPanel.height);
                decoratorLoader.item.width = topRightPanel.width;
                decoratorLoader.item.height = topRightPanel.height;

                console.log("decoratorLoader.item.width", decoratorLoader.item.width);
                console.log("decoratorLoader.item.height", decoratorLoader.item.height);
            }
        }
    }
}

