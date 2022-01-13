import QtQuick 2.12
import Acf 1.0
//import QtGraphicalEffects 1.0

Item {
    id: container;
    height: 56;
    width: isEmpty ? 30 : 73;
    property bool enabled:  true;
    property string imageSource: "../Icons/Add.svg";
    property string imageSourceDisabled: "../Icons/Add_On_Disabled.svg";
    //property string imageSourceSelected: "../Icons/Add_On_Selected.svg"
    property string text: "New";
//    property string textColor: "#335777";//"#191970"
    property string textColor: Style.textColor;

    property string textColorDisabled: "gray";
    property real fontSize: 11;
    property string fontName: "";
    property int radius: 4;

    property bool isEmpty: false;
    property bool checkable: false;
    property bool isChecked: false;

    property bool highlighted: false;

    property string hint: "button";

    signal clicked;
    signal checked;
    signal hintShow;
    signal hintHide;





    Rectangle{
        id: button;
        anchors.top: parent.top;
        anchors.topMargin: 6;
        anchors.horizontalCenter: parent.horizontalCenter;
        width: parent.width - 10;
        height: parent.height/2;
        radius: container.radius;
        visible: !container.isEmpty;
//        color: (container.checkable && container.isChecked) ? "white" : container.highlighted ? "white" : "#fafafa";
//        color: Style.buttonColor;
        color: Style.theme == "Dark" ? "#424242" : "#f0f0f3";
//        color: "#c0c0c0";

//        gradient: Gradient {
//                 GradientStop { position: 0.0; color: Style.imagingToolsGradient1; }
//                 GradientStop { position: 0.25; color: Style.imagingToolsGradient2; }
//                 GradientStop { position: 0.5; color: Style.imagingToolsGradient3; }
//                 GradientStop { position: 1.0; color: Style.imagingToolsGradient4; }
//             }
//        border.color: Style.buttonBorderColor;
//        Rectangle{
//            id: background;
//            anchors.fill: parent;
//            color: "#a0a0a0";
////            opacity: (container.checkable && container.isChecked) ? 0 : container.highlighted ? 0 : 0.1;
//            opacity: 0.1;
//        }
        Image {
            id: image;
//            source: !container.enabled ? container.imageSourceDisabled : container.imageSource;
            source: container.imageSource;
            anchors.centerIn: parent;
            fillMode: Image.PreserveAspectFit;
            height: parent.height - 8;
            width: height;
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
        color: "#00FF00";
        visible: container.checkable ? container.isChecked : container.highlighted;
    }


    MouseArea{
        id: ma;
        anchors.fill: parent;
        enabled: container.enabled && container.visible && !container.isEmpty;
        hoverEnabled: enabled;
        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;
        onClicked: {
            console.log("onClicked")
            if (container.enabled) {
                return;
            }

            container.clicked();
        }
        onPressed: {
            console.log("onPressed")
            container.scale = 0.985;
            if(!container.checkable)
                container.highlighted = true;
            else
                container.checked();
        }
        onReleased: {
            console.log("onReleased")
            container.scale = 1;
            if(!container.checkable)
                container.highlighted = false;
        }

//        onEntered: {
//        container.hintShow();
//        }
//        onExited: {
//        container.hintHide();
//        }

    }

    Text {
        id: description;
        anchors.top: selection.bottom;
        anchors.topMargin: 0;
        anchors.horizontalCenter: parent.horizontalCenter;
//        height: contentHeight;
        text: container.text;
        color: container.enabled ? container.textColor : container.textColorDisabled;
        font.pixelSize: container.fontSize;
//        font.family: container.fontName;
        visible: !container.isEmpty;

        font.family: Style.fontFamily;
//        font.pixelSize: Style.fontSize_subtitle;
    }



}
