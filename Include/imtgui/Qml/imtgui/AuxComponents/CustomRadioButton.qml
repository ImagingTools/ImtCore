import QtQuick 2.0
import Acf 1.0;

Item {
    id: customRadioButton;

    property int checkSize: 20;

    width: customRadioButton.text == "" ? checkRect.width :
                                          checkRect.width + checkBoxText.width + checkBoxText.anchors.leftMargin;

    height: Math.max(checkSize, checkBoxText.height);

    property alias text: checkBoxText.text;

    property alias border: checkRect.border;
    property alias color: checkRect.color;
    property alias radius: checkRect.radius;

    property int checkState: Qt.Unchecked;

    property alias textObj: checkBoxText;

    property int fontPixelSize: Style.fontSize_common;
    property bool fontBold: false;
    property alias fontColor: checkBoxText.color;

    property int mainMargin: 8;

    property string selectedColor: "#000000";

    signal clicked();

    Rectangle {
        id: checkRect;

        anchors.verticalCenter: parent.verticalCenter;

        height: customRadioButton.checkSize;
        width: height;
        radius: height;

        color: "transparent";

        border.width: 1;
        border.color: "#000000";


        Rectangle{
            id: circle

            anchors.centerIn: parent;

            height: checkRect.height - 2*checkRect.border.width;
            width: height;
            radius: width;
            border.color: Style.borderColor;
            border.width: 1;
            color: Style.color_shadow

            Rectangle{
                id: selectCircle

                anchors.centerIn: parent;

                height: circle.height - 8;
                width: height;
                radius: width;
                color: customRadioButton.selectedColor;
                visible: customRadioButton.checkState == Qt.Checked;
            }
        }

    }

    Text {
        id: checkBoxText;

        anchors.left: checkRect.right;
        anchors.leftMargin: customRadioButton.mainMargin;
        anchors.verticalCenter: customRadioButton.verticalCenter;

        color: customRadioButton.enabled ? Style.buttonText : Style.inactive_buttonText;

        font.pixelSize: customRadioButton.fontPixelSize;
        font.family: Style.fontFamily;
        font.bold: customRadioButton.fontBold;
    }

    MouseArea {
        anchors.fill: parent;

        cursorShape: Qt.PointingHandCursor;

        visible: customRadioButton.enabled;

        onClicked: {
            customRadioButton.clicked();
        }
    }
}
