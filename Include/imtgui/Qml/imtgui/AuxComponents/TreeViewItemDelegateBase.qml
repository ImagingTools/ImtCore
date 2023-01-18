import QtQuick 2.12
import Acf 1.0

TableViewItemDelegateBase {
    id: treeDelegateBase;

    height: isOpen ? footerItem.height + treeDelegateBase.root.rowItemHeight: treeDelegateBase.root.rowItemHeight;

    property bool isOpen: true;

    property int checkState: model.CheckState;

    property bool tristate: true; //prefixRowItem.visible;

    property var parentDelegate: null;
    property var childrenDelegates: [];

    signal parentCheckStateChanged(var data);
    signal childrenCheckStateChanged(var data);

    onSelectedChanged: {
        console.log("packageTreeItemDelegate onSelectedChanged", treeDelegateBase.selected);
        if (treeDelegateBase.selected){
            treeDelegateBase.root.selectedIndex = treeDelegateBase.modelIndex;
        }
        else{
            treeDelegateBase.root.selectedIndex = null;
        }

        treeDelegateBase.updateSelection();

        if(treeDelegateBase.rowBodyItem) {
            treeDelegateBase.rowBodyItem.forceActiveFocus();
        }
    }

    prefixRowDelegate: Row {
        id: prefixRow;

        height: treeDelegateBase.root ? treeDelegateBase.root.rowItemHeight : 0;

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

                visible: treeDelegateBase.root ? treeDelegateBase.root.tristate && model.CheckBoxVisible : false;

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

                    treeDelegateBase.root.rowModelDataChanged(treeDelegateBase, "CheckState");
                }
            }
        }
    }

    footerDelegate: Component { Column {
            id: childrenColumn;

            visible: treeDelegateBase.isOpen;

            Repeater {
                id: childModelRepeater;

                delegate: treeDelegateBase.root ? treeDelegateBase.root.rowDelegate : null;

                onItemAdded: {
                    console.log("TableViewItemDelegate onItemAdded", item.itemData.Id);
                    item.parentDelegate = treeDelegateBase;
                    treeDelegateBase.childrenDelegates.push(item);

                    console.log("TableViewItemDelegate onItemAdded");
                    item.modelIndex.parentIndex = treeDelegateBase.modelIndex;
                    treeDelegateBase.modelIndex.childModel.push(item.modelIndex);
                }

                onItemRemoved: {
                    console.log("TreeViewItemDelegateBase onItemRemoved", item.itemData.Id);
                    let index = treeDelegateBase.childrenDelegates.indexOf(item);
                    if (index > -1) {
                        treeDelegateBase.childrenDelegates.splice(index, 1);
                    }

                    index = treeDelegateBase.modelIndex.childModel.indexOf(item.modelIndex);
                    if (index > -1) {
                        treeDelegateBase.modelIndex.childModel.splice(index, 1);
                    }
                }
            }

            property ListModel childModel: model.ChildModel ? model.ChildModel: null;
            //        property TreeItemModel childModel: model.ChildModel ? model.ChildModel: null;

            onChildModelChanged: {
                console.log("TreeViewItemDelegateBase onChildModelChanged", childModel);
                if (childrenColumn.childModel){
                    childModelRepeater.model = childrenColumn.childModel;
                }
            }
        }
    }
}
