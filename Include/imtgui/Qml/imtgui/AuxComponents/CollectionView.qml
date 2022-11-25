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
    property bool isUsedDocumentManager: true;

    property string commandsId;
    property string editorPath;
    property string commandsDelegatePath: "CollectionViewCommandsDelegateBase.qml";
    property string commandUpdateGui;
    property alias commandsDelegate: commandsLoader.item;

    property alias tableElementsDelegate: collectionViewBase.tableElementsDelegate;
    property alias tableHeadersDelegate: collectionViewBase.tableHeadersDelegate;
    property alias elementsList: collectionViewBase.elementsList;
    property alias tableMinWidth: collectionViewBase.tableMinWidth;
    property alias tableDecoratorPath: collectionViewBase.tableDecoratorPath;
    property alias tableHeaderHeight: collectionViewBase.tableHeaderHeight;
    property alias tableItemHeight: collectionViewBase.tableItemHeight;
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
        let editorPath = collectionViewBase.commands.objectViewEditorPath;
        let commandsId = collectionViewBase.commands.objectViewEditorCommandsId;
        console.log("CollectionView selectItem", id, name, commandsId, editorPath);

        if (collectionViewContainer.isUsedDocumentManager){
            documentManager.addDocument({"Id": id, "Name": name, "Source": editorPath, "CommandsId": commandsId});
        }
        else{
            modalDialogManager.openDialog(contentDialog, {"contentId": id, "contentName": name, "contentSource": editorPath, "contentCommandsId": commandsId});
        }
    }

    Component {
        id: contentDialog;
        Item {
            id: content;
            property Item root;
            property bool centered: true;
            property string contentId;
            property string contentName;
            property string contentSource;
            property string contentCommandsId;
            width: contentLoader.width;
            height: contentLoader.height;

            onRootChanged: {
                contentLoader.item.root = content.root;
            }

            Loader {
                id: contentLoader;
                anchors.centerIn: parent;
                source: content.contentSource;
                onLoaded: {
                    contentLoader.item.root = content.root;
                    contentLoader.width = item.width;
                    contentLoader.height = item.height;
                    contentLoader.item.itemId = content.contentId;
                    contentLoader.item.itemName = content.contentName;
                    contentLoader.item.commandsId = content.contentCommandsId;
                }
            }
        }

    }

    Loader {
        id: commandsLoader;

        Component.onCompleted: {
            console.log("commandsLoader.source", parent.commandsDelegatePath);
            commandsLoader.source = parent.commandsDelegatePath;
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
        anchors.margins: parent.contentMargins;

        itemId: collectionViewContainer.itemId;

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

