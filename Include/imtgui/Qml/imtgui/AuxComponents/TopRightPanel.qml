import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Rectangle {
    id: topRightPanel;

    height: 55;
    width: 100;

    property Item topPanel;

    property string decoratorSource;
    onDecoratorSourceChanged: {
        if (topRightPanel.decoratorSource){
            decoratorLoader.source = topRightPanel.decoratorSource;
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

