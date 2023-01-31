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
    property bool visibleMetaInfo: true;

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

    property alias filterMenu: collectionViewBase.filterMenu;
    property alias filterMenuVisible: collectionViewBase.filterMenuVisible;
    property alias modelFilter: collectionViewBase.modelFilter;

    property TreeItemModel documentsData: TreeItemModel {}
    //property Item documentManager;


    Component.onCompleted: {
        collectionViewContainer.itemId = documentsData.GetData("Id", model.index);
        collectionViewContainer.itemName = documentsData.GetData("Title", model.index);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent(collectionViewContainer.commandUpdateGui, collectionViewContainer.updateGui);
    }

    onCommandUpdateGuiChanged: {
        Events.subscribeEvent(collectionViewContainer.commandUpdateGui, collectionViewContainer.updateGui);
    }

    onVisibleChanged: {
        if (collectionViewContainer.visible){
            Events.sendEvent("CommandsModelChanged", {"Model": commandsProvider.commandsModel,
                                                      "CommandsId": commandsProvider.commandsId});
        }
    }

    onCommandsIdChanged: {
        console.log("this onItemIdChanged", collectionViewContainer.commandsId);

        commandsProvider.commandsId = collectionViewContainer.commandsId;

        collectionViewBase.commands.gqlModelObjectView = collectionViewContainer.commandsId + "ObjectView";
        collectionViewBase.commands.gqlModelHeadersInfo = collectionViewContainer.commandsId + "Info";
        collectionViewBase.commands.gqlModelItemsInfo = collectionViewContainer.commandsId + "List";

        collectionViewBase.commandsId = collectionViewContainer.commandsId;

        collectionMetaInfo.gqlModelMetaInfo = collectionViewContainer.commandsId + "MetaInfo";

        if (commandsLoader.item){
            commandsLoader.item.commandsId = collectionViewContainer.commandsId;

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
            property Item rootItem:collectionViewContainer;
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
            onRootItemChanged: {
                contentLoader.item.rootItem = content.rootItem;

            }

            Loader {
                id: contentLoader;
                anchors.centerIn: parent;
                source: content.contentSource;
                onLoaded: {
                    contentLoader.item.root = content.root;
                    contentLoader.item.rootItem = content.rootItem;
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

            commandsLoader.item.collectionViewBase = collectionViewContainer;
            commandsLoader.item.commandsProvider = commandsProvider;
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
//            collectionViewContainer.selectItem(id, name);

            if (id == ""){
                commandsLoader.item.commandHandle("New");
            }
            else{
                commandsLoader.item.commandHandle("Edit");
            }
        }
    }

    CommandsProvider {
        id: commandsProvider;
    }

    MetaInfo {
        id: collectionMetaInfo;

        anchors.right: parent.right;

        width: visible ? 200 : 1;
        height: parent.height;

        visible: collectionViewContainer.visibleMetaInfo;

        tableData: collectionViewBase.table;

        contentVisible: collectionViewBase.table.selectedIndex != -1;

    }
}

