import QtQuick 2.15
import Acf 1.0
import imtgui 1.0

Item {
    id: treeViewDelegate;

    property TreeItemModel treeViewModel;

    signal commandActivated(string commandId);

    property int selectedCount: table.selectedElements.count;

    Component.onCompleted: {
        Events.subscribeEvent(treeViewContainer.commandsId + "CommandActivated", commandHandle);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent(treeViewContainer.commandsId + "CommandActivated", commandHandle)
    }

    onSelectedCountChanged: {
        console.log("onSelectedCountChanged", treeViewDelegate.selectedCount);
        let isEnabled = treeViewDelegate.selectedCount > 0;

        commandsProvider.setCommandIsEnabled("New", isEnabled);
        commandsProvider.setCommandIsEnabled("Remove", isEnabled);
    }

    function insertNewItem(model){
        console.log("", model);
        console.log("=====================================");
        let index = model.InsertNewItem();

        model.SetData("Id", "", index);
        model.SetData("Name", "", index);
        model.SetData("Active", true, index);
        model.SetData("Opened", false, index);
        model.SetData("Selected", false, index);
        model.SetData("Visible", true, index);
        model.SetData("State", 0, index);
        model.SetData("Optional", 2, index);

        let parentModel = model.GetParent();

        let level = 1;
        if (parentModel){
            let id = parentModel.GetData("Id");
            let parentLevel = Number(parentModel.GetData("Level"));
            level = parentLevel + 1;
        }

        model.SetData("Level", level, index);
        model.AddTreeModel("ChildModel", index);

        console.log("=====================================");

        return index;
    }


//    /**
//       The tree has only one selected item warrantly
//    */
    function getSelectedItem(model){
        console.log("getSelectedItem", model);
        let count = model.GetItemsCount();
        for (let i = 0; i < count; i++){
            let id = model.GetData("Id", i);
            let selectedItem = model.GetData("Selected", i);

            if (selectedItem){
                return model;
            }

            let childModel = model.GetData("ChildModel", i);
            if (childModel){
                let result = getSelectedItem(childModel);
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

        let count = model.GetItemsCount();
        for (let i = 0; i < count; i++){
            let selected = model.GetData("Selected", i);

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
                let item = getSelectedItem(treeViewModel);
                let index = getSelectedItemIndex(item);

                if (buttonId == "Ok"){
                    item.SetData("Name", inputValue, index);
                }
            }
        }
    }

    function commandHandle(commandId){
        console.log("commandHandle", commandId);

        if (commandId == "NewRoot"){
            let index = insertNewItem(treeViewModel);

            treeViewModel.SetData("Id", "", index);
            treeViewModel.SetData("Name", "Feature Name", index);

            treeViewModel.Refresh();
        }
        else if (commandId == "New"){
            let item = getSelectedItem(treeViewModel);
            let index = getSelectedItemIndex(item);

            let childModel = item.GetData("ChildModel", index);
            if (!childModel){
                childModel = item.AddTreeModel("ChildModel", index);
            }

            let childIndex = insertNewItem(childModel);

            childModel.SetData("Id", "", childIndex);
            childModel.SetData("Name", "Feature Name", childIndex);

            treeViewModel.Refresh();
        }
        else if (commandId == "Remove"){

            let item = getSelectedItem(treeViewModel);
            let index = getSelectedItemIndex(item);
            item.RemoveItem(index);

        }
        else{
            commandActivated(commandId);
        }
    }
}
