import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Rectangle {
    id: topLeftPanel;
    property Item topPanel;

    property string decoratorSource;
    onDecoratorSourceChanged: {
        if (topLeftPanel.decoratorSource){
            decoratorLoader.source = topLeftPanel.decoratorSource;
        }
    }

    Loader {
        id: decoratorLoader;

        onItemChanged: {
            if (decoratorLoader.item){
                decoratorLoader.item.width = parent.width;
                decoratorLoader.item.height = parent.height;
            }
        }
    }
}

