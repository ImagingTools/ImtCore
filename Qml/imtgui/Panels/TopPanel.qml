import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: topPanel;

    height: decorator ? decorator.height : 0

    property var decorator: null;
    property Component decoratorComponent: Decorators.topPanelDecorator;

    Component.onCompleted: {
        if (decoratorComponent){
            //console.log("decoratorComponent::", decoratorComponent)
            decorator = decoratorComponent.createObject(topPanel)
//            decorator.baseButton = baseButton
            bindWidth.target = decorator
            bindHeight.target = decorator
//            bindHeight.target = decorator.baseButton
        }
    }

    Binding {
        id: bindWidth;

        //target: topPanel.decorator ? topPanel.decorator : null
        property: "width"
        value: topPanel.width;
    }

    Binding {
        id: bindHeight

        property: "height"
        value: topPanel.height;
    }

}

