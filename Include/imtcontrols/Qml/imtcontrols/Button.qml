import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

Item {
    id: baseButton;

    property var decorator: style.createObject(baseButton);
    property Component style

   //required parameters in every decorator
    property string text: "";
    property string imageSource: "";

    property bool enabled: true;

    property bool isActive: false;
    property bool isFocused: false;
    property bool isPressed: false;
    property bool isHovered: enabled ? ma.containsMouse : false;
    property bool isMainButton: false;
    property bool isHighlighted: false;
    //

    property bool containsMouse: enabled ? ma.containsMouse : false;
    property bool selected: false;
    property string color: "";

    signal clicked();
    signal doubleClicked();
    signal pressed();
    signal released();
    signal entered();
    signal exited();

    signal loaded();    
    Component.onCompleted: {
        // decorator = decorator.createObject(container);
        decorator.baseButton = baseButton
    }

    // Binding {
    //     target: decorator
    //     property: "text"
    //     value: baseButton.text
    // }
    
    // Binding {
    //     target: decorator
    //     property: "color"
    //     value: baseButton.color
    // }

    // MouseArea {
    //     anchors.fill: parent
    //     // cursorShape: Qt.PointingHandCursor
    //     // hoverEnabled: false
    //     // onEntered: {}
    //     // onExited: {}
    //     // onWheel: {}
    //     onClicked: { 
    //         container.clicked() 
    //     }
    // }

       MouseArea{
        id: ma;

        anchors.fill: parent;

        enabled: baseButton.enabled;
        hoverEnabled: enabled;
        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;

        onClicked: {
            baseButton.clicked();
        }
        onDoubleClicked: {
            baseButton.doubleClicked();
        }
        onPressed: {
            baseButton.pressed();

        }
        onReleased: {
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