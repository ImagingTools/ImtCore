import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtlicgui 1.0

Item {
    id: collectionViewBaseContainer;

    anchors.fill: parent;

    property Item rootItem; //MultiDoc

    property alias baseCollectionView: collectionViewBase;

    property string itemId;
    property string itemName;

    property string commandsId;

    property string commandsDelegatePath;

    property string commandUpdateGui;

    Component.onCompleted: {
        collectionViewBase.forceActiveFocus();
    }

    onCommandUpdateGuiChanged: {
        Events.subscribeEvent(collectionViewBaseContainer.commandUpdateGui, collectionViewBaseContainer.updateGui);
    }

    onVisibleChanged: {
        if (collectionViewBaseContainer.visible){
            Events.sendEvent("CommandsModelChanged", {"Model": commandsProvider.commandsModel,
                                                      "CommandsId": commandsProvider.commandsId});
        }
    }

    onCommandsIdChanged: {
        console.log("collectionViewBaseContainer onItemIdChanged", collectionViewBaseContainer.itemId);

        commandsProvider.commandsId = collectionViewBaseContainer.commandsId;

        collectionViewBase.commands.gqlModelObjectView = collectionViewBaseContainer.commandsId + "ObjectView";
        collectionViewBase.commands.gqlModelHeadersInfo = collectionViewBaseContainer.commandsId + "Info";
        collectionViewBase.commands.gqlModelItemsInfo = collectionViewBaseContainer.commandsId + "List";

        collectionMetaInfo.gqlModelMetaInfo = collectionViewBaseContainer.commandsId + "MetaInfo";

        if (commandsLoader.item){
            commandsLoader.item.commandsId = collectionViewBaseContainer.commandsId;

            commandsLoader.item.gqlModelItem = commandsLoader.item.commandsId + "Item";
            commandsLoader.item.gqlModelRemove = commandsLoader.item.commandsId + "Remove";
            commandsLoader.item.gqlModelRename = commandsLoader.item.commandsId + "Rename";
            commandsLoader.item.gqlModelSetDescription = commandsLoader.item.commandsId + "SetDescription";
        }
    }

    function updateGui(){
        collectionViewBase.commands.updateModels();
    }

    Loader {
        id: commandsLoader;

        Component.onCompleted: {
            console.log("commandsLoader.source", collectionViewBaseContainer.commandsDelegatePath);
            commandsLoader.source = collectionViewBaseContainer.commandsDelegatePath;
        }

        onItemChanged: {
            console.log("commandsLoader onItemChanged", commandsLoader.item);
            if (commandsLoader.item){
                commandsLoader.item.collectionView = collectionViewBaseContainer;
                commandsLoader.item.tableData = collectionViewBase.table;
                commandsLoader.item.commandsProvider = commandsProvider;
            }
        }
    }

    TreeItemModel {
        id: collectionViewModel;
    }

    CollectionViewBase {
        id: collectionViewBase;

        anchors.right: collectionMetaInfo.left;
        anchors.left: collectionViewBaseContainer.left;
        anchors.top: collectionViewBaseContainer.top;
        anchors.bottom: collectionViewBaseContainer.bottom;

        onSelectedIndexChanged: {
            console.log("CollectionViewBase onSelectedIndexChanged");
            collectionMetaInfo.getMetaInfo();
        }

        onSelectedItem: {
            console.log("CollectionViewBase onItemSelected");

            let editorPath = collectionViewBase.commands.objectViewEditorPath;
            let commandsId = collectionViewBase.commands.objectViewEditorCommandsId;

            multiDocView.addPage({"Id": id, "Name": name, "Source": editorPath, "CommandsId": commandsId});
        }
    }

    CommandsProvider {
        id: commandsProvider;
    }

    MetaInfo {
        id: collectionMetaInfo;

        anchors.right: parent.right;

        width: 200;
        height: parent.height;

        tableData: collectionViewBase.table;

        contentVisible: collectionViewBase.table.selectedIndex != -1;

        color: Style.backgroundColor;
    }
}

