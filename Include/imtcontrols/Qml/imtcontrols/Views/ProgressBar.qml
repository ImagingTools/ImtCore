import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtcontrols 1.0

Item {
    id: progressBar;

    width: decorator ? decorator.width : 0;
    height: decorator ? decorator.height : 0;

    property Component decoratorComponent;
    property var decorator : null;

    property int barHeight: 20;

    property bool hasText: true;
    property bool hasTitle: true;

    property string title: qsTr("Прогресс:");
    property string text: ""; //+ progressBar.percent + "%";

    property int percent: position * 100;
    property real from: 0.0;
    property real to: 1.0;
    property real value: 0.0;
    property real position: (value - from)/(to - from);
    property bool indeterminate: false;

    onDecoratorComponentChanged: {
        if(!decoratorComponent){
            return;
        }
        if(decorator){
            decorator.destroy()
        }
        decorator = decoratorComponent.createObject(progressBar)
        decorator.baseElement = progressBar
        bindWidth.target = decorator
        bindHeight.target = decorator
    }


    Binding {
        id: bindWidth
        property: "width"
        value: progressBar.width;
    }

    Binding {
        id: bindHeight
        property: "height"
        value: progressBar.height;
    }

}

