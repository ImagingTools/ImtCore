import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0
import imtlicgui 1.0

Item {
    id: collectionViewContainer;


    property int contentMargins: 0;

    property alias baseCollectionView: collectionViewBase;

    property string itemId;
    property string itemName;
    property bool isUsedDocumentManager: true;
    property bool visibleMetaInfo: true;
    property bool hasFilter: true;

    property string commandsId;
    property string editorPath;
    property string commandsDelegatePath: "CollectionViewCommandsDelegateBase.qml";
    property string commandUpdateGui;
    property alias commandsDelegate: commandsLoader.item;

    property alias table: collectionViewBase.table;
    property alias tableElementsDelegate: collectionViewBase.tableElementsDelegate;
    property alias tableHeadersDelegate: collectionViewBase.tableHeadersDelegate;
    property alias elementsList: collectionViewBase.elementsList;
    property alias tableMinWidth: collectionViewBase.tableMinWidth;
    property alias tableDecoratorPath: collectionViewBase.tableDecoratorPath;
    property alias tableDecoratorComp: collectionViewBase.tableDecoratorComp;
    property alias tableHeaderHeight: collectionViewBase.tableHeaderHeight;
    property alias tableItemHeight: collectionViewBase.tableItemHeight;
    property alias metaInfo: collectionMetaInfo;
    property alias tableCellDecorator: collectionViewBase.tableCellDecorator;
    property alias tableWidthDecorator: collectionViewBase.tableWidthDecorator;
    property alias tableWidth: collectionViewBase.tableWidth;
    property alias tableHeaders: collectionViewBase.tableHeaders;


    property alias filterMenu: collectionViewBase.filterMenu;
    property alias filterMenuVisible: collectionViewBase.filterMenuVisible;
    property alias modelFilter: collectionViewBase.modelFilter;
    property alias defaultSortHeaderIndex: collectionViewBase.defaultSortHeaderIndex;
    property alias defaultOrderType: collectionViewBase.defaultOrderType;

    property TreeItemModel documentsData: TreeItemModel {}
    property TreeItemModel notificationModel: collectionViewBase.commands.notificationModel;

    property Item documentManager: null;

    property ListModel contextMenuModel: ListModel {}

    property alias commandsProvider: commandsProviderLocal;

    property bool hasRemoteChanges: false;
    property bool isLocalChanges: false;

    signal elementsChanged();
    signal headersChanged();

    signal filterDecoratorLoaded();

    Component.onCompleted: {
        Events.subscribeEvent("FilterActivated", collectionViewContainer.filterMenuActivate);
        Events.subscribeEvent("OnLocalizationChanged", collectionViewContainer.onLocalizationChanged);
    }

    Component.onDestruction: {
        console.log("CollectionView onDestruction", itemId, collectionViewContainer);

        Events.unSubscribeEvent("OnLocalizationChanged", collectionViewContainer.onLocalizationChanged);
        Events.unSubscribeEvent(collectionViewContainer.commandUpdateGui, collectionViewContainer.updateGui);
        Events.unSubscribeEvent("FilterActivated", collectionViewContainer.filterMenuActivate);
    }

    onHasRemoteChangesChanged: {
        if (visible && hasRemoteChanges){
            setAlertPanel(alertPanelComp)
        }
    }

    property bool localizationChanged: false;

    function onLocalizationChanged(language){
        console.log("CommandsDecorator onLocalizationChanged", language);

        if (visible){
            commandsProviderLocal.updateModel();
        }
        else{
            localizationChanged = true;
        }

        fillContextMenuModel();
    }

    Keys.onPressed: {
        if (event.key === Qt.Key_Delete){
            commandsLoader.item.commandHandle("Remove");
        }
    }

    onDocumentsDataChanged: {
    	console.log("CollectionView onDocumentsDataChanged");
        collectionViewContainer.itemId = collectionViewContainer.documentsData.GetData("Id", model.index);
        collectionViewContainer.itemName = collectionViewContainer.documentsData.GetData("Title", model.index);
    }

    onCommandUpdateGuiChanged: {
        Events.subscribeEvent(collectionViewContainer.commandUpdateGui, collectionViewContainer.updateGui);
    }

    onVisibleChanged: {
        console.log("CollectionView onVisibleChanged", visible, itemId, collectionViewContainer);
        if (collectionViewContainer.visible){
            Events.sendEvent("CommandsModelChanged", {"Model": commandsProviderLocal.commandsModel,
                                                      "CommandsId": commandsProviderLocal.commandsId});

            Events.subscribeEvent("FilterActivated", collectionViewContainer.filterMenuActivate);

            if (localizationChanged){
                commandsProviderLocal.updateModel();

                localizationChanged = false;
            }

            if (hasRemoteChanges){
                setAlertPanel(alertPanelComp);
            }
        }
        else{
            Events.unSubscribeEvent("FilterActivated", collectionViewContainer.filterMenuActivate);

            if (hasRemoteChanges){
                setAlertPanel(undefined);
            }
        }
    }

    onDocumentManagerChanged: {
        console.log("onDocumentManagerChanged", collectionViewContainer.documentManager);
        if (collectionViewContainer.documentManager != null){
            if (commandsLoader.item && commandsLoader.item.documentManager !== undefined){
                commandsLoader.item.documentManager = collectionViewContainer.documentManager;
            }
        }
    }

    function filterMenuActivate(){
        if (collectionViewContainer.hasFilter){
            collectionViewContainer.filterMenuVisible = !collectionViewContainer.filterMenuVisible;
        }
    }

    function fillContextMenuModel(){
        contextMenuModel.clear();

        contextMenuModel.append({"Id": "Edit", "Name": qsTr("Edit"), "IconSource": "../../../../" + Style.getIconPath("Icons/Edit", "On", "Normal")});
        contextMenuModel.append({"Id": "Remove", "Name": qsTr("Remove"), "IconSource": "../../../../" + Style.getIconPath("Icons/Remove", "On", "Normal")});
        contextMenuModel.append({"Id": "Rename", "Name": qsTr("Rename"), "IconSource": ""});
        contextMenuModel.append({"Id": "SetDescription", "Name": qsTr("Set Description"), "IconSource": ""});
    }

    onCommandsIdChanged: {
        console.log("CollectionView onCommandsIdChanged", collectionViewContainer.commandsId);

        commandsProviderLocal.commandsId = collectionViewContainer.commandsId;
        commandsProviderLocal.documentUuid = collectionViewContainer.commandsId;

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

    function selectItem(id, name, index){
        Events.sendEvent("CommandsClearModel");

        let editorPath = collectionViewBase.commands.objectViewEditorPath;
        let commandsId = collectionViewBase.commands.objectViewEditorCommandsId;
        console.log("CollectionView selectItem", id, name, commandsId, editorPath);

        if(name === undefined){
            name = " ";
        }

        if (commandsId === ""){
            return;
        }

        if (collectionViewContainer.isUsedDocumentManager){
            if (id === ""){
                documentManager.addDocument({"Id": id, "Name": name, "CommandsId": commandsId, "Source": editorPath}, {}, false);
            }
            else{
                documentManager.openDocument(id, {"Id": id, "Name": name, "CommandsId": commandsId, "Source": editorPath});
            }
        }
        else{
            modalDialogManager.openDialog(contentDialog, {"contentId": id, "contentName": name, "contentCommandsId": commandsId,"contentSource": editorPath});
        }
    }

    function getEditorPath(){
        return collectionViewBase.commands.objectViewEditorPath;
    }

    function getEditorCommandId(){
        return collectionViewBase.commands.objectViewEditorCommandsId;
    }

    Component {
        id: contentDialog;
        Item {
            id: content;
            property Item root: null;
            property Item rootItem:collectionViewContainer;
            property bool centered: true;
            property string contentId;
            property string contentName;
            property string contentSource;
            property string contentCommandsId;
            width: contentLoader.width;
            height: contentLoader.height;

            onRootChanged: {
                if(contentLoader.item){
                    contentLoader.item.root = content.root;
                }
            }
            onRootItemChanged: {
                if(contentLoader.item){
                    contentLoader.item.rootItem = content.rootItem;
                }
            }

            Loader {
                id: contentLoader;
                anchors.centerIn: parent;
                source: content.contentSource;
                onLoaded: {
                    console.log("contentLoader onLoaded");

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
            commandsLoader.item.commandsProvider = commandsProviderLocal;
            commandsLoader.item.documentManager = collectionViewContainer.documentManager;

            commandsLoader.item.contextMenuModel = collectionViewContainer.contextMenuModel;
        }
    }

    TreeItemModel {
        id: collectionViewModel;
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.baseColor;
    }

    function getSelectedIds(){
        return collectionViewBase.table.getSelectedIds();
    }

    function getSelectedIndexes(){
        return collectionViewBase.table.getSelectedIndexes();
    }

    function getSelectedNames(){
        return collectionViewBase.table.getSelectedNames();
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
        hasFilter: collectionViewContainer.hasFilter;

        onFilterDecoratorLoaded: {
            collectionViewContainer.filterDecoratorLoaded();
        }

        onSelectionChanged: {
            if (collectionMetaInfo.visible){
                if (selection.length === 1){
                    let itemIds = collectionViewContainer.getSelectedIds();
                    let itemNames = collectionViewContainer.getSelectedNames();

                    if (itemIds.length > 0){
                        let itemId = itemIds[0];
                        collectionMetaInfo.getMetaInfo(itemId);
                    }

                    collectionMetaInfo.contentVisible = true;
                }
                else{
                    collectionMetaInfo.contentVisible = false
                }
            }
        }

        onSelectedItem: {
            console.log("CollectionViewBase onItemSelected", commandsLoader, commandsLoader.item);

            if (id == ""){
                commandsLoader.item.commandHandle("New");
            }
            else{
                commandsLoader.item.commandHandle("Edit");
            }
        }

        onElementsChanged: {
            collectionViewContainer.elementsChanged();
        }

        onHeadersChanged: {
            collectionViewContainer.headersChanged();
        }
    }

    CommandsProvider {
        id: commandsProviderLocal;

        onModelLoaded: {
            collectionViewContainer.onCommandsModelChanged();

            collectionViewContainer.fillContextMenuModel();
        }
    }

    function onCommandsModelChanged(){}

    MetaInfo {
        id: collectionMetaInfo;

        anchors.right: parent.right;

        width: visible ? 200 : 1;
        height: parent.height;

        visible: collectionViewContainer.visibleMetaInfo;
    }

    SubscriptionClient {
        id: subscriptionClient;

        property string commandId: collectionViewContainer.commandsId;
        onCommandIdChanged: {
            if (commandId !== ""){
                let subscriptionRequestId = "On" + commandId + "CollectionChanged"
                var query = Gql.GqlRequest("subscription", subscriptionRequestId);
                var queryFields = Gql.GqlObject("notification");
                queryFields.InsertField("Id");
                query.AddField(queryFields);
                subscriptionManager.registerSubscription(query, subscriptionClient);
            }
        }

        onStateChanged: {
            console.log("SubscriptionClient onStateChanged", state);

            if (state === "Ready"){
                if (subscriptionClient.ContainsKey("data")){
                    let dataModelLocal = subscriptionClient.GetData("data")
                    if (dataModelLocal.ContainsKey("token")){
                        let accessToken = dataModelLocal.GetData("token");
                        Events.sendEvent("GetToken", function (token){
                            if (String(token) == String(accessToken)){
                                collectionViewContainer.updateGui();
                            }
                            else{
                                collectionViewContainer.hasRemoteChanges = true;
                            }
                        });
                    }
                }
            }
        }
    }

    function setAlertPanel(alertPanelComp){
        if (collectionViewContainer.documentManager != null){
            collectionViewContainer.documentManager.alertPanelComp = alertPanelComp;
        }
    }

    Component {
        id: alertPanelComp;

        Rectangle {
            anchors.fill: parent;

            color: Style.selectedColor;

            property string commandId: collectionViewContainer.commandsId;

            Component.onCompleted: {
                if (commandId !== ""){
                    Events.subscribeEvent("Update" + commandId);
                }
            }

            Component.onDestruction: {
                if (commandId !== ""){
                    Events.unSubscribeEvent("Update" + commandId);
                }
            }

            function updateData(){
                updateButton.clicked();
            }

            Image {
                id: icon;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: 10;

                width: 20;
                height: 20;

                sourceSize.height: height;
                sourceSize.width: width;

                source: "../../../" + Style.getIconPath("Icons/Lamp", Icon.State.On, Icon.Mode.Normal);
            }

            BaseText {
                id: message;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: icon.right;
                anchors.leftMargin: 10;
                anchors.right: updateButton.left;

                text: qsTr("This collection has been modified from another computer");
            }

            BaseButton {
                id: updateButton;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: parent.right;
                anchors.rightMargin: 10;

                width: 70;
                height: 30;

                text: qsTr("Update");

                onClicked: {
                    collectionViewContainer.updateGui();

                    collectionViewContainer.hasRemoteChanges = false;

                    collectionViewContainer.documentManager.alertPanelComp = undefined;
                }
            }
        }
    }
}

