import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
import imtlicgui 1.0

Item {
    id: container;

    property string commandsId;

    property string gqlModelQueryType;
    property string gqlModelQueryTypeNotify;

    property Item objectView;
    property Item tableData;
    property Item commandsProvider;

    property TreeItemModel objectModel;

    property int selectedIndex: container.tableData.selectedIndex;

    signal saved(string id, string name);
    signal closed();
    signal edited(string itemId, string itemName);
    signal removed(string itemId);

    /**
        Обновляем данные только после получения результата с сервера, удостоверясь
        что нет никаких ошибок
    */
    onSaved: {
        multiDocView.updatePageTitle({"ItemId": "", "Title": name});
    }

    onTableDataChanged: {
        console.log("DocumentView onTableDataChanged");
        tableData.rightButtonMouseClicked.connect(openPopupMenu);
    }

    onSelectedIndexChanged: {
        console.log("CollectionViewCommands onSelectedIndexChanged", container.selectedIndex, container);
        let mode = container.selectedIndex > -1 ? "Normal" : "Disabled";

        container.commandsProvider.changeCommandMode("Remove", mode);
        container.commandsProvider.changeCommandMode("Edit", mode);
    }

    function openPopupMenu(x, y){
        modalDialogManager.closeDialog();
        modalDialogManager.openDialog(popupMenu, {"x": x, "y": y, "model": contextMenuModel});
    }

    Component {
        id: errorDialog;
        MessageDialog {
        }
    }

    Component {
        id: messageDialog;
        MessageDialog {
            onFinished: {
                if (buttonId == "Yes"){
                    let elementsModel = objectModel.GetData("Items");

                    let selectedIndex = container.tableData.selectedIndex;
                    let removedId = elementsModel.GetData("Id", selectedIndex);

                    elementsModel.RemoveItem(selectedIndex);

                    if (selectedIndex === elementsModel.GetItemsCount()){
                        container.tableData.selectedIndex = -1;
                    }

                    removed(removedId);
                }
            }
        }
    }

    Component {
        id: editDialog;
        EditDialog {
            onFinished: {
                if (buttonId == "Ok"){
                    let elementsModel = objectModel.GetData("Items");

                    let oldId = elementsModel.GetData("Id", selectedIndex);
                    let oldName = elementsModel.GetData("Name", selectedIndex);

                    elementsModel.SetData("Id", valueId, container.selectedIndex);
                    elementsModel.SetData("Name", valueName, container.selectedIndex);

                    undoRedoManager.modelChanged();

                    edited(oldId, oldName);

                    objectView.updateGui();
                }
            }
        }
    }

    Component {
        id: inputDialog;
        InputDialog {
            onFinished: {
                console.log("InputDialog result", buttonId, inputValue);
                if (buttonId == "Ok"){
                    container.gqlModelQueryType = "Add";
                    container.gqlModelQueryTypeNotify = "addedNotification";

                    container.objectView.itemId = inputValue;
                    container.objectView.itemName = inputValue;

                    saveQuery.updateModel();
                }
            }
        }
    }

    Component {
        id: setDescriptionDialog;
        InputDialog {
            onFinished: {
                if (buttonId == "Ok"){
                    let elements = container.tableData.elements;
                    elements.SetData("Description", inputValue, selectedIndex);

                    objectView.updateGui();

                    undoRedoManager.modelChanged();
                }
            }
        }
    }

    Component {
        id: popupMenu;

        PopupMenuDialog {
            onFinished: {
                console.log("DocumentView PopupMenuDialog", commandId);
                commandHandleBase(commandId);
            }
        }
    }

    ListModel {
        id: contextMenuModel;

        Component.onCompleted: {
            contextMenuModel.append({"Id": "Edit", "Name": qsTr("Edit"), "IconSource": "../../../../Icons/Light/Edit_On_Normal.svg"});
            contextMenuModel.append({"Id": "Remove", "Name": qsTr("Remove"), "IconSource": "../../../../Icons/Light/Remove_On_Normal.svg"});
            contextMenuModel.append({"Id": "SetDescription", "Name": qsTr("Set Description"), "IconSource": ""});
        }
    }

    function commandHandleBase(commandId){
        if (commandId === "New"){
            let elements = container.tableData.elements;
            let index = elements.InsertNewItem();
            elements.SetData("Name", "Item Name", index);
            elements.SetData("Id", "", index);
            elements.SetData("Description", "", index);

            objectView.updateGui();

            undoRedoManager.modelChanged();
        }
        else if (commandId === "Remove"){
            modalDialogManager.openDialog(messageDialog, {"message": qsTr("Remove selected item from the document ?")});
        }
        else if (commandId === "Edit"){
            let id = container.tableData.getSelectedId();
            let name = container.tableData.getSelectedName();
            modalDialogManager.openDialog(editDialog, {"titleId":   qsTr("Item-ID"),
                                                       "titleName": qsTr("Item Name"),
                                                       "valueId":   id,
                                                       "valueName": name,
                                                       "model":     tableData.elements});
        }
        else if (commandId === "Save"){
            let itemId = container.objectView.itemId;
            if (itemId === ""){
                modalDialogManager.openDialog(inputDialog, {"message": qsTr("Please enter the name of the item: ")});
            }
            else{
                container.gqlModelQueryType = "Update";
                container.gqlModelQueryTypeNotify = "updatedNotification";

                saveQuery.updateModel();
            }
        }
        else if (commandId === "Close"){
            container.closed();
            multiDocView.closePage("");
        }
        else if (commandId === "SetDescription"){
            let elements = tableData.elements;
            let selectedDescription = elements.GetData("Description", selectedIndex);
            modalDialogManager.openDialog(setDescriptionDialog, {"message": qsTr("Please enter the description of the item:"),
                                                                 "inputValue": selectedDescription});
        }
    }

    function saveModel(){
        saveQuery.updateModel();
    }

    GqlModel {
        id: saveQuery;

        function updateModel(){
            console.log( "updateModel saveModel");
            var query = Gql.GqlRequest("query", container.commandsId + container.gqlModelQueryType);
            var queryFields = Gql.GqlObject(container.gqlModelQueryTypeNotify);

            let itemId = container.objectView.itemId;
            let itemName = itemId;

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", itemId);

            var jsonString = objectModel.toJSON();
            jsonString = jsonString.replace(/\"/g,"\\\\\\\"")

            inputParams.InsertField ("Item", jsonString);

            query.AddParam(inputParams);

            queryFields.InsertField("Id");
            queryFields.InsertField("Successed");

            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("ObjectView query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, saveQuery);
            if (this.state === "Ready"){
                var dataModelLocal;
                if (saveQuery.ContainsKey("errors")){
                    dataModelLocal = saveQuery.GetData("errors");
                    dataModelLocal = dataModelLocal.GetData(container.commandsId + container.gqlModelQueryType);
                    if (gqlModelQueryType == "Add"){
                        container.objectView.itemId = "";
                    }

                    if (dataModelLocal.ContainsKey("message")){
                        let message = dataModelLocal.GetData("message");
                        modalDialogManager.openDialog(errorDialog, {"message": message});
                    }

                    return;
                }

                if (saveQuery.ContainsKey("data")){
                    dataModelLocal = saveQuery.GetData("data");

                    dataModelLocal = dataModelLocal.GetData(container.commandsId + container.gqlModelQueryType);

                    if (dataModelLocal.ContainsKey(gqlModelQueryTypeNotify)){
                        dataModelLocal = dataModelLocal.GetData(gqlModelQueryTypeNotify);

                        let itemId = dataModelLocal.GetData("Id");
                        let itemName = dataModelLocal.GetData("Name");

                        container.saved(itemId, itemName);
                    }
                }
            }
        }
    }
}

