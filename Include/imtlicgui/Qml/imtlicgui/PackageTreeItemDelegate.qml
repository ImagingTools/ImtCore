import QtQuick 2.0
import Acf 1.0
import imtgui  1.0

TreeItemDelegate {
    id: treeItemDelegate;

    itemDelegate: Rectangle {
        id: body;

        anchors.top: treeItemDelegate.top;
        anchors.right: treeItemDelegate.right;

        width: parent.width;
        height: parent.height;

        visible: itemData.Visible;

        color: model.Selected ? Style.selectedColor : "transparent";

        MouseArea {
            anchors.fill: parent;

            onClicked: {
                treeItemDelegate.clicked(model, model.index);
            }

            onDoubleClicked: {
                treeItemDelegate.doubleClicked();
            }
        }

        Image {
            id: iconArrow;

            anchors.left: parent.left;
            anchors.leftMargin: 10;
            anchors.verticalCenter: parent.verticalCenter;

            width: 10;
            height: 10;

            visible: childrenCount > 0;

            source: model.Opened ? "../../../" + "Icons/" + Style.theme + "/" + "Down" + "_On_Normal.svg" :
                                     "../../../" + "Icons/" + Style.theme + "/" + "Right" + "_On_Normal.svg";

            sourceSize.height: height;
            sourceSize.width: width;

            MouseArea {
                id: mouseArea;

                anchors.fill: parent;

                onClicked: {
                    console.log("iconArrow onClicked");
                    model.Opened = !model.Opened;
                }
            }
        }

        CheckBox {
            id: checkBox;

            anchors.left: iconArrow.right;
            anchors.leftMargin: 5;
            anchors.verticalCenter: parent.verticalCenter;

            checkState: model.State;

            visible: model.Level >= 1;

            onClicked: {
                if (!model.Active){
                    return;
                }
                console.log("onClicked", itemData.Id);

                if (model.State == Qt.PartiallyChecked){
                    model.State = Qt.Checked;
                }
                else{
                    model.State = Qt.Checked - model.State;
                }

                treeView._itemStateChanged(itemData, model.State, true);
                treeView._itemStateChanged(itemData, model.State, false);
            }
        }

        Text {
            id: title;

            anchors.left: checkBox.visible ? checkBox.right : iconArrow.right;
            anchors.leftMargin: 10;
            anchors.verticalCenter: parent.verticalCenter;

            text: model.Name;
            color: model.Active ? Style.textColor : Style.disabledInActiveTextColor;

            font.pixelSize: Style.fontSize_common;
            font.family: Style.fontFamily;
        }
    }
}

//Item {
//    id: treeItemDelegate;

//    height: childrenColumn.visible ? childrenColumn.height + viewDelegateLoader.height : viewDelegateLoader.height;

//    property int itemHeight: 30;

//    property TreeItemModel childModel: model.ChildModel ? model.ChildModel : null;

//    property int state: model.State;
//    property string itemId: model.Id;

//    property bool itemVisible: model.Visible;
//    property bool itemActive: model.Active;

//    property var itemData: model;

//    property alias itemDelegate: viewDelegateLoader.sourceComponent;

//    signal doubleClicked;
//    signal clicked(var itemData, int index);

//    onChildModelChanged: {
//        childModelRepeater.model = model.ChildModel;
//    }

//    Loader {
//        id: viewDelegateLoader;

//        anchors.top: treeItemDelegate.top;

//        width: parent.width;
//        height: model.Visible ? treeItemDelegate.itemHeight : 0;

//        sourceComponent: Rectangle {
//            id: body;

//            anchors.top: treeItemDelegate.top;
//            anchors.right: treeItemDelegate.right;

//            width: parent.width;
//            height: parent.height;

//            visible: itemData.Visible;

//            color: model.Selected ? Style.selectedColor : "transparent";

//            MouseArea {
//                anchors.fill: parent;

//                onClicked: {
//                    treeItemDelegate.clicked(model, model.index);
//                }

//                onDoubleClicked: {
//                    treeItemDelegate.doubleClicked();
//                }
//            }

//            Image {
//                id: iconArrow;

//                anchors.left: parent.left;
//                anchors.leftMargin: 10;
//                anchors.verticalCenter: parent.verticalCenter;

//                width: 10;
//                height: 10;

//                visible: childModelRepeater.count > 0;

//                source: model.Opened ? "../../../" + "Icons/" + Style.theme + "/" + "Down" + "_On_Normal.svg" :
//                                         "../../../" + "Icons/" + Style.theme + "/" + "Right" + "_On_Normal.svg";

//                sourceSize.height: height;
//                sourceSize.width: width;

//                MouseArea {
//                    id: mouseArea;

//                    anchors.fill: parent;

//                    onClicked: {
//                        console.log("iconArrow onClicked");
//                        model.Opened = !model.Opened;
//                    }
//                }
//            }

//            CheckBox {
//                id: checkBox;

//                anchors.left: iconArrow.right;
//                anchors.leftMargin: 5;
//                anchors.verticalCenter: parent.verticalCenter;

//                checkState: model.State;

//                visible: model.Level >= 1;

//                onClicked: {
//                    if (!model.Active){
//                        return;
//                    }

//                    if (model.State == Qt.PartiallyChecked){
//                        model.State = Qt.Checked;
//                    }
//                    else{
//                        model.State = Qt.Checked - model.State;
//                    }
//                }
//            }

//            Text {
//                id: title;

//                anchors.left: checkBox.visible ? checkBox.right : iconArrow.right;
//                anchors.leftMargin: 10;
//                anchors.verticalCenter: parent.verticalCenter;

//                text: model.Name;
//                color: model.Active ? Style.textColor : Style.disabledInActiveTextColor;

//                font.pixelSize: Style.fontSize_common;
//                font.family: Style.fontFamily;
//            }
//        }
//    }

//    Column {
//        id: childrenColumn;

//        anchors.top: viewDelegateLoader.bottom;
//        anchors.right: parent.right;

//        width: treeItemDelegate.width - 20;

//        visible: model.Opened && model.Visible;

//        Repeater {
//            id: childModelRepeater;

//            delegate: treeView.itemDelegate;

//            onItemAdded: {
//                console.log('Repeater onItemAdded', itemData.Level);

//                if (itemData.Level >= 1){
//                    item.itemData.Parent = itemData;
//                    if (!itemData.Children){
//                        itemData.Children = [item.itemData]
//                    }
//                    else{
//                        itemData.Children.push(item.itemData);
//                    }
//                }
//            }
//        }
//    }
//}
