import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtcontrols 1.0

TableViewItemDelegateBase {
    id: treeDelegateBase;

    height: !treeDelegateBase.root ? 0 : treeDelegateBase.isOpened ? treeDelegateBase.footerItem.height + treeDelegateBase.root.rowItemHeight: treeDelegateBase.root.rowItemHeight;

    property bool isOpened: true;

    property bool hasChild: false;

    property bool tristate: true; //prefixRowItem.visible;

    property var rootDelegate: treeDelegateBase.level === 0 ? treeDelegateBase : parentDelegate.rootDelegate;
    property var parentDelegate: null;

    property var childrenDelegates: [];

    level: treeDelegateBase.parentDelegate == null ? 0 : treeDelegateBase.parentDelegate.level + 1;

    signal parentCheckStateChanged(var delegate);
    signal childrenCheckStateChanged(var delegate);

    property var childModel: model.ChildModel;

    onChildModelChanged: {
        console.log("Delegate onChildModelChanged", childModel);
    }

    Connections {
        id: connections;

        target: treeDelegateBase;

        function onIsOpenedChanged(){
            console.log("onIsOpenedChanged", treeDelegateBase.isOpened);
            if (treeDelegateBase.parentDelegate != null && treeDelegateBase.isOpened){
                treeDelegateBase.parentDelegate.isOpened = true;
            }
        }

        enabled: true;
    }

    onParentCheckStateChanged: {
        if (isCheckable){
            treeDelegateBase.checkState = delegate.checkState;
        }

        for (let childrenDelegate of treeDelegateBase.childrenDelegates){
            childrenDelegate.parentCheckStateChanged(treeDelegateBase);
        }
    }

    onChildrenCheckStateChanged: {
        if (isCheckable){
            let isAllChecked = treeDelegateBase.root.__checkState(treeDelegateBase.childrenDelegates, Qt.Checked);
            let isAllUnchecked = treeDelegateBase.root.__checkState(treeDelegateBase.childrenDelegates, Qt.Unchecked);

            if (isAllChecked){
                if (treeDelegateBase.checkState != Qt.Checked){
                    treeDelegateBase.checkState = Qt.Checked;
                }
            }
            else if (isAllUnchecked){
                if (treeDelegateBase.checkState!= Qt.Unchecked){
                    treeDelegateBase.checkState = Qt.Unchecked;
                }
            }
            else if (!isAllChecked && !isAllUnchecked){
                if (treeDelegateBase.checkState != Qt.PartiallyChecked){
                    treeDelegateBase.checkState = Qt.PartiallyChecked;
                }
            }
        }

        if (treeDelegateBase.parentDelegate){
            treeDelegateBase.parentDelegate.childrenCheckStateChanged(treeDelegateBase);
        }
    }

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

                    iconSource: treeDelegateBase.isOpened ? "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal) :
                                                            "../../../" + Style.getIconPath("Icons/Right", Icon.State.On, Icon.Mode.Normal);

                    onClicked: {
                        connections.enabled = false;
                        treeDelegateBase.isOpened = !treeDelegateBase.isOpened;
                        connections.enabled = true;
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
                    isActive: treeDelegateBase.isActive && !treeDelegateBase.root.readOnly;
                    visible: treeDelegateBase.root ? treeDelegateBase.root.tristate && treeDelegateBase.isCheckable : false;

                    onClicked: {
                        if (treeDelegateBase.checkState === Qt.Checked){
                            treeDelegateBase.root.uncheckItem(treeDelegateBase);
                        }
                        else{
                            treeDelegateBase.root.checkItem(treeDelegateBase);
                        }
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

                model: treeDelegateBase.itemData.ChildModel ? treeDelegateBase.itemData.ChildModel: 0;

                delegate: treeDelegateBase.root ? treeDelegateBase.root.rowDelegate : null;

                onCountChanged: {
                    treeDelegateBase.hasChild = count > 0;
                }

                onItemAdded: {
                    item.parentDelegate = treeDelegateBase;
                    treeDelegateBase.childrenDelegates.push(item);

                    item.modelIndex.parentIndex = treeDelegateBase.modelIndex;
                    treeDelegateBase.modelIndex.childModel.push(item.modelIndex);

                    item.modelIndex.treeModel = childModelRepeater.model;
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
        }
    }
}
