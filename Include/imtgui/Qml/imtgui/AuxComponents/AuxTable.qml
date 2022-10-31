import QtQuick 2.12
import Acf 1.0

Item {
    id: tableContainer;

    property int selectedIndex: -1;
    property int itemHeight: 35;
    property int headerHeight: 35;

    property bool hasFilter: false;
    property bool hasSort: false;

    property bool showHeaders: true;

    property TreeItemModel headers; //: elementsList.model;

    property alias delegate: elementsList.delegate;
    property alias elements: elementsList.model;
    property alias elementsList: elementsList;

    property alias headerDelegate: headersList.delegate;
    property real headerElementWidth: (headersList.width - iconFilter.width)/headersList.count;
    property alias headerElementHeight: headersList.height;

    property alias backgroundElementsColor: elementsBg.color;

    property int radius: 7;

    signal selectItem(string idSelected, string name);
    signal rightButtonMouseClicked(int mouseX, int mouseY);
    signal doubleClicked(int mouseX, int mouseY);
    signal setActiveFocusFromTable();
    signal headerClicked(string headerId);
    signal textFilterChanged(string id, int index, string text);
    signal filterClicked();

    onSelectedIndexChanged: {
        if (selectedIndex > -1){
            elementsList.forceActiveFocus();
        }
    }

    function getSelectedId(){
        console.log("getSelectedId");
        if (tableContainer.selectedIndex > -1){
            let item = elementsList.itemAtIndex(tableContainer.selectedIndex)
            return item.getSelectedId();
        }

        return "";
    }

    function getSelectedName(){
        if (tableContainer.selectedIndex > -1){
            let item = elementsList.itemAtIndex(tableContainer.selectedIndex)
            return item.getSelectedName();
        }

        return "";
    }

    function getSelectedItemData(){
        if (tableContainer.selectedIndex > -1){
            let item = elementsList.itemAtIndex(tableContainer.selectedIndex)
            return item.getItemData();
        }

        return null;
    }

    MouseArea {
        id: maTable;

        anchors.fill: parent;

        onClicked: {
            console.log("AuxTable MouseArea onClicked");
            tableContainer.selectedIndex = -1;
        }
    }

    Item {
        id: headersPanel;

        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top: parent.top;

        height: visible ? tableContainer.headerHeight: 0;

        visible: headersList.count > 0 && showHeaders;

        clip: true;

        ListView {
            id: headersList;

            anchors.fill: parent;

            clip: true;
            orientation: ListView.Horizontal;
            spacing: 0;
            model: tableContainer.headers;
            boundsBehavior: Flickable.StopAtBounds;

            delegate: Rectangle{
                id: deleg;

                width: (headersList.width - iconFilter.width)/headersList.count;
                height: headersList.height;

                color: Style.baseColor;

                Text {
                    id: name;

                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.left: parent.left;
                    anchors.right: iconSort.left;
                    anchors.leftMargin: 8;

                    font.pixelSize: Style.fontSize_common * deleg.scale;
                    font.family: Style.fontFamilyBold;
                    font.bold: true;
                    color: Style.textColor;
                    text: model.Name;
                    elide: Text.ElideRight;
                }

                Image {
                    id: iconSort;

                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.right: parent.right;
                    anchors.rightMargin: 8;

                    height: 10;
                    width: 10;

                    visible: headersList.currentIndex === model.index && tableContainer.hasSort;

                    source: "../../../" + "Icons/" + Style.theme + "/" + "Down" + "_On_Normal.svg";
                    sourceSize.width: width;
                    sourceSize.height: height;
                }

                MouseArea {
                    id: headerMa;

                    anchors.fill: parent;

                    visible: tableContainer.hasSort;

                    onReleased: {
                        deleg.scale = 1;
                    }

                    onPressed: {
                        deleg.scale = 0.985;
                    }

                    onClicked: {
                        headersList.currentIndex = model.index;
                        tableContainer.headerClicked(model.Id);
                    }
                }
            }
        }//Headers ListView

        Rectangle{
            id: bottomLine;

            anchors.left: parent.left;
            anchors.right: parent.right;
            anchors.bottom: parent.bottom;

            height: 1;

            //TODO -> Style
            color: "lightgray";
        }
    }//headers

    AuxButton {
        id: iconFilter;

        anchors.verticalCenter: headersPanel.verticalCenter;
        anchors.right: headersPanel.right;
        anchors.rightMargin: 5;

        visible: tableContainer.hasFilter && showHeaders;

        width: tableContainer.hasFilter ? 20 : 0;
        height: width;

        iconSource: "../../../" + "Icons/" + Style.theme + "/Filter_On_Normal.svg";

        onClicked: {
            console.log("AuxButton iconFilter onClicked");
            filterClicked();
        }
    }

    Rectangle {
        id: elementsBg;

        anchors.fill: elementsList;
        color: Style.baseColor;
    }

    Component {
        id: elementDelegateComp;

        TableDelegate {
            height: tableContainer.itemHeight;
            width: elementsList.width;

            selected: tableContainer.selectedIndex === model.index;

            onClicked: {
                tableContainer.selectedIndex = model.index;
            }

            onRightButtonMouseClicked: {
                console.log("AuxTable onRightButtonMouseClicked", mX, mY);
                var point = mapToItem(thumbnailDecoratorContainer, mX, mY);
                tableContainer.rightButtonMouseClicked(point.x, point.y);
            }

            onDoubleClicked: {
                console.log("onDoubleClicked", model["Id"], model["Name"])
                var point = mapToItem(thumbnailDecoratorContainer, mX, mY);
                tableContainer.doubleClicked(point.x, point.y)
                tableContainer.selectItem(model.Id, model.Name);

            }
        }
    }

    ListView {
        id: elementsList;

        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top: headersPanel.bottom;
        anchors.bottom: parent.bottom;

        boundsBehavior: Flickable.StopAtBounds;

        clip: true;

        Keys.onUpPressed: {
            if (tableContainer.selectedIndex > 0){
                tableContainer.selectedIndex--;
            }
            else{
                tableContainer.selectedIndex = elementsList.count - 1;
            }
        }

        Keys.onDownPressed: {
            if (tableContainer.selectedIndex < elementsList.count - 1){
                tableContainer.selectedIndex++;
            }
            else{
                tableContainer.selectedIndex = 0;
            }
        }

        delegate: elementDelegateComp;

//        delegate: TableDelegate {
//            id: tableDelegate;

//            height: tableContainer.itemHeight;
//            width: elementsList.width;

//            selected: tableContainer.selectedIndex === model.index;
//            radius: tableContainer.radius;

//            Component.onCompleted: {
//                for(var i = 0; i < tableContainer.headers.GetItemsCount(); i++){
//                    tableDelegate.addToArray(model[tableContainer.headers.GetData("Id", i)]);
//                }
//            }

//            onClicked: {
//                tableContainer.selectedIndex = model.index;
//            }

//            onRightButtonMouseClicked: {
//                console.log("AuxTable onRightButtonMouseClicked", mX, mY);
//                var point = tableDelegate.mapToItem(thumbnailDecoratorContainer, mX, mY);
//                tableContainer.rightButtonMouseClicked(point.x, point.y);
//            }

//            onDoubleClicked: {
//                console.log("onDoubleClicked", model["Id"], model["Name"])
//                tableContainer.selectItem(model.Id, model.Name);
//            }
//        }//Elements delegate
    }//Elements ListView
}
