import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtlicgui 1.0
import imtqml 1.0

Item {
    id: gridCollectionViewContainer;

    clip: true;

    property int contentMargins: 0;

    property alias baseGridCollectionView: gridCollectionViewBase;

    property string itemId;
    property string itemName;
    property bool isUsedDocumentManager: true;
    property bool visibleMetaInfo: true;

    property string editorPath;
    property string commandsDelegatePath: "CollectionViewCommandsDelegateBase.qml";
    property string commandUpdateGui;
    property alias commandsDelegate: commandsLoader.item;

    property alias commandsId: gridCollectionViewBase.commandsId;
    property alias commands: gridCollectionViewBase.commands;
    property alias gridCellWidth: gridCollectionViewBase.gridCellWidth;
    property alias gridCellHeight: gridCollectionViewBase.gridCellHeightMin;
    property alias gridContentHeight: gridCollectionViewBase.gridContentHeight;
    property alias gridContentY: gridCollectionViewBase.gridContentY;
    property alias gridBottomMargin: gridCollectionViewBase.gridBottomMargin;

    property alias hasExtention: gridCollectionViewBase.hasExtention;
    property alias openST: gridCollectionViewBase.openST;

    property alias gridCount: gridCollectionViewBase.gridCount;
    property alias gridCountInLine: gridCollectionViewBase.gridCountInLine;
    property alias gridRowCount : gridCollectionViewBase.gridRowCount;
    property alias gridSelectedRow: gridCollectionViewBase.gridSelectedRow;
    property alias gridIsLastRow: gridCollectionViewBase.gridIsLastRow;
    property alias gridAddHeight: gridCollectionViewBase.gridAddHeight;
    property alias gridCellHeightMin: gridCollectionViewBase.gridCellHeightMin;
    property alias gridCellHeightMax: gridCollectionViewBase.gridCellHeightMax;

    property alias extendingInfoComp: gridCollectionViewBase.extendingInfoComp;
    property alias gridDelegateMargin: gridCollectionViewBase.gridDelegateMargin;

    property alias gridIndicatorMainColor: gridCollectionViewBase.gridIndicatorMainColor;
    property alias gridIndicatorCoverColor: gridCollectionViewBase.gridIndicatorCoverColor;
    property alias gridIndicatorCoverOpacity: gridCollectionViewBase.gridIndicatorCoverOpacity;
    property alias gridIndicatorVisible: gridCollectionViewBase.gridIndicatorVisible;

    property alias gridElementsDelegate: gridCollectionViewBase.gridElementsDelegate;
    property alias gridElementsModel: gridCollectionViewBase.gridElementsModel;
    //property alias selectedIndex: gridCollectionViewBase.grid.selectedIndex;
    property alias selectedIndex: gridCollectionViewBase.selectedIndex;
//    property alias elementsList: gridCollectionViewBase.elementsList;
//    property alias gridMinWidth: gridCollectionViewBase.gridMinWidth;
    property alias gridDecoratorPath: gridCollectionViewBase.gridDecoratorPath;

//    property alias tableItemHeight: gridCollectionViewBase.gridItemHeight;
    property alias metaInfo: collectionMetaInfo;
    property alias commandsProvider: commandsProvider;
    property alias paginationCurrentIndex: gridCollectionViewBase.paginationCurrentIndex;
    property alias paginationPageSize: gridCollectionViewBase.paginationPageSize;



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
            console.log("IDTEST::Model", commandsProvider.commandsModel, "IDTEST::CommandsId", commandsProvider.commandsId);
        }
    }

    onCommandsIdChanged: {
        console.log("this onItemIdChanged", itemId, commandsId);

        commandsProvider.commandsId = commandsId;

        gridCollectionViewBase.gqlModelObjectView = commandsId + "ObjectView";
        gridCollectionViewBase.gqlModelHeadersInfo = commandsId + "Info";
        gridCollectionViewBase.gqlModelItemsInfo = commandsId + "List";

        gridCollectionViewBase.commandsId = commandsId;

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
        gridCollectionViewBase.commands.updateModels();
    }

    function selectItem(id, name){
        console.log("MEGATEST2")
        let editorPath = gridCollectionViewBase.commands.objectViewEditorPath;
//        let editorPath = "qrc:/Chronos/DeviceElementCollectionView.qml"
        let commandsId = gridCollectionViewBase.commands.objectViewEditorCommandsId;
        console.log("CollectionView selectItem", id, name, commandsId, editorPath);

        if (gridCollectionViewContainer.isUsedDocumentManager){
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

            width: contentLoader.width;
            height: contentLoader.height;

            property Item root;
            property bool centered: true;
            property string contentId;
            property string contentName;
            property string contentSource;
            property string contentCommandsId;

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
            commandsLoader.item.tableData = gridCollectionViewBase.grid;
        }
    }

    TreeItemModel {
        id: collectionViewModel;
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.baseColor;
    }

    GridCollectionViewBase {
        id: gridCollectionViewBase;

//        anchors.right: collectionMetaInfo.left;
//        anchors.left: parent.left;
//        anchors.top: parent.top;
//        anchors.bottom: parent.bottom;

        anchors.fill: parent;

        anchors.margins: parent.contentMargins;

        itemId: gridCollectionViewContainer.itemId;

        commandsId: parent.commandsId;
        loadData: true;

        onSelectedIndexChangedSignal: {
            console.log("gridCollectionViewBase onSelectedIndexChanged");
            collectionMetaInfo.getMetaInfo();
        }

        onSelectedItem: {
            console.log("gridCollectionViewBase onItemSelected");
            selectItem(id, name);
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

        visible: gridCollectionViewContainer.visibleMetaInfo;

//        tableData: gridCollectionViewBase.grid;

//        contentVisible: gridCollectionViewBase.grid.selectedIndex != -1;

    }
}

