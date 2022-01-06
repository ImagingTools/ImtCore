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

    property var headerKeysArray: [];
    property var headerNamesArray: [];

    signal selectItem(string itemId, string name);

    function clearHeadersArray(){
        while(tableContainer.headerKeysArray.length > 0)
            tableContainer.headerKeysArray.pop();
        while(tableContainer.headerNamesArray.length > 0)
            tableContainer.headerNamesArray.pop();
    }

    function addToHeadersArray(strKey, strName){
        tableContainer.headerKeysArray.push(strKey);
        tableContainer.headerNamesArray.push(strName);
        headersList.model = tableContainer.headerKeysArray.length
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
            interactive: false;
            delegate: Rectangle{
                id:deleg;
                width: headersList.width/headersList.count;
                height: headersList.height;
                //color: "transparent";
                color: Style.theme === "Light" ? "white": Style.backgroundColor;
                Text {
                    id: name;
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.left: parent.left;
                    anchors.leftMargin: 8;
                    font.pixelSize: Style.fontSize_small;
                    font.family: Style.fontFamilyBold;
                    font.bold: true; //tableContainer.fontBold
                    color: Style.textColor;
                    text: tableContainer.headerNamesArray[model.index];
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
        interactive: false;
        delegate: TableDelegate {
            id: tableDelegate;
            width: elementsList.width;
            selected: elementsList.selectedIndex === model.index;
            Component.onCompleted: {
                for (var i = 0; i < tableContainer.headerKeysArray.length; i++){
                    console.log("AUXTable addToArray", model[tableContainer.headerKeysArray[i]])
                    tableDelegate.addToArray(model[tableContainer.headerKeysArray[i]])
                }
            }

            onClicked: {
                elementsList.selectedIndex = model.index;
            }

            onDoubleClicked: {
                /*
                    У Accounts нет поля Name, нужно model["AccountName"]
                  */
                console.log("onDoubleClicked", model["Id"], model["Name"])
                tableContainer.selectItem(model["Id"], model["Name"]);
            }
        }
    }

    PopupMenuDialog {
        id: popupMenuDialog;
        visible: false;
    }

}
