import QtQuick 2.12
import Acf 1.0

Text {
    id: titleLogin;

    color: Style.textColor;
    font.family: Style.fontFamily;
    font.pixelSize: Style.fontSize_common;

    elide: Text.ElideRight;

    Component{
        id: emptyDecorator;
        Item{
            property Item rootItem: null;
        }
    }

    Loader{
        id: titleDecoratorLoader1;

        sourceComponent: Style.inputTitleDecorator !==undefined ? Style.inputTitleDecorator: emptyDecorator;
        onLoaded: {}
    }
}
