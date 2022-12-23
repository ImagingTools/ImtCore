import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

Item {
    id: tableItemDelegate;

    height: childrenColumn.visible ? childrenColumn.height + rowBody.height : rowBody.height;

    property int itemHeight: 30;

    signal clicked(string id, int index, var model);
    signal doubleClicked();

    signal buttonClicked();

    property int headersCount: 2;

    property TreeItemModel childModel: model.ChildModel ? model.ChildModel : null;

    property ModelIndex modelIndex: ModelIndex {};

    Component.onCompleted: {
        console.log("tableItemDelegate onCompleted", model.Id, model.Name);
        modelIndex.itemData = model;
        modelIndex.index = model.index;
    }

    onChildModelChanged: {
        console.log("tableItemDelegate onChildModelChanged", model.ChildModel);

        if (model.ChildModel){
            childModelRepeater.model = model.ChildModel;
        }
    }

    Rectangle {
        id: rowBody;

        width: parent.width;
        height: tableItemDelegate.itemHeight;

        color: modelIndex.equal(tableTreeView.selectedIndex) ? Style.selectedColor : "transparent";

        MouseArea {
            id: rowMouseArea;

            anchors.fill: parent;

            onClicked: {
                tableItemDelegate.clicked(model.Id, model.index, model);
            }

            onDoubleClicked: {
                tableItemDelegate.doubleClicked();
            }
        }

        Item {
            id: nameRect;

            anchors.right: idRect.left;

            width: parent.width / headersCount - model.Level * 10;
            height: parent.height;

            AuxButton {
                id: arrowButton;

                anchors.left: parent.left;
//                anchors.leftMargin: 10;
                anchors.verticalCenter: parent.verticalCenter;

                width: 15;
                height: width;

                iconSource: model.Opened ? "../../../" + "Icons/" + Style.theme + "/" + "Down" + "_On_Normal.svg" :
                                       "../../../" + "Icons/" + Style.theme + "/" + "Right" + "_On_Normal.svg";

                visible: childModelRepeater.count > 0;

                onClicked: {
                    model.Opened = !model.Opened;
                }
            }

            Text {
                id: titleName;

                anchors.left: arrowButton.right;
                anchors.leftMargin: 10;
                anchors.verticalCenter: parent.verticalCenter;

                width: parent.width - 10;

                text: model.Name ? model.Name : "";
//                color: model.Active ? Style.textColor : Style.disabledInActiveTextColor;
                color: Style.textColor;

                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamily;

                elide: Text.ElideRight;
            }
        }

        Item {
            id: idRect;

            anchors.right: parent.right;

            width: parent.width / headersCount;

            height: parent.height;

            Text {
                id: titleId;

                anchors.left: parent.left;
                anchors.leftMargin: 10;
                anchors.verticalCenter: parent.verticalCenter;

                width: parent.width - 10;

                text: model.Id ? model.Id : "";
                color: Style.textColor;

                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamily;

                elide: Text.ElideRight;
            }
        }

        Item {
            id: buttonRect;

            anchors.right: idRect.right;
            anchors.rightMargin: 10;

            width: 18;
            height: parent.height;

            visible: model.Level == 1;

            AuxButton {
                anchors.verticalCenter: buttonRect.verticalCenter;
                anchors.horizontalCenter: buttonRect.horizontalCenter;

                width: 18;
                height: width;

                iconSource: "../../../Icons/" + Style.theme + "/Add_On_Normal.svg";

                onClicked: {
                    tableItemDelegate.buttonClicked();
                }
            }
        }
    }

    Column {
        id: childrenColumn;

        anchors.top: rowBody.bottom;
        anchors.right: parent.right;

        visible: model.Opened;

        onHeightChanged: {
            console.log("childrenColumn onHeightChanged", height)
            console.log("model.Id", model.Id)
        }

        Repeater {
            id: childModelRepeater;

            delegate: tableTreeView.delegateComp;

            onItemAdded: {

                console.log("onItemAdded", item);

                item.modelIndex.parentIndex = modelIndex;
                modelIndex.childModel.push(item.modelIndex);
            }
        }
    }
}
