import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: leftPanelElement;

    property string title: "";
    property string imageSource: "";
    property bool visibleMarker: false;
    property int mainMargin: 14;
//    property alias model: menuList.model;

    signal accepted(string text);
    signal clicked();

    property bool highlighted;
    property bool selected: false;
    property string text;


//    Rectangle{
//        id: header;
//        width: parent.width;
//        height: Math.max(Style.size_indicatorHeight, titleText.height + 20);


//        Rectangle {
//            id: marker;

//            anchors.left: parent.left;

//            color: "#7700FF";

//            width: 5;
//            height: parent.height;
//            visible: leftPanelElement.selected;
//        }

//        Item {
//            id: iconContainer;
//            anchors.verticalCenter: parent.verticalCenter;
//            anchors.left: marker.right;
//            anchors.leftMargin: leftPanelElement.mainMargin;
//            width: 25;
//            height: 25;
//            Image {
//                id: icon;
//                anchors.centerIn: parent;
//                width: parent.width;
//                height: parent.height;
//                sourceSize.width: width;
//                sourceSize.height: height;
//                source: leftPanelElement.imageSource;

//            }
//        }

//        Text {
//            id: titleText;
//            anchors.left: iconContainer.right;
//            anchors.leftMargin: leftPanelElement.mainMargin;
//            anchors.verticalCenter: parent.verticalCenter;
//            width: parent.width - marker.width - iconContainer.width -
//                   iconContainer.anchors.leftMargin *2 -
//                   openImageContainer.width - openImageContainer.anchors.rightMargin *2;
////            height: contentHeight;
//            horizontalAlignment: Text.AlignLeft;
//            verticalAlignment: Text.AlignVCenter;
//            wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
//            font.pixelSize: fontSize;
//            font.bold: true;
//            font.family: Style.fontFamily;
//            color: leftPanelElement.selected ? Style.color_text_titles : Style.color_text_notActive;
//            text: leftPanelElement.title;
//            property int fontSize: Style.fontSize_small;
//            onFontSizeChanged: {
//                font.pixelSize = fontSize;
//            }
//        }

//        Item {
//            id: openImageContainer;
//            anchors.verticalCenter: parent.verticalCenter;
//            anchors.right: parent.right;
//            anchors.rightMargin: 10;
//            width: 10;
//            height: 10;
//            visible: model.SubPages ? true : false;
//            Image{
//                id: openImage;
//                anchors.centerIn: parent;
//                width: parent.width;
//                height: parent.height;
//                sourceSize.width: width;
//                sourceSize.height: height;
//                source: "../../../" + "Icons/" + Style.theme + "/" + "Down" + "_On_Normal.svg";
//            }
//        }

//    }//header


//    property string imageSource;


    Image {
        id: image;

        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.top: parent.top;
        anchors.topMargin: 5;

        width: 50;
        height: 50;

        fillMode: Image.PreserveAspectFit;

        source: container.imageSource;
        sourceSize.height: height;
        sourceSize.width: width;
    }

    Rectangle {
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.left: parent.left;

        width: 5;

        color: Style.iconColorOnSelected;
        visible: container.selected;
    }

    Text {
        id: description;

        anchors.bottom: parent.bottom;
        anchors.horizontalCenter: parent.horizontalCenter;

        text: container.text;
        color: container.selected ? Style.iconColorOnSelected: Style.textColor;
        font.pixelSize: container.fontSize;
        font.family: Style.fontFamily;
    }
}
