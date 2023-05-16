import QtQuick 2.12
import Acf 1.0

TableViewItemDelegateBase {
    id: treeDelegateBase;

    height: !treeDelegateBase.root ? 0 : treeDelegateBase.isOpened ? treeDelegateBase.footerItem.height + treeDelegateBase.root.rowItemHeight: treeDelegateBase.root.rowItemHeight;

    property bool isOpened: true;

    property bool hasChild: false;

    property bool tristate: true; //prefixRowItem.visible;

    property var parentDelegate: null;
    property var childrenDelegates: [];

    level: treeDelegateBase.parentDelegate == null ? 0 : treeDelegateBase.parentDelegate.level + 1;

    signal parentCheckStateChanged(var data);
    signal childrenCheckStateChanged(var data);

    onSelectedChanged: {
        if(treeDelegateBase.rowBodyItem) {
            treeDelegateBase.rowBodyItem.forceActiveFocus();
        }
    }

    prefixRowDelegate: Component {
        Row {
            id: prefixRow;

            height: treeDelegateBase.root ? treeDelegateBase.root.rowItemHeight : 0;

            spacing: 10;

            Item {
                width: 15;
                height: prefixRow.height;

                AuxButton {
                    id: arrowButton;

                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.horizontalCenter: parent.horizontalCenter;

                    width: 15;
                    height: width;

                    visible: treeDelegateBase.hasChild;

                    iconSource: treeDelegateBase.isOpened ? "../../../" + "Icons/" + Style.theme + "/" + "Down" + "_On_Normal.svg" :
                                                          "../../../" + "Icons/" + Style.theme + "/" + "Right" + "_On_Normal.svg";

                    onClicked: {
                        treeDelegateBase.isOpened = !treeDelegateBase.isOpened;
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

                    checkState: treeDelegateBase.checkState;
                    isActive: treeDelegateBase.isActive;
                    visible: treeDelegateBase.root ? treeDelegateBase.root.tristate && treeDelegateBase.isCheckable : false;

                    onClicked: {
                        if (treeDelegateBase.checkState == Qt.PartiallyChecked){
                            treeDelegateBase.checkState = Qt.Checked;
                        }
                        else{
                            treeDelegateBase.checkState = Qt.Checked - treeDelegateBase.checkState;
                        }

//                        if (model.CheckState == Qt.PartiallyChecked){
//                            model.CheckState = Qt.Checked;
//                        }
//                        else{
//                            model.CheckState = Qt.Checked - model.CheckState;
//                        }

//                        if (treeDelegateBase.parentDelegate){
//                            treeDelegateBase.parentDelegate.childrenCheckStateChanged(treeDelegateBase.itemData);
//                        }

//                        for (let i = 0; i < treeDelegateBase.childrenDelegates.length; i++){
//                            treeDelegateBase.childrenDelegates[i].parentCheckStateChanged(treeDelegateBase.itemData);
//                        }

//                        treeDelegateBase.root.rowModelDataChanged(treeDelegateBase, "CheckState");
                    }
                }
            }
        }
    }

    footerDelegate: Component { Column {
            id: childrenColumn;

            visible: treeDelegateBase.isOpened;

            Repeater {
                id: childModelRepeater;

                delegate: treeDelegateBase.root ? treeDelegateBase.root.rowDelegate : null;

                onItemAdded: {
                    item.parentDelegate = treeDelegateBase;
                    treeDelegateBase.childrenDelegates.push(item);

                    item.modelIndex.parentIndex = treeDelegateBase.modelIndex;
                    treeDelegateBase.modelIndex.childModel.push(item.modelIndex);

//                    // Связываем modelIndex соседних элементов
//                    if (index > 0){
//                        let prevItem = childModelRepeater.itemAt(index - 1);
//                        item.modelIndex.prevIndex = prevItem.modelIndex;
//                        prevItem.modelIndex.nextIndex =item.modelIndex;
//                    }
                }

                onItemRemoved: {
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
                console.log("onChildModelChanged", childModel);
                if (childrenColumn.childModel){
                    if (childrenColumn.childModel.GetItemsCount() > 0){
                            treeDelegateBase.hasChild = true;
                    }

                    childModelRepeater.model = childrenColumn.childModel;
                }
                else{
                    treeDelegateBase.hasChild = false;
                }
            }
        }
    }
}
