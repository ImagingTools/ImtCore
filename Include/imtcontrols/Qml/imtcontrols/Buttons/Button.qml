import QtQuick 2.12
import Acf 1.0
//import imtcontrols 1.0
//import imtqml 1.0

Item {
    id: baseButton;

    width: decorator ? decorator.width : 0
    height: decorator ? decorator.height : 0

    property Component decoratorComponent
    property var decorator : null

    //required parameters in every decorator
    property string text: ""
    property string iconSource: ""

    property bool down: false
    property bool hoverEnabled: true
    property bool hovered: enabled && hoverEnabled ? ma.containsMouse : false;
    property bool checkable: false;
    property bool checked: false;

    property int focusPolicy: Qt.WheelFocus;
    property int focusReason: -1;

    signal clicked();
    signal doubleClicked();
    signal pressed();
    signal released();
    signal entered();
    signal exited();
    signal toggled();

    QtObject {
        id: _private
        function onClicked(){
            baseButton.focus = true;
            baseButton.forceActiveFocus();
            if (baseButton.checkable){
                baseButton.checked = !baseButton.checked
                baseButton.toggled()
            }
            else{
                baseButton.clicked();
            }
        }
    }

    Component.onCompleted: {
    }

    onDecoratorComponentChanged: {
        if(!decoratorComponent){
            return;
        }
        //console.log("decoratorComponent::button", decoratorComponent)

        if(decorator){
            decorator.destroy()
        }
        decorator = decoratorComponent.createObject(baseButton)
        decorator.baseElement = baseButton
        bindWidth.target = decorator
        bindHeight.target = decorator

        if(focus){
            forceActiveFocus()
        }

    }


    Keys.onPressed: {
        if (event.key === Qt.Key_Return || event.key === Qt.Key_Space){
            event.accepted = true;
            _private.onClicked()
        }
    }

    Binding {
        id: bindWidth
        property: "width"
        value: baseButton.width;
    }

    Binding {
        id: bindHeight
        property: "height"
        value: baseButton.height;
    }

    MouseArea{
        id: ma;

        anchors.fill: parent;

        z: 10;

        enabled: baseButton.enabled;
        hoverEnabled: enabled;
        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;

        onClicked: {
            _private.onClicked()
        }
        onDoubleClicked: {
            baseButton.doubleClicked();
        }
        onPressed: {
            baseButton.down = true
            baseButton.pressed();

        }
        onReleased: {
            baseButton.down = false
            baseButton.released();
        }
        onEntered: {
            baseButton.entered();
        }
        onExited: {
            baseButton.exited();
        }

    }

}
