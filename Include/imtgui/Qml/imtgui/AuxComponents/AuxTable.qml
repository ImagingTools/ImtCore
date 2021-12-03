import QtQuick 2.12
import Acf 1.0
//import '../UxAdvancedComponents' as AUX


Item {
    id: container;
    property alias selectedIndex: elementsList.selectedIndex;
    property int fontSize: 12;
    property bool fontBold: true;
    property string fontName: "";
    property real delegateWidth: container.count == 0 ? 0 : headersList.width/headersList.count;
    property int count: 3; //headersArray.length;
    property alias headersModel: headersList.model;
    property alias elementsModel: elementsList.model;

    property var headersArray: ["First", "Second", "Third"];

    function clearHeadersArray(){
        while(headersArray.length > 0)
            headersArray.pop();
    }

    function addToHeadersArray(str){
        headersArray.push(str);
    }

    Rectangle {
        id: headersPanel;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top: parent.top;
        height: 30;
        color: "transparent";

        ListView{
            id: headersList;
            anchors.fill: parent;
            clip: true;
//            boundsBehavior: Flickable.StopAtBounds;
            orientation: ListView.Horizontal;
            spacing: 0;
            model: 3;
            delegate: Rectangle{
                id:deleg;
                width: headersList.width/headersList.count;
                height: headersList.height;
                //color: "transparent";
                color: Style.backgroundColor;
                Text {
                    id: name;
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.left: parent.left;
                    anchors.leftMargin: 8;
                    font.pixelSize: container.fontSize;
                    font.family: container.fontName;
                    font.bold: true; //container.fontBold
                    color: Style.textColor;
                    text: container.headersArray[model.index];
                }
            }
        }


        Rectangle{
            id: bottomLine;
            anchors.left: parent.left;
            anchors.right: parent.right;
            anchors.bottom: parent.bottom;
            height: 1;
            color: "lightgray";
        }

    }//headers



    ListView{
        id: elementsList;
        property int selectedIndex: -1;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top: headersPanel.bottom;
        anchors.bottom: parent.bottom;
        clip: true;
//        boundsBehavior: Flickable.StopAtBounds;
        spacing: 0;
        model: 10;
        delegate: TableDelegate {
            width: elementsList.width;
            fontName: container.fontName;
            selected: elementsList.selectedIndex === model.index;
            onClicked: {
                elementsList.selectedIndex = model.index;
            }

        }
    }


}
