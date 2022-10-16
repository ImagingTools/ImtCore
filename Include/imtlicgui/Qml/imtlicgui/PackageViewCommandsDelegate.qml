import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

DocumentWorkspaceCommandsDelegateBase {
    id: container;

    showInputIdDialog: true;

    property Item tableTreeViewEditor;

    Component.onCompleted: {
        Events.subscribeEvent("TreeViewModelUpdateFinished", updateGui)
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("TreeViewModelUpdateFinished", updateGui)
        tableTreeViewEditor.selectedIndexChanged.disconnect(indexChanged)
    }

    onEntered: {
        objectModel.SetData("Id", value);
        objectModel.SetData("Name", value);
    }

    onTableTreeViewEditorChanged: {
        console.log('DocumentCommands onTableTreeViewEditorChanged');
        tableTreeViewEditor.selectedIndexChanged.connect(indexChanged)
    }

    onSaved: {
        if (gqlModelQueryType == "Add"){
            Events.sendEvent("TreeViewModelUpdate");
        }

        Events.sendEvent("FeatureDependenciesUpdate");
    }

    onClosed: {
        Events.sendEvent("TreeViewModelUpdate");
        Events.sendEvent("FeatureDependenciesUpdate");
    }

    onVisibleChanged: {
        console.log("PackageViewCommands onVisibleChanged", visible);
        console.log(objectModel.GetData("Id"));
    }

    onCommandActivated: {
        console.log("PackageViewCommands onCommandActivated", commandId);

        let model = tableTreeViewEditor.elements; //Elements model

        let itemIndex = -1;
        if (tableTreeViewEditor.selectedIndex){
            //Get path of indexes for the selected item.
            let indexes = tableTreeViewEditor.selectedIndex.getIndexes();

            //Последний индекс наш item(ChildModel может не быть у него)
            itemIndex = indexes.pop()

            if (itemIndex >= 0){
                for (let index of indexes){
                    model = model.GetData('ChildModel', index)
                }
            }
        }

        if (commandId === "New"){
            let childModel;
            if (itemIndex < 0){
                childModel = model;
            }
            else{
                childModel = model.GetData('ChildModel', itemIndex);
                if (!childModel){
                    childModel = model.AddTreeModel('ChildModel', itemIndex);
                }
            }

            let childIndex = insertNewItem(childModel)

            childModel.SetData("Id", "", childIndex);
            childModel.SetData("Name", "Feature Name", childIndex);

            updateGui();

//            tableTreeViewEditor.table.elements = 0;
//            tableTreeViewEditor.table.elements = objectModel.GetData("Items");
        }
        else if (commandId === "Remove"){
            modalDialogManager.openDialog(messageDialog, {"message": qsTr("Remove selected feature from the package ?")});
        }


    }

    function indexChanged(){
        console.log('DocumentCommands indexChanged', tableTreeViewEditor.selectedIndex);

        let mode = tableTreeViewEditor.selectedIndex ? "Normal" : "Disabled";
        commandsProvider.changeCommandMode("Remove", mode);
    }

    function insertNewItem(model){
        let index = model.InsertNewItem();

        model.SetData("Description", "", index);
        model.SetData("Active", true, index);
        model.SetData("Opened", false, index);
        model.SetData("Selected", false, index);
        model.SetData("Visible", true, index);
        model.SetData("State", Qt.Unchecked, index);
        model.SetData("Optional", false, index);

        let parentModel = model.GetParent();

        let level = 1;
        if (parentModel && parentModel != model){
            let id = parentModel.GetData("Id");
            let parentLevel = Number(parentModel.GetData("Level"));

            if (parentLevel){
                level = parentLevel + 1;
            }
        }

        model.SetData("Level", level, index);
        model.AddTreeModel("ChildModel", index);

        return index;
    }

    Component {
        id: messageDialog;

        MessageDialog {
            onFinished: {
                if (buttonId == "Yes"){
                    let model = tableTreeViewEditor.elements; //Elements model

                    //Get path of indexes for the selected item.
                    console.log("tableTreeViewEditor", tableTreeViewEditor);
                    console.log("tableTreeViewEditor.selectedIndex", tableTreeViewEditor.selectedIndex);
                    let indexes = tableTreeViewEditor.selectedIndex.getIndexes();
                    console.log("indexes", indexes);

                    //Последний индекс наш item(ChildModel может не быть у него)
                    let itemIndex = indexes.pop()

                    console.log("itemIndex", itemIndex);

                    if (itemIndex >= 0){
                        for (let index of indexes){
                            model = model.GetData('ChildModel', index)
                        }

                         model.RemoveItem(itemIndex);
                    }
                }
            }
        }
    }
}

