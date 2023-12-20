import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtqml 1.0
import imtlicgui 1.0

Item {
    id: collectionStructureContainer;

    property string uuid;
    property DocumentManager documentManagerPtr: null;
    property string documentName;

    property int contentMargins: 0;

    property alias baseCollectionView: collectionStructureBase;

    property bool isUsedDocumentManager: true;
    property bool visibleMetaInfo: true;
    property bool hasFilter: true;

    property string commandId;
    property string documentTypeId;

    property string commandsDelegatePath: "CollectionViewCommandsDelegateBase.qml";
    property alias commandsDelegate: commandsDelegateLoader.item;

    property alias treeView: collectionStructureBase.treeView;

    property alias filterMenu: collectionStructureBase.filterMenu;
    property alias filterMenuVisible: collectionStructureBase.filterMenuVisible;
    property alias modelFilter: collectionStructureBase.modelFilter;
    // property alias defaultSortHeaderIndex: collectionStructureBase.defaultSortHeaderIndex;
    // property alias defaultOrderType: collectionStructureBase.defaultOrderType;

    property TreeItemModel notificationModel: collectionStructureBase.commands.notificationModel;

    property ListModel contextMenuModel: ListModel {}

    property alias commandsProvider: commandsProviderLocal;

    property bool hasRemoteChanges: false;
    property bool localizationChanged: false;

    signal elementsChanged();
    signal headersChanged();

    signal filterDecoratorLoaded();

    Component.onCompleted: {
        Events.subscribeEvent("FilterActivated", collectionStructureContainer.filterMenuActivate);
        Events.subscribeEvent("OnLocalizationChanged", collectionStructureContainer.onLocalizationChanged);
        Events.subscribeEvent("UpdateAllModels", collectionStructureContainer.receiveRemoteChanges);
    }

    Component.onDestruction: {
        console.log("CollectionView onDestruction", commandId)

        Events.unSubscribeEvent("OnLocalizationChanged", collectionStructureContainer.onLocalizationChanged);
        Events.unSubscribeEvent("FilterActivated", collectionStructureContainer.filterMenuActivate);
        Events.unSubscribeEvent("UpdateAllModels", collectionStructureContainer.receiveRemoteChanges);
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
        if (commandsDelegateLoader.item){
            if (commandsDelegateLoader.item.documentManagerPtr !== undefined){
                commandsDelegateLoader.item.documentManagerPtr = collectionStructureContainer.documentManagerPtr;
            }
        }
    }

    Keys.onPressed: {
        if (event.key === Qt.Key_Delete){
            commandsDelegateLoader.item.commandHandle("Remove");
        }
    }

    onVisibleChanged: {
        console.log("CollectionView onVisibleChanged", visible);
        if (collectionStructureContainer.visible){

            if (localizationChanged){
                commandsProviderLocal.updateModel();

                localizationChanged = false;
            }

            if (hasRemoteChanges){
                setAlertPanel(alertPanelComp);
            }

            Events.subscribeEvent("FilterActivated", collectionStructureContainer.filterMenuActivate);
        }
        else{
            if (hasRemoteChanges){
                setAlertPanel(undefined);
            }

            Events.unSubscribeEvent("FilterActivated", collectionStructureContainer.filterMenuActivate);
        }
    }

    onCommandIdChanged: {
        console.log("CollectionView onCommandsIdChanged", collectionStructureContainer.commandId);

        commandsProviderLocal.additionInputParams = collectionStructureContainer.getAdditionalInputParams();
        commandsProviderLocal.commandId = collectionStructureContainer.commandId;
        commandsProviderLocal.uuid = collectionStructureContainer.commandId;

        collectionStructureBase.commands.additionInputParams = getAdditionalInputParams()

//        collectionStructureBase.commandId = collectionViewContainer.commandId;

        collectionMetaInfo.getMetaInfoGqlCommand = collectionStructureContainer.commandId + "MetaInfo";

        if (commandsDelegateLoader.item){
            commandsDelegateLoader.item.commandId = collectionStructureContainer.commandId;
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
        let documentName = collectionStructureContainer.documentName;

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
        if (collectionStructureContainer.hasFilter){
            collectionStructureContainer.filterMenuVisible = !collectionStructureContainer.filterMenuVisible;
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
        collectionStructureBase.updateModels();
    }

    function updateModel(){

    }


    function selectItem(id, name){
        console.log("CollectionView selectItem", id, name);

        let editorPath = collectionStructureContainer.getEditorPath();
        let documentTypeId = collectionStructureContainer.getEditorCommandId();

        if (documentTypeId === ""){
            console.error("Unable to select item documentTypeId is invalid");
            return;
        }

        if (!name){
            name = "";
        }

        if (collectionStructureContainer.isUsedDocumentManager){
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
        return collectionStructureBase.commands.objectViewEditorPath;
    }

    function getEditorCommandId(){
        return collectionStructureBase.commands.objectViewEditorCommandsId;
    }

    // function getSelectedIds(){
    //     return collectionStructureBase.treeView.getSelectedIds();
    // }

    // function getSelectedIndexes(){
    //     return collectionStructureBase.table.getSelectedIndexes();
    // }

    // function getSelectedNames(){
    //     return collectionStructureBase.table.getSelectedNames();
    // }

    Component {
        id: contentDialog;
        Item {
            id: content;
            property Item root: null;
            property Item rootItem:collectionStructureContainer;
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
        id: commandsDelegateLoader;

        Component.onCompleted: {
            console.log("commandsDelegateLoader.source", parent.commandsDelegatePath);
            commandsDelegateLoader.source = parent.commandsDelegatePath;
        }

        onLoaded: {
            commandsDelegateLoader.item.commandId = collectionStructureContainer.commandId;

            // commandsDelegateLoader.item.tableData = collectionStructureBase.table;

            commandsDelegateLoader.item.collectionStructureBase = collectionStructureContainer;
            commandsDelegateLoader.item.commandsProvider = commandsProviderLocal;
            commandsDelegateLoader.item.documentManagerPtr = collectionStructureContainer.documentManagerPtr;

            commandsDelegateLoader.item.contextMenuModel = collectionStructureContainer.contextMenuModel;
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

    CollectionStructureBase {
        id: collectionStructureBase;

        anchors.right: collectionMetaInfo.left;
        anchors.left: parent.left;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.margins: parent.contentMargins;

        hasFilter: collectionStructureContainer.hasFilter;

        property bool ok: collectionStructureContainer.visible && collectionStructureContainer.commandId !== "";
        onOkChanged: {
            console.log("collectionStructureBase onOkChanged", ok);
            if (ok){
                collectionStructureBase.commandId = collectionStructureContainer.commandId;
//                collectionViewContainer.updateGui();
            }
        }

        onFilterDecoratorLoaded: {
            collectionStructureContainer.filterDecoratorLoaded();
        }

        onSelectionChanged: {
            if (collectionMetaInfo.visible){
                if (selection.length === 1){
                    let itemIds = collectionStructureContainer.getSelectedIds();
                    let itemNames = collectionStructureContainer.getSelectedNames();

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
            console.log("collectionStructureBase onItemSelected", commandsDelegateLoader, commandsDelegateLoader.item);

            if (id == ""){
                commandsDelegateLoader.item.commandHandle("New");
            }
            else{
                commandsDelegateLoader.item.commandHandle("Edit");
            }
        }

        onElementsChanged: {
            collectionStructureContainer.elementsChanged();
        }

    }

    CommandsProvider {
        id: commandsProviderLocal;

        property bool ok: collectionStructureContainer.visible && commandId !== "";
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

            collectionStructureContainer.fillContextMenuModel();

            collectionStructureContainer.onCommandsModelChanged();
        }
    }

    function onCommandsModelChanged(){}

    MetaInfo {
        id: collectionMetaInfo;

        anchors.right: parent.right;

        width: visible ? 200 : 1;
        height: parent.height;

        visible: collectionStructureContainer.visibleMetaInfo;
    }

    SubscriptionClient {
        id: subscriptionClient;

        property string ok: collectionStructureContainer.commandId !== "" && subscriptionClient.subscriptionId !== "";
        onOkChanged: {
            if (collectionStructureContainer.commandId !== ""){
                let subscriptionRequestId = "On" + collectionStructureContainer.commandId + "CollectionChanged"
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
                                collectionStructureContainer.updateGui();
                            }
                            else{
                                collectionStructureContainer.hasRemoteChanges = true;
                            }
                        });
                    }
                }
            }
        }
    }

    function setAlertPanel(alertPanelComp){
        if (collectionStructureContainer.documentManagerPtr != null){
            collectionStructureContainer.documentManagerPtr.setAlertPanel(alertPanelComp);
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

