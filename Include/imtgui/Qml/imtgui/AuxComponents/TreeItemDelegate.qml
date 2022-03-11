import QtQuick 2.0
import Acf 1.0

Item {
    id: treeItemDelegate;

    width: 100;
//    height: mainRect.height + childrenColumn.height;

//    height: childrenColumn.visible ? mainRect.height + childrenColumn.height: mainRect.height;
    height: childrenColumn.visible ?
                mainRect.height + (childrenColumn.height ?
                                       childrenColumn.height : (model.childItemModel ?
                                                                    model.childItemModel._rows.length * 30 : 0)) :
                mainRect.height;

    property bool isOpened: true;

    property TreeItemModel childItemModel;

    property Item listViewItem;

    signal checkBoxState(int state, string packageId, string featureId);

    onCheckBoxState: {
        console.log("TreeItemDelegate onCheckBoxState", state, packageId, featureId);
    }

//    onHeightChanged: {
//        console.log("TreeItemDelegate onHeightChanged", treeItemDelegate.height);
//    }

    Component.onCompleted: {
        if (model.childItemModel)
        {
           // console.log("Model childCount ", model.childItemModel.GetItemsCount());
            treeItemRepeater.model = model.childItemModel;
//            childrenColumn.model = model.childItemModel;
        }

    }

//    onIsOpenedChanged: {
//        console.log("TreeItemDelegate onIsOpenedChanged");
//        if (treeItemDelegate.isOpened){
//            console.log(" = model.childItemModel");
//            treeItemRepeater.model = model.childItemModel;
//        } else {
//            console.log(" = 0");
//            treeItemRepeater.model = 0;
//        }
//    }

//    onCheckBoxStateChanged: {
//        //treeItemRepeater.model = treeItemDelegate.childItemModel;
//    }


    Rectangle {
        id: mainRect;

        anchors.top: parent.top;
        anchors.right: parent.right;

        width: parent.width - model.level * 20;
        height: model.visible ? 30 : 0;

        color: Style.baseColor;

        visible: model.visible === 1;

//        MouseArea {
//            anchors.fill: parent;

//            onClicked: {
//                selectionBackGround.visible = true;
//            }
//        }

//        Rectangle {
//            id: selectionBackGround;
//            anchors.fill: parent;
//            color: "#4682B4";
//            opacity: 0.2;
//            radius: 2;
//            visible: false;
//        }

        Image {
            id: iconArrow;

            anchors.left: parent.left;
            anchors.leftMargin: 10;
            anchors.verticalCenter: parent.verticalCenter;

            width: 10;
            height: 10;

            visible: model.level === 0;
            source: treeItemDelegate.isOpened ? "../../../" + "Icons/" + Style.theme + "/" + "Down" + "_On_Normal.svg" :
                                                "../../../" + "Icons/" + Style.theme + "/" + "Right" + "_On_Normal.svg";

            sourceSize.height: height;
            sourceSize.width: width;

            MouseArea {
                id: mouseArea;

                anchors.fill: parent;

                onClicked: {
                    console.log("TreeItemDelegate Image onClicked");
                    treeItemDelegate.isOpened = !treeItemDelegate.isOpened;
//                    isOpened = !isOpened;
                    console.log("treeItemDelegate", treeItemDelegate);
//                    console.log("TreeItemDelegate onHeightChanged", treeItemDelegate.height);
//                    console.log("treeItemDelegate.isOpened", treeItemDelegate.isOpened);
//                    console.log("Model id", model.Id);
//                    console.log("Model packageId", model.packageId);
                }
            }
        }

        CheckBox {
             id: checkBox;

             anchors.rightMargin: 10;
             anchors.right: titleModel.left;
             anchors.verticalCenter: parent.verticalCenter;

             checkState: model.stateChecked;

             visible: model.level === 1;

             onCheckStateChanged: {
                // console.log("TreeItemDelegate CheckBox onCheckStateChanged", checkBox.checkState, model.packageId, model.Id);

//                 treeItemDelegate.checkBoxState(checkBox.checkState, model.packageId, model.Id);
                // listViewItem.changeCheckBoxState(checkBox.checkState, model.packageId, model.Id);
             }

             MouseArea {
                 anchors.fill: parent;

                 visible: model.isActive === 1;

                 onClicked: {
                     console.log("TreeItemDelegate CheckBox MouseArea onClicked", checkBox.checkState);
                     if (checkBox.checkState == 2) {
                         checkBox.checkState = 0
//                         model.stateChecked = 0;
                     }
                     else {
                         checkBox.checkState = 2
//                         model.stateChecked = 2;
                     }

//                     treeItemDelegate.checkBoxState(checkBox.checkState, model.packageId, model.Id);
//                     treeItemDelegate.listViewItem.changeCheckBoxState(checkBox.checkState, model.packageId, model.Id);
                     mainTreeView.changeCheckBoxState(checkBox.checkState, model.packageId, model.Id);
                 }
             }
        }

        Text {
            id: titleModel;

            anchors.left: iconArrow.right;
            anchors.leftMargin: 10;
            anchors.verticalCenter: parent.verticalCenter;

            text: model.Name;
            color: model.isActive === 1 ? Style.textColor : Style.disabledInActiveTextColor;
            font.pixelSize: Style.fontSize_common;
            font.family: Style.fontFamily;
        }
    }

//    ListView {
//        id: childrenColumn;

//        anchors.top: mainRect.bottom;

//        height: childrenColumn.count * 30;
//        width: treeItemDelegate.width;

//        visible: treeItemDelegate.isOpened;


//        delegate: Loader {
//            id: loader;

//            source: "TreeItemDelegate.qml";

//            onItemChanged: {
//                if (loader.item) {
//                    console.log("TreeItemDelegate child load");
//                    console.log("treeItemDelegate.listViewItem", treeItemDelegate.listViewItem);
//                    loader.height = loader.item.height;
//                    //childrenColumn.height += item.height;
//                    loader.item.listViewItem = treeItemDelegate.listViewItem;
//                    console.log("loader.item.listViewItem", loader.item.listViewItem);
//                }
//            }
//        }
//    }

    Column {
        id: childrenColumn;

        anchors.top: mainRect.bottom;

        width: treeItemDelegate.width;

        visible: treeItemDelegate.isOpened;

        Repeater {
             id: treeItemRepeater;
//             property bool isOpened: treeItemDelegate.isOpened;

//             onIsOpenedChanged: {
//                 console.log("TreeItemDelegate onIsOpenedChanged");
//                 if (treeItemDelegate.isOpened){
//                     treeItemRepeater.model = model.childItemModel;
//                 } else {
//                     treeItemRepeater.model = 0;
//                 }
//             }

             delegate: Loader {
                 id: loader;

                 source: "TreeItemDelegate.qml";

                 onItemChanged: {
                     if (loader.item) {
                         console.log("TreeItemDelegate child load");
                         console.log("treeItemDelegate.listViewItem", treeItemDelegate.listViewItem);
                         loader.height = loader.item.height;
                         //childrenColumn.height += item.height;
                         loader.item.listViewItem = treeItemDelegate.listViewItem;
                         console.log("loader.item.listViewItem", loader.item.listViewItem);
                     }
                 }
             }
//             onHeightChanged: {
//                 console.log('============================')
//                 console.log('treeItemRepeater height: ', height)
//                 console.log('============================')
//             }

//             onVisibleChanged: {
//                console.log('Repeater onVisibleChanged', treeItemRepeater.visible);
//             }
       }

//        onHeightChanged: {
//            parent.height = height + mainRect.height;
//        }
    }
}
