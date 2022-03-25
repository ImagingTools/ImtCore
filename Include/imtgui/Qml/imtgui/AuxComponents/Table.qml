import QtQuick 2.12
import Acf 1.0
//import '../UxAdvancedComponents' as AUX


Item {
    id: container;

    property alias selectedIndex: elementsList.selectedIndex;
    property alias headers: headersList.model;
    property alias elements: elementsList.model;

    property real delegateWidth: count == 0 ? 0 : headersList.width / headersList.count;

    property int count: headersArray.length;

    property var headersArray: ["First", "Second", "Third"];

    function clearHeadersArray() {
        while(headersArray.length > 0)
            headersArray.pop();
    }

    function addToHeadersArray(str) {
        headersArray.push(str);
    }

    Rectangle {
        id: headersPanel;

        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top: parent.top;

        height: 30;

        color: "transparent";

        Row {
            height: parent.height;

            Repeater {
                id: headersList;
                clip: true;

                model: 3;

                delegate: Rectangle {
                    id:deleg;

                    width: headersPanel.width/headersList.count;
                    height: headersPanel.height;

                    color: "transparent";

                    Text {
                        id: name;

                        anchors.verticalCenter: parent.verticalCenter;
                        anchors.left: parent.left;
                        anchors.leftMargin: 8;

                        font.bold: true; //container.fontBold
                        color: Style.textColor;
                        text: container.headersArray[model.index];

                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_subtitle;
                    }
                }
            }
        }

        Rectangle {
            id: bottomLine;

            anchors.left: parent.left;
            anchors.right: parent.right;
            anchors.bottom: parent.bottom;

            height: 1;

            color: "lightgray";
        }
    }//headers

    ListView {
        id: elementsList;

        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top: headersPanel.bottom;
        anchors.bottom: parent.bottom;

        clip: true;

        spacing: 0;
        model: 10;

        property int selectedIndex: -1;

        delegate: TableDelegate {
            width: elementsList.width;

            selected: elementsList.selectedIndex === model.index;

            onClicked: {
                elementsList.selectedIndex = model.index;
            }
        }
    }
}
