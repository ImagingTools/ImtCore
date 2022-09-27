import QtQuick 2.15
import Acf 1.0
import imtgui 1.0

Item {
    id: treeViewDelegate;

    property TreeItemModel treeViewModel;

    property alias commandsModel: commandsModel;

    signal commandActivated(string commandId);

    Component.onCompleted: {
        Events.subscribeEvent(treeViewContainer.commandsId + "CommandActivated", commandHandle);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent(treeViewContainer.commandsId + "CommandActivated", commandHandle)
    }

    function modelUpdated(){

    }

    function insertNewItem(model){
        let index = model.InsertNewItem();

        model.SetData("Id", "", index);
        model.SetData("Name", "", index);
        model.SetData("Active", true, index);
        model.SetData("Opened", false, index);
        model.SetData("Selected", false, index);
        model.SetData("Visible", true, index);
        model.SetData("State", 0, index);
        //model.SetData("Level", level, index);
        model.AddTreeModel("ChildModel", index);

        return index;
    }

    ListModel{
        id: commandsModel;

        ListElement{
            Id: "NewRoot";
            Name: qsTr("NewRoot");
            Mode: "Disabled";
            IconSource: "../../../../Icons/Light/PointsAdd_Off_Normal.svg";
        }

        ListElement{
            Id: "New";
            Name: qsTr("New");
            Mode: "Disabled";
            IconSource: "../../../../Icons/Light/Add_Off_Normal.svg";
        }

        ListElement{
            Id: "Remove";
            Name: qsTr("Remove");
            Mode: "Disabled";
            IconSource: "../../../../Icons/Light/Delete_Off_Normal.svg";
        }
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

            console.log("Id", id);
            console.log("Selected", selectedItem);
            console.log();
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


    /**
       The tree has only one selected item warrantly
    */
//    function getSelectedItem(object){
//        let model = object["Model"];
//        let count = model.GetItemsCount();
//        for (let i = 0; i < count; i++){
//            let id = model.GetData("Id", i);
//            let selectedItem = model.GetData("Selected", i);

//            console.log("Id", id);
//            console.log("Selected", selectedItem);
//            console.log();

//            if (selectedItem){
//                object["Index"] = i;
//                return true;
//            }

//            let childModel = model.GetData("ChildModel", i);
//            if (childModel){

//                object["Model"] = childModel;
//                object["Level"]++;

//                return getSelectedItem(object);
//            }
//        }

//        return false;
//    }

    function getSelectedItemIndex(model){

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

            treeViewModel.SetData("Id", "Test" + index, index);
            treeViewModel.SetData("Name", "Test" + index, index);
        }
        else if (commandId == "New"){
            let item = getSelectedItem(treeViewModel);
            let index = getSelectedItemIndex(item);

//            let params = {"Model": treeViewModel,
//                          "Index": 0,
//                          "Level": 0}

//            let result = getSelectedItem(params);
//            let index = params["Index"];
//            let level = params["Level"];

//            let item = params["Model"];
//            let index = getSelectedItemIndex(item);

            let childModel = item.GetData("ChildModel", index);
            if (!childModel){
                childModel = item.AddTreeModel("ChildModel", index);
            }

            let childIndex = insertNewItem(childModel);

//            treeViewModel.Refresh();

            childModel.SetData("Id", "Test" + childIndex, childIndex);
            childModel.SetData("Name", "Test" + childIndex, childIndex);

//            childModel.Refresh();
            treeViewModel.Refresh();
        }
        else if (commandId == "Remove"){
//            let params = {"Model": treeViewModel,
//                          "Index": 0,
//                          "Level": 0};

            let item = getSelectedItem(treeViewModel);
            let index = getSelectedItemIndex(item);
            item.RemoveItem(index);


//            if (item){
//                let model = params["Model"];
//                let index = params["Index"];

//                model.RemoveItem(index);

//            }

//            let index = getSelectedItemIndex(item);


//            treeViewModel.Refresh();
        }
        else if (commandId == "Edit"){

            let params = {"Model": treeViewModel,
                          "Index": 0,
                          "Level": 0};


            let item = getSelectedItem(params);
//            let index = getSelectedItemIndex(item);


            let name = item.GetData("Name", index);

            modalDialogManager.openDialog(inputDialog, {"message": qsTr("Please enter the name of the item:"), "inputValue": name});
        }
        else{
            commandActivated(commandId);
        }
    }
}
