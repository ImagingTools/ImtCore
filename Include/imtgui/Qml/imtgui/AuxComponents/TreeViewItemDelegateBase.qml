import QtQuick 2.12
import Acf 1.0

TableViewItemDelegateBase {
    id: treeDelegateBase;

    height: !treeDelegateBase.root ? 0 : treeDelegateBase.isOpen ? treeDelegateBase.footerItem.height + treeDelegateBase.root.rowItemHeight: treeDelegateBase.root.rowItemHeight;

    property bool isOpen: model.IsOpen;

    property int checkState: model.CheckState;

    property bool tristate: true; //prefixRowItem.visible;

    property var parentDelegate: null;
    property var childrenDelegates: [];

    level: treeDelegateBase.parentDelegate == null ? 0 : treeDelegateBase.parentDelegate.level + 1;

    onLevelChanged: {
        console.log("onLevelChanged", model.Id, level);
    }

    onHeightChanged: {
        console.log("onHeightChanged", model.Id, height);
    }

    signal parentCheckStateChanged(var data);
    signal childrenCheckStateChanged(var data);

    function clear(){
        model.CheckState = Qt.Unchecked;
        model.Active = true;
        model.Visible = true;
    }

    function setState(state){
        console.log( model.Id, "setState", state);
        model.CheckState = state;
    }

    function setActive(active){
        console.log( model.Id, "setActive", active);
        model.Active = active;
    }

    function setVisible(visible){
        model.Visible = visible;
    }

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

    prefixRowDelegate: Component {
        Row {
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
    //                    treeDelegateBase.isOpen = !treeDelegateBase.isOpen;
                        console.log("TreeView isOpen", model)

                        model.IsOpen = !model.IsOpen;
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
    }

    footerDelegate: Component { Column {
            id: childrenColumn;

            visible: treeDelegateBase.isOpen;

            Component.onCompleted: {
                console.log("Column onCompleted");
            }

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

                    // Связываем modelIndex соседних элементов
                    if (index > 0){
                        let prevItem = childModelRepeater.itemAt(index - 1);
                        item.modelIndex.prevIndex = prevItem.modelIndex;
                        prevItem.modelIndex.nextIndex =item.modelIndex;
                    }
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

            property var childModel: treeDelegateBase.itemData.ChildModel ? model.ChildModel: 0;
            onChildModelChanged: {
                console.log("TreeViewItemDelegateBase onChildModelChanged", childModel);
                if (childrenColumn.childModel){
                    childModelRepeater.model = childrenColumn.childModel;
                }
            }
        }
    }
}
