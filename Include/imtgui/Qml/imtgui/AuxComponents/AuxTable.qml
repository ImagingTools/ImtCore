import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0
//import '../UxAdvancedComponents' as AUX


Item {
    id: tableContainer;

    property int selectedIndex: -1;
    property int itemHeight: 35;

    property bool hasFilter: false;
    property bool hasSort: false;

    property TreeItemModel headers; //: elementsList.model;

    property alias delegate: elementsList.delegate;
    property alias elements: elementsList.model;

    property int radius: 7;

    signal selectItem(string idSelected, string name);
    signal rightButtonMouseClicked(int mouseX, int mouseY);
    signal setActiveFocusFromTable();
    signal headerClicked(string headerId, string sortOrder);
    signal textFilterChanged(string id, int index, string text);
    signal filterClicked();

    function getSelectedId(){
        if (tableContainer.selectedIndex > -1){
             return tableContainer.elements.GetData("Id", tableContainer.selectedIndex);
        }

        return null;
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

    Item {
        id: headersPanel;

        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top: parent.top;

        height: 35;

        clip: true;

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

                width: (headersList.width - iconFilter.width)/headersList.count;
                height: headersList.height;

                color: Style.baseColor;

                property string sortOrder: "ASC";

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
                        console.log("onReleased");
                        deleg.scale = 1;
                    }

                    onPressed: {
                        deleg.scale = 0.985;
                    }

                    onClicked: {
                        headersList.currentIndex = model.index;
                        if (deleg.sortOrder == 'ASC'){
                            deleg.sortOrder = 'DESC'
                            iconSort.source = "../../../" + "Icons/" + Style.theme + "/" + "Up" + "_On_Normal.svg";
                        }
                        else if (deleg.sortOrder == 'DESC'){
                            deleg.sortOrder = 'ASC'
                            iconSort.source = "../../../" + "Icons/" + Style.theme + "/" + "Down" + "_On_Normal.svg";
                        }
                        tableContainer.headerClicked(model.Id, deleg.sortOrder);
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

        visible: tableContainer.hasFilter;

        width: tableContainer.hasFilter ? 20 : 0;
        height: width;

//        highlighted: filterMenu.visible;

        iconSource: "../../../" + "Icons/" + Style.theme + "/Filter_On_Normal.svg";

        onClicked: {
            console.log("AuxButton iconFilter onClicked");
            filterClicked();
        }
    }

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

            height: tableContainer.itemHeight;
            width: elementsList.width;

            selected: tableContainer.selectedIndex === model.index;
            radius: tableContainer.radius;

            Component.onCompleted: {
                for(var i = 0; i < tableContainer.headers.GetItemsCount(); i++){
                    tableDelegate.addToArray(model[tableContainer.headers.GetData("Id",i)]);
                }
            }

            onClicked: {
                elementsList.selectedId = model.Id;
                elementsList.selectedName = model.Name;
                tableContainer.selectedIndex = model.index;
            }

            onRightButtonMouseClicked: {
                console.log("AuxTable onRightButtonMouseClicked", mX, mY);
                var point = tableDelegate.mapToItem(thumbnailDecoratorContainer, mX, mY);
                tableContainer.rightButtonMouseClicked(point.x, point.y);
            }

            onDoubleClicked: {
                console.log("onDoubleClicked", model["Id"], model["Name"])
                tableContainer.selectItem(model.Id, model.Name);
            }
        }//Elements delegate
    }//Elements ListView
}
