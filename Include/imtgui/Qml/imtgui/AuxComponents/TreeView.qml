import QtQuick 2.15
import Acf 1.0

Rectangle {
    id: treeViewContainer;

    color: Style.baseColor;

    property alias model: modelItems;

    signal itemStateChanged(var itemData);

    TreeItemModel {
        id: modelItems;
    }

    Component.onCompleted: {
        console.log("TreeView onCompleted");
        let json = treeViewModel.modelTreeView.toJSON();
        console.log("json", json);
        modelItems.Parse(json);
    }

//    Component.onCompleted: {
//        Events.subscribeEvent("TreeViewModelUpdateStarted", disconnectFunctions);
//        Events.subscribeEvent("TreeViewModelUpdateFinished", connectFunctions);
//    }

//    Component.onDestruction: {
//        Events.unSubscribeEvent("TreeViewModelUpdateStarted", disconnectFunctions);
//        Events.unSubscribeEvent("TreeViewModelUpdateFinished", connectFunctions);
//    }

    function connectFunctions(){
        for (let i = 0; i < mainTreeView.count; i++){
            let delegate = mainTreeView.itemAtIndex(i);

            //delegate.connectFunctions();
        }
    }

    function disconnectFunctions(){
        for (let i = 0; i < mainTreeView.count; i++){
            let delegate = mainTreeView.itemAtIndex(i);

            //delegate.disconnectFunctions();
        }
    }


    function resetSelectedItem(model){
        let count = model.GetItemsCount();
        for (let i = 0; i < count; i++){
            let selected = model.GetData("Selected", i);

            if (selected){
                model.SetData("Selected", false, i);
            }

            let childModel = model.GetData("ChildModel", i);
            if (childModel){
                resetSelectedItem(childModel);
            }
        }
    }

    function checkState(parentItemData, state){
        let result = true;
        if (parentItemData.Children){
            let children = parentItemData.Children;
            for (let child of children){

                if (child.State != state){
                    result = false;
                    break;
                }

                result &= checkState(child, state);
            }
        }

        return result;
    }

    Component {
        id: delegateComp;

        TreeItemDelegate {
            width: treeViewContainer.width;

            Component.onCompleted: {
                connectFunctions();
            }

            Component.onDestruction: {
                disconnectFunctions();
            }

//            function connectFunctions(){
//                this.itemVisibleChanged.connect(visibleChanged);
//                this.itemActiveChanged.connect(activeChanged);
//                this.itemStateChanged.connect(stateChanged);
//            }

//            function disconnectFunctions(){
//                this.itemVisibleChanged.disconnect(visibleChanged);
//                this.itemActiveChanged.disconnect(activeChanged);
//                this.itemStateChanged.disconnect(stateChanged);
//            }


            onClicked: {
                resetSelectedItem(modelItems);
                itemData.Selected = true;
            }

            onItemVisibleChanged: {
                console.log("TreeView onItemVisibleChanged", itemData.Id, itemData.Visible);
                if (itemData.Parent){
                    itemData.Parent.Visible = itemData.Visible;
                }
            }

            onItemActiveChanged: {
                if (itemData.Parent){
                    if (itemData.Parent.Active != itemData.Active){
                        itemData.Parent.Active = itemData.Active;
                    }
                }

                 if (itemData.Children){
                     let children = itemData.Children;
                     for (let child of children){
                         if (child.Active != itemActive){
                             child.Active = itemActive;
                         }
                     }
                 }
            }

            onStateChanged: {
                if (itemData.Children && state != Qt.PartiallyChecked){
                    let children = itemData.Children;
                    for (let child of children){
                        if (child.Active && child.State != state){
                            child.State = state;
                        }
                    }
                }

                if (itemData.Parent && itemData.Parent.Active){
                    let isAllChecked = checkState(itemData.Parent, Qt.Checked);
                    let isAllUnchecked = checkState(itemData.Parent, Qt.Unchecked);

                    if (isAllChecked){
                        if (itemData.Parent.State != Qt.Checked){
                            itemData.Parent.State = Qt.Checked
                        }
                    }
                    else if (isAllUnchecked){
                        if (itemData.Parent.State != Qt.Unchecked){
                            itemData.Parent.State = Qt.Unchecked
                        }
                    }
                    else if (!isAllChecked && !isAllUnchecked){
                        if (itemData.Parent.State != Qt.PartiallyChecked){
                            itemData.Parent.State = Qt.PartiallyChecked
                        }
                    }
                }

                treeViewContainer.itemStateChanged(itemData);
            }
        }
    }

    ListView {
        id: mainTreeView;

        anchors.fill: parent;

        boundsBehavior: Flickable.StopAtBounds;
        model: modelItems;

        delegate: delegateComp;
    }
}
