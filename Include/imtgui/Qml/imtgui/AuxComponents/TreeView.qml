import QtQuick 2.15
import Acf 1.0

Rectangle {
    id: treeViewContainer;

    color: Style.baseColor;

    property TreeItemModel model: modelItems;

    signal itemStateChanged(var itemData);

    property alias itemDelegate: delegateLoader.sourceComponent;

    property UndoRedoManager undoRedoManager;

    TreeItemModel {
        id: modelItems;
    }

    Component.onCompleted: {
        Events.subscribeEvent("TreeViewModelUpdateFinished", refreshModel)
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("TreeViewModelUpdateFinished", refreshModel)
    }

    function getItemDataById(itemId){
        //Обход по делегатам packages
        for (let i = 0; i < mainTreeView.count; i++){
            let item = mainTreeView.itemAtIndex(i);

            let childRepeater = item.childRepeater;

            //Обход по делегатам репитора children
            for (let j = 0; j < childRepeater.count; j++){
                let childItem = childRepeater.itemAt(j);

                let itemData = childItem.itemData;
                console.log("itemData", itemData);
                let currentItemId = itemData.Id;

                if (currentItemId == itemId){
                    return itemData;
                }

                if (itemData.Children){
                    let result = _getItemDataRecursive(itemData.Children, itemId);

                    if (result != null){
                        return result;
                    }
                }
            }
        }

        return null;
    }

     function _getItemDataRecursive(childModel, itemId){
         if (childModel){
             for (let child of childModel){
                 if (child.Id == itemId){
                     return child;
                 }

                 if (child.Children){
                      let result = _getItemDataRecursive(child.Children, itemId);
                     if (result != null){
                         return result;
                     }
                 }
             }
         }

         return null;
     }

    function refreshModel(){
        treeViewContainer.model = 0;
        treeViewContainer.model = treeViewModel.modelTreeView;
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

    function itemVisibleChanged(itemData){
        let parent = itemData.Parent;
        while (parent){
            parent.Visible = itemData.Visible;
            parent = parent.Parent;
        }
    }

    function itemsStateChanged(itemData){
        console.log("itemsStateChanged");
        if (undoRedoManager){
            undoRedoManager.beginChanges();
        }

        itemStateChanged(itemData);

        childrenStateChanged(itemData, itemData.State);
        parentStateChanged(itemData, itemData.State);

        if (undoRedoManager){
            undoRedoManager.endChanges();
        }
    }

    function childrenStateChanged(itemData, state){
        if (itemData.Children){
            for (let child of itemData.Children){
                if (child.Active && child.State != state){
                    child.State = state;
                    itemStateChanged(child);
                    childrenStateChanged(child, state);
                }
            }
        }
    }

    function parentStateChanged(itemData, state){
        console.log("parentStateChanged", itemData.Id, itemData.State);
        let parent = itemData.Parent;
        if (parent && parent.Active){
            let isAllChecked = checkState(parent, Qt.Checked);
            let isAllUnchecked = checkState(parent, Qt.Unchecked);

            if (isAllChecked){
                if (parent.State != Qt.Checked){
                    parent.State = Qt.Checked
                }
            }
            else if (isAllUnchecked){
                if (parent.State != Qt.Unchecked){
                    parent.State = Qt.Unchecked
                }
            }
            else if (!isAllChecked && !isAllUnchecked){
                if (parent.State != Qt.PartiallyChecked){
                    parent.State = Qt.PartiallyChecked
                }
            }

            parentStateChanged(parent, state);
            itemStateChanged(parent);
        }
    }

    Loader {
        id: delegateLoader;

        sourceComponent: TreeItemDelegate {
            width: treeViewContainer.width;

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
        }
    }

    ListView {
        id: mainTreeView;

        anchors.fill: parent;

        boundsBehavior: Flickable.StopAtBounds;
        model: treeViewContainer.model;

        delegate: treeViewContainer.itemDelegate;
    }

    function clearModel(){
        mainTreeView.model.Clear();
    }
}
