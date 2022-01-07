import QtQuick 2.12
import Acf 1.0

Item {
    id: container;
    width: 64;
    height: 64;
    property string text: "Test";
    property string imageSource: "../Icons/FeaturePackage.svg";
    property string selectionColor: "#00BFFF";
    property string textColor: "#335777";//"#191970"
    property string fontName: "";
    property bool enabled:  true;
    property bool selected: false;
    property bool highlighted: ma.containsMouse && container.enabled;
    property real imageDecrease: 0.63;
    property real imageSelectedCoeff: 0.73;
    property real fontSize: 11;
    signal clicked;


    Image {
        id: image;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.top: parent.top;
        anchors.topMargin: 5; // description.height/2
        fillMode: Image.PreserveAspectFit;
        width: container.highlighted || container.selected ? parent.width * imageSelectedCoeff : parent.width * imageDecrease;
        height:  container.highlighted || container.selected ? parent.height * imageSelectedCoeff : parent.height * imageDecrease;
        source: container.imageSource;

    }

    Rectangle{
      //  id: selection;
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
        anchors.horizontalCenter: container.horizontalCenter;
        text: container.text;
        color: container.selected ? Style.iconColorOnSelected: Style.textColor;
        font.pixelSize: container.fontSize;
        font.family: Style.fontFamily;

    }

    MouseArea{
        id: ma;
        anchors.fill: parent;
        hoverEnabled: enabled;
        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;
        onClicked: {
            container.clicked();
        }
    }

}
