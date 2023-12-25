import QtQuick 2.12
import Acf 1.0

Rectangle{
    id: container;
    property bool enabled:  true;
    property string imageSource: "../Icons/Add.svg";
    property string imageSourceDisabled: "../Icons/Add_On_Disabled.svg";
    property string imageSourcePressed: "../Icons/Add.svg";
    property string imageSourcePressedDisabled: "../Icons/Add_On_Disabled.svg";

    property string text: "";
    property string textColor: Style.textColor;
    property string fontName: "";
    property int fontSize: 11;

    property real imageDecreaseCoeff: container.text == "" ? 0.9 : 0.4;

    property bool pressable: false;
    property bool pressed: false;

    property string hint: "button";

    signal clicked;

    anchors.verticalCenter: parent.verticalCenter;
    height: 56; //parent.height/1.5
    width: height;
    border.width: 1;
    border.color: ma.containsMouse ? "gray" : "transparent";
    radius: 2;
    color: ma.containsMouse ? "white" : "transparent";
    Image {
        id: image;
        anchors.centerIn: parent;
        anchors.verticalCenterOffset: container.text == "" ? 0 : -container.height/2 + height/2 + 4;
        width: Math.min(parent.width,parent.height) * imageDecreaseCoeff;
        height:  Math.min(parent.width,parent.height) * imageDecreaseCoeff;
        fillMode: Image.PreserveAspectFit;
        source: (container.enabled && container.pressed) ?imageSourcePressed : (container.enabled && !container.pressed) ?
                                                               imageSource : (!container.enabled && container.pressed) ?
                                                                   imageSourcePressedDisabled : imageSourceDisabled;

    }

    Text {
        id: name;
        anchors.horizontalCenter: container.horizontalCenter;
        anchors.bottom: container.bottom;
        anchors.bottomMargin: 4;
        color: container.enabled ? container.textColor : "gray";
        text: container.text;

        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_subtitle;
    }

    MouseArea{
        id: ma;
        anchors.fill: parent;
        enabled: container.enabled && container.visible;
        hoverEnabled: enabled;
        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;
        onClicked: {
            if(pressable)
                container.pressed = !container.pressed;
            container.clicked();
        }
    }

}
