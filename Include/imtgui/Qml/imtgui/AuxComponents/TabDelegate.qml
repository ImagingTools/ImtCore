import QtQuick 2.12
import Acf 1.0

Rectangle{
    id: tabDelegate;
    height: 40;
    width: 160;
//    color: selected ? "white" : "transparent";
    color: selected ? Style.baseColor : "transparent";

    property bool selected: false;
    property bool firstElement: false;
    property string text: "no name";
    property string firstElementText: "text";
    property string firstElementImageSource:  "../../../" + "Icons/" + Style.theme + "/" + "Workflow" + "_On_Normal.svg";
//    property string firstElementImageSource: "../../Icons/Workflow.svg";
//    property string closeButtonImageSource: "../../Icons/DeleteStylized.svg";
    property bool autoWidth: true;
    signal clicked;
    signal closeSignal;




    Rectangle{
        id: selection;
        anchors.bottom: parent.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;
        height: 2;
        color: "#FF4500";
        visible: selected;
    }

    Item {
        id: imagetabDelegate;
        height: parent.height;
        width: visible ? height : 1;
        visible: tabDelegate.firstElement;

        Image {
            id: firsElementImage;
            anchors.centerIn: parent;
            width: parent.width * 0.6;
            height: parent.height * 0.6;
//            sourceSize.width: width;
//            sourceSize.height: height;
            fillMode: Image.PreserveAspectFit;
            source:  "../../../" + "Icons/" + Style.theme + "/" + tabDelegate.firstElementImageSource + "_On_Normal.svg";

//            source: tabDelegate.firstElementImageSource;
        }
    }



    Item {
        id: texttabDelegate;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.left: imagetabDelegate.right;
//        anchors.right: closeButton.left;
        width: text.width + tabDelegate.height;

        onWidthChanged: {
            if (tabDelegate.autoWidth){
                tabDelegate.width = texttabDelegate.width + tabDelegate.height
            }
        }

        Text {
            id: text;
            anchors.centerIn: parent;
            text: tabDelegate.text;
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

        }
    }

    MouseArea{
        id: ma;
        anchors.fill: parent;
//        enabled: tabDelegate.visible;
//        hoverEnabled: enabled;
//        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;
        onClicked: {
            tabDelegate.clicked();
        }
    }

    Item {
        id: closeButton;
        anchors.right: parent.right;
        anchors.rightMargin: 8;
        anchors.verticalCenter: parent.verticalCenter;
        height: parent.height * 0.4;
        width: height;
        visible: !firstElement;
        Image {
            id: closeImage;
            anchors.centerIn: parent;
            width: parent.width;
            height: parent.height;
//            sourceSize.width: width;
//            sourceSize.height: height;
            fillMode: Image.PreserveAspectFit;
            source: "../../../" + "Icons/" + Style.theme + "/" + "DeleteStylized" + "_On_Normal.svg";
        }

        MouseArea{
            id: closeMA;
            anchors.fill: parent;
//            enabled: tabDelegate.visible;
//            hoverEnabled: enabled;
//            cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;
            onClicked: {
                tabDelegate.closeSignal();
            }
        }
    }

}
