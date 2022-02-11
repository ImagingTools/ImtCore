import QtQuick 2.12
import Acf 1.0
//import '../UxAdvancedComponents' as AUX


Item {
    id: tableContainer;
    property int selectedIndex: -1;
    property real delegateWidth: tableContainer.count == 0 ? 0 : headersList.width/headersList.count;
    property int count: 3; //headersArray.length;
    property alias elements: elementsList.model;
    property TreeItemModel headers; //: elementsList.model;
    property alias delegate: elementsList.delegate;
    signal selectItem(string selectedId, string name);

    signal rightButtonMouseClicked(Item item, int mouseX, int mouseY);

    function getSelectedId(){
        return elementsList.selectedId;
    }

    function getSelectedName(){
        return elementsList.selectedName;
    }

    MouseArea {
        id: maTable;
        anchors.fill: parent;

        onClicked: {
            console.log("AuxTable MouseArea onClicked");
            tableContainer.selectedIndex = -1;
        }
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
            orientation: ListView.Horizontal;
            spacing: 0;
            model: tableContainer.headers;
            boundsBehavior: Flickable.StopAtBounds;
            delegate: Rectangle{
                id: deleg;
                width: headersList.width/headersList.count;
                height: headersList.height;
                color: Style.theme === "Light" ? "white": Style.backgroundColor;
                Text {
                    id: name;
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.left: parent.left;
                    anchors.leftMargin: 8;
                    font.pixelSize: Style.fontSize_small;
                    font.family: Style.fontFamilyBold;
                    font.bold: true;
                    color: Style.textColor;
                    text: model.Name;
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

    ListView {
        id: elementsList;
        property string selectedId;
        property string selectedName;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top: headersPanel.bottom;
        anchors.bottom: parent.bottom;
        clip: true;
        spacing: 0;
        boundsBehavior: Flickable.StopAtBounds;
        delegate: TableDelegate {
            id: tableDelegate;
            width: elementsList.width;
            selected: tableContainer.selectedIndex === model.index;
            Component.onCompleted: {

                console.log("elementsList tableDelegate", model["Id"], model.index)
                console.log("elements.GetItemsCount",tableContainer.elements.GetItemsCount())
                for(var i = 0; i < tableContainer.headers.GetItemsCount(); i++){
                    tableDelegate.addToArray(model[tableContainer.headers.GetData("Id",i)]);
                }
            }

            onClicked: {
                console.log("elements.GetItemsCount", tableContainer.selectedIndex, model.index, elementsList.selectedId, elementsList.selectedName)
                elementsList.selectedId = model["Id"];
                elementsList.selectedName = model[tableContainer.headers.GetData("Id",0)];
                tableContainer.selectedIndex = model.index;
                console.log("TableDelegate onClicked", tableContainer.selectedIndex, elementsList.selectedId, elementsList.selectedName)
            }

            onTableDelegateRrightButtonMouseClicked: {
                console.log("AuxTable onRightButtonMouseClicked");
                var x = mouseX;
                var y = model.index * tableDelegate.height + mouseY;

                tableContainer.rightButtonMouseClicked(tableContainer, x, y);
            }

//            onRightButtonMouseClicked: {

//            }

            onDoubleClicked: {
                console.log("onDoubleClicked", model["Id"], model["Name"])
                tableContainer.selectItem(model["Id"], model[tableContainer.headers.GetData("Id",0)]);
//                tableContainer.selectItem(model["Id"], model["Name"]);
            }
        }

        onModelChanged: {
            console.log("AuxTable elements model onModelChanged");

//            console.log("count = ", elementsList.model.GetItemsCount());

//            for (var i = 0; i < elementsList.model.GetItemsCount(); i++) {
//                console.log("Name ", elementsList.model.GetData("Name", i));
//            }
        }
    }

}
