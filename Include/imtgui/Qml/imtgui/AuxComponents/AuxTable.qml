import QtQuick 2.12
import Acf 1.0
//import '../UxAdvancedComponents' as AUX


Item {
    id: tableContainer;

    property int selectedIndex: -1;
    property int count: 3; //headersArray.length;

    property real delegateWidth: tableContainer.count == 0 ? 0 : headersList.width/headersList.count;

    property TreeItemModel headers; //: elementsList.model;

    property Item activeItem; //: elementsList.model;

    property alias delegate: elementsList.delegate;
    property alias elements: elementsList.model;

    signal selectItem(string idSelected, string name);
    signal rightButtonMouseClicked(Item item, int mouseX, int mouseY);
    signal setActiveFocusFromTable();

    function getSelectedId(){
        return elementsList.selectedId;
    }

    function getSelectedName(){
        return elementsList.selectedName;
    }

    function changeDataByIndex(index){
        console.log("AuxTable changeDataByIndex", index);
        elementsList.selectedId = elements.GetData("Id", index);
        elementsList.selectedName  = elements.GetData("Name", index);
        tableContainer.selectedIndex = index;
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

                color: Style.baseColor;

                Text {
                    id: name;

                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.left: parent.left;
                    anchors.leftMargin: 8;

                    font.pixelSize: Style.fontSize_common;
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

    Rectangle {
        anchors.fill: elementsList;
        color: Style.baseColor;
    }

    ListView {
        id: elementsList;

        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top: headersPanel.bottom;
        anchors.bottom: parent.bottom;

        clip: true;
        spacing: 0;
        boundsBehavior: Flickable.StopAtBounds;

        property string selectedId;
        property string selectedName;

        delegate: TableDelegate {
            id: tableDelegate;

            width: elementsList.width;

            selected: tableContainer.selectedIndex === model.index;

            Component.onCompleted: {
                for(var i = 0; i < tableContainer.headers.GetItemsCount(); i++){
                    tableDelegate.addToArray(model[tableContainer.headers.GetData("Id",i)]);
                }
            }

            onClicked: {
                elementsList.selectedId = model["Id"];
                elementsList.selectedName = model[tableContainer.headers.GetData("Id",0)];
                tableContainer.selectedIndex = model.index;
                tableContainer.setActiveFocusFromTable();
            }

            onTableDelegateRrightButtonMouseClicked: {
                console.log("AuxTable onRightButtonMouseClicked", mX, mY);
                var tempX = mX;
                var tempY = (model.index + 1) * tableDelegate.height + mY;
                console.log("tempX", tempX);
                console.log("tempY", tempY);
                tableContainer.rightButtonMouseClicked(thubnailDecoratorContainer, tempX, tempY);
            }

            onDoubleClicked: {
                console.log("onDoubleClicked", model["Id"], model["Name"])
                tableContainer.selectItem(model["Id"], model[tableContainer.headers.GetData("Id", 0)]);
            }
        }
    }

}
