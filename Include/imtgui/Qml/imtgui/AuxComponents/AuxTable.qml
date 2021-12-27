import QtQuick 2.12
import Acf 1.0
//import '../UxAdvancedComponents' as AUX


Item {
    id: tableContainer;
    property alias selectedIndex: elementsList.selectedIndex;
    property real delegateWidth: tableContainer.count == 0 ? 0 : headersList.width/headersList.count;
    property int count: 3; //headersArray.length;
    property alias headersModel: headersList.model;
    property alias elementsModel: elementsList.model;

    property var headersArray: [];

    function clearHeadersArray(){
        while(tableContainer.headersArray.length > 0)
            tableContainer.headersArray.pop();
    }

    function addToHeadersArray(str){
        tableContainer.headersArray.push(str);
        headersList.model = tableContainer.headersArray.length
    }

    Rectangle {
        id: headersPanel;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top: parent.top;
        height: 35;
        color: "transparent";

        ListView{
            id: headersList;
            anchors.fill: parent;
            clip: true;
//            boundsBehavior: Flickable.StopAtBounds;
            orientation: ListView.Horizontal;
            spacing: 0;
            model: 0;
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
                    font.pixelSize: Style.fontSize_common;
                    font.family: Style.fontFamilyBold;
                    font.bold: true; //tableContainer.fontBold
                    color: Style.textColor;
                    text: tableContainer.headersArray[model.index];
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
        model: 0;
        delegate: TableDelegate {
            id: tableDelegate;
            width: elementsList.width;
            selected: elementsList.selectedIndex === model.index;
            Component.onCompleted: {
                for (var i = 0; i < headersArray.length; i++){
                    tableDelegate.addToArray(model[headersArray[i]])
                }
            }

            onClicked: {
                elementsList.selectedIndex = model.index;
            }

        }
    }

    PopupMenuDialog {
        id: popupMenuDialog;
        visible: false;
    }



}
