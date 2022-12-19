import QtQuick 2.12
import Acf 1.0

TableViewItemDelegateBase {
    id: treeDelegateBase;

    height: isOpen ? footerItem.height + root.rowItemHeight: root.rowItemHeight;

    property bool isOpen: true;

    property int checkState: model.CheckState;

    property bool tristate: true; //prefixRowItem.visible;

    property var parentDelegate: null;
    property var childrenDelegates: []

    signal parentCheckStateChanged(var data);
    signal childrenCheckStateChanged(var data);

    onSelectedChanged: {
        console.log("packageTreeItemDelegate onSelectedChanged", selected);
        if (selected){
            root.selectedIndex = modelIndex;
        }
        else{
            root.selectedIndex = null;
        }

        updateSelection();

        if(rowBodyItem) {
            rowBodyItem.forceActiveFocus();
        }
    }

    prefixRowDelegate: Row {
        id: prefixRow;

        height: root.rowItemHeight;

        spacing: 10;

        function click(){
            checkBox.clicked();
        }

        Item {
            width: 15;
            height: prefixRow.height;

            AuxButton {
                id: arrowButton;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.horizontalCenter: parent.horizontalCenter;

                width: 15;
                height: width;

                visible: model.ChildModel ? model.ChildModel.count > 0 : false;

                iconSource: treeDelegateBase.isOpen ? "../../../" + "Icons/" + Style.theme + "/" + "Down" + "_On_Normal.svg" :
                                     "../../../" + "Icons/" + Style.theme + "/" + "Right" + "_On_Normal.svg";

                onClicked: {
                    treeDelegateBase.isOpen = !treeDelegateBase.isOpen;
                }
            }
        }

        Item {
            width: checkBox.visible ? 15 : 0;
            height: prefixRow.height;

            CheckBox {
                id: checkBox;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.horizontalCenter: parent.horizontalCenter;

                checkState: model.CheckState;

                isActive: model.Active;

                visible: root.tristate && model.CheckBoxVisible;

                onClicked: {
                    if (model.CheckState == Qt.PartiallyChecked){
                        model.CheckState = Qt.Checked;
                    }
                    else{
                        model.CheckState = Qt.Checked - model.CheckState;
                    }

                    if (treeDelegateBase.parentDelegate){
                        treeDelegateBase.parentDelegate.childrenCheckStateChanged(treeDelegateBase.itemData);
                    }

                    for (let i = 0; i < treeDelegateBase.childrenDelegates.length; i++){
                        treeDelegateBase.childrenDelegates[i].parentCheckStateChanged(treeDelegateBase.itemData);
                    }

                    root.rowModelDataChanged(treeDelegateBase, "CheckState");
                }
            }
        }
    }

    footerDelegate: Component { Column {
        id: childrenColumn;

        visible: treeDelegateBase.isOpen;

        Repeater {
            id: childModelRepeater;

            delegate: root.rowDelegate;

            onItemAdded: {
                console.log("TableViewItemDelegate onItemAdded", item.itemData.Id);
                item.parentDelegate = treeDelegateBase;
                treeDelegateBase.childrenDelegates.push(item)

                console.log("TableViewItemDelegate onItemAdded");
                item.modelIndex.parentIndex = modelIndex;
                modelIndex.children.push(item.modelIndex);
            }

            onItemRemoved: {
                console.log("TreeViewItemDelegateBase onItemRemoved", item.itemData.Id);
                let index = treeDelegateBase.childrenDelegates.indexOf(item);
                if (index > -1) {
                    treeDelegateBase.childrenDelegates.splice(index, 1);
                }

                index = modelIndex.children.indexOf(item.modelIndex);
                if (index > -1) {
                    modelIndex.children.splice(index, 1);
                }
            }
        }

        property ListModel childModel: model.ChildModel ? model.ChildModel: null;
//        property TreeItemModel childModel: model.ChildModel ? model.ChildModel: null;

        onChildModelChanged: {
            console.log("TreeViewItemDelegateBase onChildModelChanged", childModel);
            if (childModel){
                childModelRepeater.model = childModel;
            }
        }
    }
    }
}
