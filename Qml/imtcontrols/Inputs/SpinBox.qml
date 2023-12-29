import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtcontrols 1.0

ControlBase{
    id: spinBox;

    decoratorComponent: Style.isQtStyle ? DecoratorsQt.spinBoxDecorator: Decorators.spinBoxDecorator;

    property var numberReg: /^\-{0,1}\d*\.{0,1}\d*$/;

    property real startValue: 0;
    property real from: 0;
    property real to: 99;
    property real stepSize: 1;

    property real value: startValue;

    property bool editable: true;
    property bool wrap: false;

    property string icon: "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal);

    property string fontColor: "#000000";
    property int fontSize: Style.fontSize_common;

    onDecoratorComponentChanged: {
        bindValue.target = decorator;
    }

    Binding {
        id: bindValue;
        property: "value";
        value: spinBox.value;
    }

}


