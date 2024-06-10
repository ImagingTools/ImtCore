import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Item {
    id: treeViewDelegate;

    property TreeItemModel treeViewModel;


    property int selectedCount;

    signal commandActivated(string commandId);

    Component.onCompleted: {
//        Events.subscribeEvent(treeViewContainer.commandsId + "CommandActivated", treeViewDelegate.commandHandle);
    }

    Component.onDestruction: {
//        Events.unSubscribeEvent(treeViewContainer.commandsId + "CommandActivated", treeViewDelegate.commandHandle)
    }

    onSelectedCountChanged: {
        console.log("onSelectedCountChanged", treeViewDelegate.selectedCount);
        let isEnabled = treeViewDelegate.selectedCount > 0;

    }

    function insertNewItem(model){
        console.log("", model);
        console.log("=====================================");
        let index = model.insertNewItem();

        model.setData("Id", "", index);
        model.setData("Name", "", index);
        model.setData("Active", true, index);
        model.setData("Opened", false, index);
        model.setData("Selected", false, index);
        model.setData("Visible", true, index);
        model.setData("State", 0, index);
        model.setData("Optional", 2, index);

        let parentModel = model.getParent();

        let level = 1;
        if (parentModel){
            let id = parentModel.getData("Id");
            let parentLevel = Number(parentModel.getData("Level"));
            level = parentLevel + 1;
        }

        model.setData("Level", level, index);
        model.addTreeModel("ChildModel", index);

        console.log("=====================================");

        return index;
    }


//    /**
//       The tree has only one selected item warrantly
//    */
    function getSelectedItem(model){
        console.log("getSelectedItem", model);
        let count = model.getItemsCount();
        for (let i = 0; i < count; i++){
            let id = model.getData("Id", i);
            let selectedItem = model.getData("Selected", i);

            if (selectedItem){
                return model;
            }

            let childModel = model.getData("ChildModel", i);
            if (childModel){
                let result = treeViewDelegate.getSelectedItem(childModel);
                if (result){
                    return result;
                }
            }
        }

        return null;
    }

    function getSelectedItemIndex(model){
        console.log("getSelectedItemIndex", model);
        if (!model){
            return -1;
        }

        let count = model.getItemsCount();
        for (let i = 0; i < count; i++){
            let selected = model.getData("Selected", i);

            if (selected){
                return i;
            }
        }

        return -1;
    }

    Component {
        id: inputDialog;

        InputDialog {
            onFinished: {
                let item = treeViewDelegate.getSelectedItem(treeViewDelegate.treeViewModel);
                let index = treeViewDelegate.getSelectedItemIndex(item);

                if (buttonId == "Ok"){
                    item.setData("Name", inputValue, index);
                }
            }
        }
    }

    function commandHandle(commandId){
        console.log("commandHandle", commandId);

        if (commandId == "NewRoot"){
            let index = treeViewDelegate.insertNewItem(treeViewDelegate.treeViewModel);

            treeViewDelegate.treeViewModel.setData("Id", "", index);
            treeViewDelegate.treeViewModel.setData("Name", "Feature Name", index);

            treeViewDelegate.treeViewModel.refresh();
        }
        else if (commandId == "New"){
            let item = treeViewDelegate.getSelectedItem(treeViewDelegate.treeViewModel);
            let index = treeViewDelegate.getSelectedItemIndex(item);

            let childModel = item.getData("ChildModel", index);
            if (!childModel){
                childModel = item.addTreeModel("ChildModel", index);
            }

            let childIndex = insertNewItem(childModel);

            childModel.setData("Id", "", childIndex);
            childModel.setData("Name", "Feature Name", childIndex);

            treeViewDelegate.treeViewModel.refresh();
        }
        else if (commandId == "Remove"){

            let item = treeViewDelegate.getSelectedItem(treeViewDelegate.treeViewModel);
            let index = treeViewDelegate.getSelectedItemIndex(item);
            item.removeItem(index);

        }
        else{
            treeViewDelegate.commandActivated(commandId);
        }
    }
}
