import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtqml 1.0
import imtcolgui 1.0

Item {
    id: collectionViewContainer;

    property string uuid;
    property var documentManagerPtr: null;
    property string documentName;

    property int contentMargins: 0;

    property alias baseCollectionView: collectionViewBase;

    property bool isUsedDocumentManager: true;
    property bool visibleMetaInfo: true;
    property bool hasFilter: true;

    property string commandId;
    property string documentTypeId;

    property string commandsDelegatePath: "CollectionViewCommandsDelegateBase.qml";
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

    property TreeItemModel notificationModel: collectionViewBase.commands.notificationModel;

    property ListModel contextMenuModel: ListModel {}

    property alias commandsProvider: commandsProviderLocal;

    property bool hasRemoteChanges: false;
    property bool localizationChanged: false;

    signal elementsChanged();
    signal headersChanged();

    signal filterDecoratorLoaded();

    Component.onCompleted: {
        Events.subscribeEvent("FilterActivated", collectionViewContainer.filterMenuActivate);
        Events.subscribeEvent("OnLocalizationChanged", collectionViewContainer.onLocalizationChanged);
        Events.subscribeEvent("UpdateAllModels", collectionViewContainer.receiveRemoteChanges);
    }

    Component.onDestruction: {
        console.log("CollectionView onDestruction", commandId)

        Events.unSubscribeEvent("OnLocalizationChanged", collectionViewContainer.onLocalizationChanged);
        Events.unSubscribeEvent("FilterActivated", collectionViewContainer.filterMenuActivate);
        Events.unSubscribeEvent("UpdateAllModels", collectionViewContainer.receiveRemoteChanges);
    }

    onHasRemoteChangesChanged: {
        console.log("onHasRemoteChangesChanged", hasRemoteChanges)

        if (visible && hasRemoteChanges){
            setAlertPanel(alertPanelComp)
        }

        if (hasRemoteChanges){
            Events.sendEvent("HasRemoteChanges");
        }
    }

    onNotificationModelChanged: {
        if (documentManagerPtr){
            let index = documentManagerPtr.getDocumentIndexByUuid(uuid);
            if (index >= 0){
                documentManagerPtr.updateDocumentTitle(index);
            }
        }
    }

    onDocumentManagerPtrChanged: {
        if (commandsLoader.item){
            if (commandsLoader.item.documentManagerPtr !== undefined){
                commandsLoader.item.documentManagerPtr = collectionViewContainer.documentManagerPtr;
            }
        }
    }

    Keys.onPressed: {
        if (event.key === Qt.Key_Delete){
            commandsLoader.item.commandHandle("Remove");
        }
    }

    onVisibleChanged: {
        console.log("CollectionView onVisibleChanged", visible);
        if (collectionViewContainer.visible){

            if (localizationChanged){
                commandsProviderLocal.updateModel();

                localizationChanged = false;
            }

            if (hasRemoteChanges){
                setAlertPanel(alertPanelComp);
            }

            Events.subscribeEvent("FilterActivated", collectionViewContainer.filterMenuActivate);
        }
        else{
            if (hasRemoteChanges){
                setAlertPanel(undefined);
            }

            Events.unSubscribeEvent("FilterActivated", collectionViewContainer.filterMenuActivate);
        }
    }

    onCommandIdChanged: {
        console.log("CollectionView onCommandsIdChanged", collectionViewContainer.commandId);

        commandsProviderLocal.additionInputParams = collectionViewContainer.getAdditionalInputParams();
        commandsProviderLocal.commandId = collectionViewContainer.commandId;
        commandsProviderLocal.uuid = collectionViewContainer.commandId;

        collectionViewBase.commands.additionInputParams = getAdditionalInputParams()

//        collectionViewBase.commandId = collectionViewContainer.commandId;

        collectionMetaInfo.getMetaInfoGqlCommand = collectionViewContainer.commandId + "MetaInfo";

        if (commandsLoader.item){
            commandsLoader.item.commandId = collectionViewContainer.commandId;
        }
    }

    function onLocalizationChanged(language){
        console.log("CommandsDecorator onLocalizationChanged", commandId, visible);

        if (visible){
            commandsProviderLocal.updateModel();
        }
        else{
            localizationChanged = true;
        }

        fillContextMenuModel();
    }

    function getDocumentName(){
        let documentName = collectionViewContainer.documentName;

        if (notificationModel != null){
            if (notificationModel.ContainsKey("TotalCount")){
                let totalCount = notificationModel.GetData("TotalCount");

                documentName += " (" + totalCount + ")";
            }
        }

        return documentName;
    }

    function receiveRemoteChanges(){
        if (hasRemoteChanges){
            updateGui();
            hasRemoteChanges = false;

            setAlertPanel(undefined);
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

    function getAdditionalInputParams(){
        return {}
    }

    function updateGui(){
        collectionViewBase.updateModels();
    }

    function updateModel(){

    }


    function selectItem(id, name){
        console.log("CollectionView selectItem", id, name);

        let editorPath = collectionViewContainer.getEditorPath();
        let documentTypeId = collectionViewContainer.getEditorCommandId();

        if (documentTypeId === ""){
            console.error("Unable to select item documentTypeId is invalid");
            return;
        }

        if (!name){
            name = "";
        }

        if (collectionViewContainer.isUsedDocumentManager){
            if (id === ""){
                documentManagerPtr.insertNewDocument(documentTypeId);
            }
            else{
                documentManagerPtr.openDocument(id, documentTypeId);
            }
        }
        else{
            modalDialogManager.openDialog(contentDialog, {"contentId": id, "contentName": name, "contentCommandsId": documentTypeId,"contentSource": editorPath});
        }
    }

    function getEditorPath(){
        return collectionViewBase.commands.objectViewEditorPath;
    }

    function getEditorCommandId(){
        return collectionViewBase.commands.objectViewEditorCommandsId;
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
            commandsLoader.item.commandId = collectionViewContainer.commandId;

            commandsLoader.item.tableData = collectionViewBase.table;

            commandsLoader.item.collectionViewBase = collectionViewContainer;
            commandsLoader.item.commandsProvider = commandsProviderLocal;
            commandsLoader.item.documentManagerPtr = collectionViewContainer.documentManagerPtr;

            commandsLoader.item.contextMenuModel = collectionViewContainer.contextMenuModel;
        }

        onStatusChanged: {
            if (status == Loader.Error){
                console.error("Commands delegate is not loaded!");
            }
        }
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

        hasFilter: collectionViewContainer.hasFilter;

        property bool ok: collectionViewContainer.visible && collectionViewContainer.commandId !== "";
        onOkChanged: {
            console.log("CollectionViewBase onOkChanged", ok);
            if (ok){
                collectionViewBase.commandId = collectionViewContainer.commandId;
//                collectionViewContainer.updateGui();
            }
        }

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

            if (!commandsLoader.item){
                return;
            }

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

        property bool ok: collectionViewContainer.visible && commandId !== "";
        onOkChanged: {
            if (commandsModel == null){
                commandsProviderLocal.updateModel();

                return;
            }

            if (ok){
                commandsProviderLocal.updateGui()
            }
            else{
                commandsProviderLocal.clearGui();
            }
        }

        onCommandsModelChanged: {
            if (ok){
                commandsProviderLocal.updateGui()
            }

            collectionViewContainer.fillContextMenuModel();

            collectionViewContainer.onCommandsModelChanged();
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

        property string ok: collectionViewContainer.commandId !== "" && subscriptionClient.subscriptionId !== "";
        onOkChanged: {
            if (collectionViewContainer.commandId !== ""){
                let subscriptionRequestId = "On" + collectionViewContainer.commandId + "CollectionChanged"
                var query = Gql.GqlRequest("subscription", subscriptionRequestId);
                var queryFields = Gql.GqlObject("notification");
                queryFields.InsertField("Id");
                query.AddField(queryFields);

                console.log("registerSubscription", subscriptionRequestId, subscriptionId)
                subscriptionManager.registerSubscription(query, subscriptionClient);
            }
        }

        onStateChanged: {
            console.log("SubscriptionClient onStateChanged", state);

            if (state === "Ready"){
                console.log("subscriptionClient Ready", subscriptionClient.commandId);

                if (subscriptionClient.ContainsKey("data")){
                    let dataModelLocal = subscriptionClient.GetData("data")
                    if (dataModelLocal.ContainsKey("token")){
                        let accessToken = dataModelLocal.GetData("token");
                        Events.sendEvent("GetToken", function (token){
                            console.log("local token", token);

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
        if (collectionViewContainer.documentManagerPtr != null){
            collectionViewContainer.documentManagerPtr.setAlertPanel(alertPanelComp);
        }
    }

    Component {
        id: alertPanelComp;

        Rectangle {
            anchors.fill: parent;

            color: Style.selectedColor;

            Component.onCompleted: {
                Events.subscribeEvent("UpdateAllModels", updateButton.clicked);
            }

            Component.onDestruction: {
                Events.unSubscribeEvent("UpdateAllModels", updateButton.clicked);
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

                text: qsTr("This table has been modified from another computer");
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
                    Events.sendEvent("UpdateAllModels");
                }
            }
        }
    }
}

