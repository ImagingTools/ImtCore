import QtQuick 2.0
import Acf 1.0;

Rectangle{
    id: radioBox;

    width: list.width + 2* mainMargin;
    height: 24;
    color: "transparent";

    property alias model: list.model;
    property alias spacing: list.spacing;
    property alias selectedIndex: list.selectedIndex;
    property alias listView: list;

    property int mainMargin: 2;

    ListView{
        id:list;

        anchors.centerIn: parent;

        width: contentWidth;
        height: radioBox.height - 2*radioBox.mainMargin;
        boundsBehavior: Flickable.StopAtBounds;
        orientation: ListView.Horizontal;
        spacing: 16;
        clip: true;
        property int selectedIndex: -1;
        model: 5;
        delegate:
            CustomRadioButton{
            anchors.verticalCenter: parent.verticalCenter;
            text: model.text;
            checkState: model.index == list.selectedIndex ? Qt.Checked : Qt.Unchecked;
            checkSize: list.height;
            onClicked: {
                list.selectedIndex = model.index;
            }
        }
    }
}
