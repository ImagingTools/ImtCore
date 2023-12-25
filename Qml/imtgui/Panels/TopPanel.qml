import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Rectangle {
    id: topPanel;

    color: Style.backgroundColor;

    onWidthChanged: {
         if (topPanelDecoratorLoader.item){
             topPanelDecoratorLoader.item.width = topPanel.width;
         }
    }

    Loader {
        id: topPanelDecoratorLoader;

        source: Style.topPanelDecoratorPath;
        onSourceChanged: {
            console.log("topPanelDecoratorLoader onSourceChanged", topPanelDecoratorLoader.source)
        }

        onItemChanged: {
            if (topPanelDecoratorLoader.item){
                topPanelDecoratorLoader.item.width = topPanel.width;
                topPanelDecoratorLoader.item.height = topPanel.height;
                topPanelDecoratorLoader.item.topPanel = topPanel;
            }
        }
    }
}

