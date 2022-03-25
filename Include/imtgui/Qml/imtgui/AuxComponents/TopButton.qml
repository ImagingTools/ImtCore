import QtQuick 2.12
import Acf 1.0
import QtGraphicalEffects 1.12
//import QtGraphicalEffects 1.0

Item {
    id: container;

    height: 56;
    width: isEmpty ? 30 : 73;

    property string imageSource: "../Icons/Add.svg";
    property string imageSourceDisabled: "../Icons/Add_On_Disabled.svg";
    property string text: "New";
    property string textColor: Style.textColor;
    property string hint: "button";
    property string textColorDisabled: "gray";
    property string fontName: "";

    property real fontSize: 11;

    property int radius: 4;

    property bool isEmpty: false;
    property bool checkable: false;
    property bool isChecked: false;
    property bool enabled:  true;
    property bool highlighted: false;

    signal clicked;
    signal checked;
    signal hintShow;
    signal hintHide;

    DropShadow {
       id: dropShadow;

       anchors.fill: button;

       verticalOffset: 1;

       radius: 2;
       samples: 5;
       color: Style.shadowColor;
       source: button;

       visible: button.visible;
    }

    Rectangle{
        id: button;

        anchors.top: parent.top;
        anchors.topMargin: 6;
        anchors.horizontalCenter: parent.horizontalCenter;

        width: parent.width - 10;
        height: parent.height/2;

        radius: container.radius;
        visible: !container.isEmpty;

        color: Style.baseColor;

        gradient: selection.visible ? pressedGradientButton : standardGradientButton;

        Gradient {
            id: standardGradientButton;

            GradientStop { position: 0.0; color: Style.imagingToolsGradient0; }
            GradientStop { position: 1.0; color: Style.imagingToolsGradient1; }
        }

        Gradient {
            id: pressedGradientButton;

            GradientStop { position: 0.0; color: Style.imagingToolsGradient0; }
            GradientStop { position: 1.0; color: Style.imagingToolsGradient0; }
        }

        Image {
            id: image;

            anchors.centerIn: parent;

            height: parent.height - 8;
            width: height;

            source: container.imageSource;
            fillMode: Image.PreserveAspectFit;

            sourceSize.width: width;
            sourceSize.height: height;
        }
    }

    Rectangle{
        id: selection;

        anchors.top: button.bottom;
        anchors.horizontalCenter: parent.horizontalCenter;

        height: 4;
        width: button.width/1.8;
        color: Style.greenColor;
        visible: container.checkable && container.highlighted;
    }

    MouseArea{
        id: ma;

        anchors.fill: parent;

        enabled: container.enabled && container.visible && !container.isEmpty;
        hoverEnabled: enabled;
        cursorShape: containsMouse && container.checkable ? Qt.PointingHandCursor : Qt.ArrowCursor;

        onClicked: {
            if (container.checkable) {
                container.clicked();
            }
        }

        onPressed: {
            container.highlighted = container.checkable;
            container.scale = 0.985;
        }

        onReleased: {
            container.scale = 1;
            container.highlighted = false;
        }
    }

    Text {
        id: description;

        anchors.top: selection.bottom;
        anchors.topMargin: 0;
        anchors.horizontalCenter: parent.horizontalCenter;

        text: container.text;
        color: container.enabled ? container.textColor : container.textColorDisabled;
        font.pixelSize: container.fontSize;
        visible: !container.isEmpty;

        font.family: Style.fontFamily;
    }
}
