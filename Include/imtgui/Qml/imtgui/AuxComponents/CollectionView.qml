import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtlicgui 1.0

Item {
    id: collectionViewContainer;

    property int contentMargins: 0;

    property alias baseCollectionView: collectionViewBase;

    property string itemId;
    property string itemName;

    property string commandsId;
    property string commandsDelegatePath: "CollectionViewCommandsDelegateBase.qml";
    property string commandUpdateGui;
    property alias commandsDelegate: commandsLoader.item;

    property alias tableElementsDelegate: collectionViewBase.tableElementsDelegate;
    property alias tableHeadersDelegate: collectionViewBase.tableHeadersDelegate;
    property alias elementsList: collectionViewBase.elementsList;
    property alias metaInfo: collectionMetaInfo;

    Component.onCompleted: {
        itemId = documentsData.GetData("Id", model.index);
        itemName = documentsData.GetData("Title", model.index);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent(commandUpdateGui, updateGui);
    }

    onCommandUpdateGuiChanged: {
        Events.subscribeEvent(commandUpdateGui, updateGui);
    }

    onVisibleChanged: {
        if (visible){
            Events.sendEvent("CommandsModelChanged", {"Model": commandsProvider.commandsModel,
                                                      "CommandsId": commandsProvider.commandsId});
        }
    }

    onCommandsIdChanged: {
        console.log("this onItemIdChanged", itemId);

        commandsProvider.commandsId = commandsId;

        collectionViewBase.commands.gqlModelObjectView = commandsId + "ObjectView";
        collectionViewBase.commands.gqlModelHeadersInfo = commandsId + "Info";
        collectionViewBase.commands.gqlModelItemsInfo = commandsId + "List";

        collectionViewBase.commandsId = commandsId;

        collectionMetaInfo.gqlModelMetaInfo = commandsId + "MetaInfo";

        if (commandsLoader.item){
            commandsLoader.item.commandsId = commandsId;

            commandsLoader.item.gqlModelItem = commandsLoader.item.commandsId + "Item";
            commandsLoader.item.gqlModelRemove = commandsLoader.item.commandsId + "Remove";
            commandsLoader.item.gqlModelRename = commandsLoader.item.commandsId + "Rename";
            commandsLoader.item.gqlModelSetDescription = commandsLoader.item.commandsId + "SetDescription";
        }
    }

    function updateGui(){
        collectionViewBase.commands.updateModels();
    }

    function selectItem(id, name){
        console.log("CollectionView selectItem", id, name);
        let editorPath = collectionViewBase.commands.objectViewEditorPath;
        let commandsId = collectionViewBase.commands.objectViewEditorCommandsId;

        documentManager.addDocument({"Id": id, "Name": name, "Source": editorPath, "CommandsId": commandsId});
    }

    Loader {
        id: commandsLoader;

        Component.onCompleted: {
            console.log("commandsLoader.source", commandsDelegatePath);
            commandsLoader.source = commandsDelegatePath;
        }

        onLoaded: {
            commandsLoader.item.tableData = collectionViewBase.table;
        }
    }

    TreeItemModel {
        id: collectionViewModel;
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.baseColor;
    }

    CollectionViewBase {
        id: collectionViewBase;

        anchors.right: collectionMetaInfo.left;
        anchors.left: parent.left;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.margins: contentMargins;

        commandsId: parent.commandsId;
        loadData: true;

        onSelectedIndexChanged: {
            console.log("CollectionViewBase onSelectedIndexChanged");
            collectionMetaInfo.getMetaInfo();
        }

        onSelectedItem: {
            console.log("CollectionViewBase onItemSelected");
            selectItem(id, name);
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
    }
}

