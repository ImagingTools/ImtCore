import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: packageCollectionContainer;
    anchors.fill: parent;
    property Item rootItem;
    property Item multiDocViewItem;
    property alias itemId: packageCollectionView.itemId;
    property alias itemName: packageCollectionView.itemName;
    property alias model: packageCollectionView.model;

    property int contextMenuX;
    property int contextMenuY;

    function refresh() {
        packageCollectionView.refresh();
    }

    function menuActivated(menuId) {
        packageCollectionView.menuActivated(menuId)
    }

    ListModel {
        id: contextMenuModel;

        Component.onCompleted: {
            contextMenuModel.append({"id": "Edit", "name": "Edit", "imageSource": "../../../Icons/Light/Edit_On_Normal.svg", "mode": "Normal"});
            contextMenuModel.append({"id": "Remove", "name": "Remove", "imageSource": "../../../Icons/Light/Remove_On_Normal.svg", "mode": "Normal"});
            contextMenuModel.append({"id": "", "name": "", "imageSource": "", "mode": "Normal"});
            contextMenuModel.append({"id": "SetDescription", "name": "Set Description", "imageSource": "", "mode": "Normal"});
            contextMenuModel.append({"id": "Rename", "name": "Rename", "imageSource": "", "mode": "Normal"});
        }
    }

    function commandsChanged(commandsId) {
        console.log("PackageCollectionView commandsChanged!", commandsId, packageCollectionContainer.rootItem);
        if (commandsId !== "Packages"){
            return;
        }
        console.log("packageCollectionView.selectedIndex", packageCollectionView.selectedIndex);
        if (packageCollectionView.selectedIndex > -1) {

            packageCollectionContainer.rootItem.setModeMenuButton("Remove", "Normal");
            packageCollectionContainer.rootItem.setModeMenuButton("Edit", "Normal");
        } else {
            packageCollectionContainer.rootItem.setModeMenuButton("Remove", "Disabled");
            packageCollectionContainer.rootItem.setModeMenuButton("Edit", "Disabled");
        }
    }

    function getDescriptionBySelectedItem(){
        var dataModelLocal = packageCollectionView.model.GetData("data");
        var description = dataModelLocal.GetData("Description", packageCollectionView.table.selectedIndex);
        return description;
    }

    function getNameBySelectedItem(){
        var dataModelLocal = packageCollectionView.model.GetData("data");
        var name = dataModelLocal.GetData("Name", packageCollectionView.table.selectedIndex);
        return name;
    }

    function updatePackageAfterRename(newId, newName) {
//        var dataModelLocal = packageCollectionView.model.GetData("data");

//        dataModelLocal.SetData("Id", newId, packageCollectionView.selectedIndex);
//        dataModelLocal.SetData("Name", value, packageCollectionView.table.selectedIndex);

        packageCollectionView.model.SetData("Id", newId, packageCollectionView.selectedIndex);
        packageCollectionView.model.SetData("Name", newName, packageCollectionView.table.selectedIndex);
    }

    function updatePackageAfterSetDescription(description) {
        packageCollectionView.model.SetData("Description", description, packageCollectionView.selectedIndex);
    }

    function dialogResult(parameters) {
         console.log("PackageCollectionView dialogResult", parameters["status"]);
        if (parameters["status"] === "ok") {
            if (parameters["dialog"] === "InputDialog") {
                var value = parameters["value"];
                var dataModelLocal = packageCollectionView.model.GetData("data");

                if (parameters["typeOperation"] === "SetDescription") {
                    setDescriptionModel.setDescription(value);
                }
                else if (parameters["typeOperation"] === "Rename"){
                    renameModel.rename(value);
                }

                packageCollectionView.model.SetData("data", dataModelLocal);
                packageCollectionView.refresh();
            }
        }
        else if (parameters["status"] === "Edit") {
            packageCollectionContainer.menuActivated("Edit");
        }
        else if (parameters["status"] === "Remove") {
            packageCollectionContainer.menuActivated("Remove");
        }
        else if (parameters["status"] === "Set Description") {
            var source = "AuxComponents/InputDialog.qml";
            var parameters = {};
            parameters["message"] = "Please enter the description of the package: ";
            parameters["nameDialog"] = "Set description";
            parameters["typeOperation"] = "SetDescription";

            parameters["startingValue"] = packageCollectionContainer.getDescriptionBySelectedItem();

            parameters["resultItem"] = packageCollectionContainer;
            thubnailDecoratorContainer.openDialog(source, parameters);
        }
        else if (parameters["status"] === "Rename") {
            var source = "AuxComponents/InputDialog.qml";
            var parameters = {};
            parameters["message"] = "Please enter the name of the package: ";
            parameters["nameDialog"] = "Rename Dialog";
            parameters["typeOperation"] = "Rename";

            parameters["startingValue"] = packageCollectionContainer.getNameBySelectedItem();
            parameters["resultItem"] = packageCollectionContainer;
            thubnailDecoratorContainer.openDialog(source, parameters);
        }
    }

    function getMenuButtonsX() {
        return packageCollectionContainer.contextMenuX + 75;
    }

    function getMenuButtonsY() {
        return packageCollectionContainer.contextMenuY + 132;
    }

    function openContextMenu(item, mouseX, mouseY) {
        var point = packageCollectionContainer.mapToItem(item, mouseX, mouseY);

        contextMenuX = point.x;
        contextMenuY = point.y;

        var source = "AuxComponents/PopupMenuDialog.qml";
        var parameters = {};
        parameters["model"] = contextMenuModel;
        parameters["backgroundOpacity"] = 0;
        parameters["resultItem"] = packageCollectionContainer;
        parameters["itemHeight"] = 25;
        parameters["itemWidth"] = 150;
        parameters["styleColor"] = Style.theme == "Dark" ? "Light" : "Dark";
        thubnailDecoratorContainer.openDialog(source, parameters);
    }

    CollectionView {
        id: packageCollectionView;
        anchors.fill: parent;
        autoRefresh: true;
        Component.onCompleted: {
            packageCollectionView.gqlModelInfo = "FeaturePackageInfo"
            packageCollectionView.gqlModelItems = "FeaturePackageList";
            packageCollectionView.gqlModelRemove = "FeaturePackageRemove";
            console.log("packageCollectionView onCompleted", packageCollectionView.gqlModelInfo)
        }

        onSelectItem: {
            console.log("packageCollectionView onSelectItem", packageCollectionView.selectedIndex);
            console.log("Item id = ", itemId);
            console.log("Name = ", name);
            var typeOperation = "Open";
            if (itemId === "") {
                name = "New Package";
                typeOperation = "New";
            }
            packageCollectionContainer.multiDocViewItem.addToHeadersArray(itemId, name,  "../../imtlicgui/PackageView.qml", "PackageEdit", typeOperation)
        }

        onRightButtonMouseClicked: {
            console.log("PackageView CollectionView AuxTable onRightButtonMouseClicked");
            packageCollectionContainer.openContextMenu(item, mouseX, mouseY);
        }

        onSelectedIndexChanged: {
            console.log("packageCollectionView onSelectedIndexChanged", packageCollectionView.selectedIndex);
            if (packageCollectionView.selectedIndex > -1){
                packageCollectionContainer.commandsChanged("Packages")
            }
        }
    }

    GqlModel {
        id: renameModel;

        function rename(newName) {
            console.log( "PackageCollectionView renameModel rename");

            var query;
            var queryFields;
            var inputParams = Gql.GqlObject("input");

            query = Gql.GqlRequest("query", "FeaturePackageRename");
            inputParams.InsertField("Id");
            inputParams.InsertFieldArgument("Id", packageCollectionView.table.getSelectedId());
            queryFields = Gql.GqlObject("rename");
            query.AddParam(inputParams);

            renameModel.SetData("Id", packageCollectionView.table.getSelectedId())
            renameModel.SetData("NewName", newName)

            var jsonString = renameModel.toJSON();
            jsonString = jsonString.replace(/\"/g,"\\\\\\\"")

            inputParams.InsertField("Item");
            inputParams.InsertFieldArgument ("Item", jsonString);

            queryFields.InsertField("Id");
            queryFields.InsertField("Successed");

            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("PackageCollectionView rename query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, renameModel);
            if (this.state === "Ready"){
                var dataModelLocal = this.GetData("data");

                if (dataModelLocal && dataModelLocal.ContainsKey("item")) {
                    dataModelLocal = dataModelLocal.GetData("item");

                    var newId = dataModelLocal.GetData("NewId");
                    var newName = dataModelLocal.GetData("NewName");

                    packageCollectionContainer.updatePackageAfterRename(newId, newName);
                }
            }
        }
    }

    GqlModel {
        id: setDescriptionModel;

        function setDescription(description) {
            console.log( "PackageCollectionView renameModel rename");

            var query;
            var queryFields;
            var inputParams = Gql.GqlObject("input");

            query = Gql.GqlRequest("query", "FeaturePackageSetDescription");
            inputParams.InsertField("Id");
            inputParams.InsertFieldArgument("Id", packageCollectionView.table.getSelectedId());
            queryFields = Gql.GqlObject("setDescription");
            query.AddParam(inputParams);

            setDescriptionModel.SetData("Id", packageCollectionView.table.getSelectedId())
            setDescriptionModel.SetData("Description", description)

            var jsonString = setDescriptionModel.toJSON();
            jsonString = jsonString.replace(/\"/g,"\\\\\\\"");

            inputParams.InsertField("Item");
            inputParams.InsertFieldArgument ("Item", jsonString);

            queryFields.InsertField("Id");
            queryFields.InsertField("Successed");

            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("PackageCollectionView setDescription query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, setDescriptionModel);
            if (this.state === "Ready"){
                var dataModelLocal = this.GetData("data");

                if (dataModelLocal && dataModelLocal.ContainsKey("item")) {
                    dataModelLocal = dataModelLocal.GetData("item");

                    var description = dataModelLocal.GetData("Description");

                    packageCollectionContainer.updatePackageAfterSetDescription(description);
                }
            }
        }
    }
}
