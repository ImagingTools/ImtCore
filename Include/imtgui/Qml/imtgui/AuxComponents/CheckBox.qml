import QtQuick 2.0

Rectangle {
    id: container;
    width: 10;
    height: width;
    color: checkState == 2 ? "green" : (checkState == 0) ? "white" : "black";
    border.color: "black";

    property int checkState: Qt.Unchecked;

}
