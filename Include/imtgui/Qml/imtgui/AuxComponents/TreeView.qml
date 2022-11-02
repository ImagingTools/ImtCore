import QtQuick 2.15
import Acf 1.0

Rectangle {
    id: treeViewContainer;

    color: Style.baseColor;

    property TreeItemModel model: modelItems;

    signal itemStateChanged(var itemData);

    property alias itemDelegate: delegateLoader.sourceComponent;

    TreeItemModel {
        id: modelItems;
    }

    Component.onCompleted: {
        Events.subscribeEvent("TreeViewModelUpdateFinished", refreshModel)
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("TreeViewModelUpdateFinished", refreshModel)
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

    function _itemStateChanged(itemData, state, isForward){
        console.log("_itemStateChanged", itemData.Id, state, isForward);
        if (isForward){
            if (itemData.Children){
                for (let child of itemData.Children){
                    if (child.Active && child.State != state){
                        child.State = state;
                        _itemStateChanged(child, state,  true);
                    }
                }
            }
        }
        else{
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

                if (parent.Parent){
                    _itemStateChanged(parent.Parent, state, false);
                }
            }
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

//            onStateChanged: {
//                if (itemData.Children && state != Qt.PartiallyChecked){
//                    let children = itemData.Children;
//                    for (let child of children){
//                        if (child.Active && child.State != state){
//                            child.State = state;
//                        }
//                    }
//                }

//                if (itemData.Parent && itemData.Parent.Active){
//                    let isAllChecked = checkState(itemData.Parent, Qt.Checked);
//                    let isAllUnchecked = checkState(itemData.Parent, Qt.Unchecked);

//                    if (isAllChecked){
//                        if (itemData.Parent.State != Qt.Checked){
//                            itemData.Parent.State = Qt.Checked
//                        }
//                    }
//                    else if (isAllUnchecked){
//                        if (itemData.Parent.State != Qt.Unchecked){
//                            itemData.Parent.State = Qt.Unchecked
//                        }
//                    }
//                    else if (!isAllChecked && !isAllUnchecked){
//                        if (itemData.Parent.State != Qt.PartiallyChecked){
//                            itemData.Parent.State = Qt.PartiallyChecked
//                        }
//                    }
//                }

//                treeViewContainer.itemStateChanged(itemData);
//            }
        }
    }

//    Component {
//        id: delegateComp;

//        TreeItemDelegate {
//            width: treeViewContainer.width;

//            onClicked: {
//                resetSelectedItem(modelItems);
//                itemData.Selected = true;
//            }

//            onItemVisibleChanged: {
//                console.log("TreeView onItemVisibleChanged", itemData.Id, itemData.Visible);
//                if (itemData.Parent){
//                    itemData.Parent.Visible = itemData.Visible;
//                }
//            }

//            onItemActiveChanged: {
//                if (itemData.Parent){
//                    if (itemData.Parent.Active != itemData.Active){
//                        itemData.Parent.Active = itemData.Active;
//                    }
//                }

//                 if (itemData.Children){
//                     let children = itemData.Children;
//                     for (let child of children){
//                         if (child.Active != itemActive){
//                             child.Active = itemActive;
//                         }
//                     }
//                 }
//            }

//            onStateChanged: {
//                if (itemData.Children && state != Qt.PartiallyChecked){
//                    let children = itemData.Children;
//                    for (let child of children){
//                        if (child.Active && child.State != state){
//                            child.State = state;
//                        }
//                    }
//                }

//                if (itemData.Parent && itemData.Parent.Active){
//                    let isAllChecked = checkState(itemData.Parent, Qt.Checked);
//                    let isAllUnchecked = checkState(itemData.Parent, Qt.Unchecked);

//                    if (isAllChecked){
//                        if (itemData.Parent.State != Qt.Checked){
//                            itemData.Parent.State = Qt.Checked
//                        }
//                    }
//                    else if (isAllUnchecked){
//                        if (itemData.Parent.State != Qt.Unchecked){
//                            itemData.Parent.State = Qt.Unchecked
//                        }
//                    }
//                    else if (!isAllChecked && !isAllUnchecked){
//                        if (itemData.Parent.State != Qt.PartiallyChecked){
//                            itemData.Parent.State = Qt.PartiallyChecked
//                        }
//                    }
//                }

//                treeViewContainer.itemStateChanged(itemData);
//            }
//        }
//    }

    ListView {
        id: mainTreeView;

        anchors.fill: parent;

        boundsBehavior: Flickable.StopAtBounds;
        model: treeViewContainer.model;

        delegate: itemDelegate;
    }

    function clearModel(){
        mainTreeView.model.Clear();
    }
}
