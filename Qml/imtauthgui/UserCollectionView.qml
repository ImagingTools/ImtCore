import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0
import imtauthUsersSdl 1.0

RemoteCollectionView {
    id: userCollectionViewContainer;

    collectionId: "Users";
    visibleMetaInfo: true;

    property string productId;
    property var documentManager: null;

    commandsDelegateComp: Component {DocumentCollectionViewDelegate {
            collectionView: userCollectionViewContainer;

            documentTypeId: "User";
            viewTypeId: "UserEditor";

            function updateItemSelection(selectedItems){
                if (userCollectionViewContainer.commandsController){
                    let isEnabled = selectedItems.length === 1;
                    if (!isEnabled){
                        userCollectionViewContainer.commandsController.setCommandIsEnabled("Remove", false);
                        userCollectionViewContainer.commandsController.setCommandIsEnabled("Edit", false);
                    }
                    else{
                        let index = selectedItems[0];
                        let systemIds = userCollectionViewContainer.table.elements.getData("SystemId", index);

                        userCollectionViewContainer.commandsController.setCommandIsEnabled("Remove", isEnabled && !systemIds.includes("Ldap"));
                        userCollectionViewContainer.commandsController.setCommandIsEnabled("Edit", isEnabled);
                    }
                }
            }
        }
    }

    filterMenu.decorator: filterDecoratorComp;

    function handleSubscription(dataModel){
        userCollectionViewContainer.doUpdateGui();
    }

    Component {
        id: filterDecoratorComp;

        DecoratorBase {
            id: decoratorBase;

            width: baseElement ? baseElement.width: 0;
            height: 40;

            Row {
                id: content;
                anchors.left: parent.left;
                anchors.verticalCenter: parent.verticalCenter;
                spacing: Style.size_mainMargin;

                TreeItemModel {
                    id: systemInfoModel;

                    Component.onCompleted: {
                        updateModel();
                    }

                    function updateModel(){
                        systemInfoModel.clear();

                        let index = systemInfoModel.insertNewItem();
                        systemInfoModel.setData("Id", "All", index)
                        systemInfoModel.setData("Name", qsTr("All System"), index)

                        index = systemInfoModel.insertNewItem();
                        systemInfoModel.setData("Id", "Internal", index)
                        systemInfoModel.setData("Name", qsTr("Internal"), index)

                        index = systemInfoModel.insertNewItem();
                        systemInfoModel.setData("Id", "Ldap", index)
                        systemInfoModel.setData("Name", qsTr("LDAP"), index)

                        systemComboBox.model = systemInfoModel;
                    }
                }

                ComboBox {
                    id: systemComboBox;

                    width: 250;
                    height: filtermenu.height;
                    currentIndex: 0;
                    onCurrentIndexChanged: {
                        if (currentIndex == 1){
                            userCollectionViewContainer.collectionFilter.addAdditionalFilter("SystemId", "")
                        }
                        else if (currentIndex > 1){
                            let id = systemComboBox.model.getData("Id", currentIndex);
                            userCollectionViewContainer.collectionFilter.addAdditionalFilter("SystemId", id)
                        }
                        else{
                            userCollectionViewContainer.collectionFilter.removeFilterById("SystemId");
                        }

                        userCollectionViewContainer.doUpdateGui();
                    }
                }
            }

            FilterPanelDecorator {
                id: filtermenu
                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: parent.right;
                baseElement: decoratorBase.baseElement;
                width: 325;
            }
        }
    }

    dataControllerComp:
        Component {
        CollectionRepresentation {
            collectionId: userCollectionViewContainer.collectionId;
            Component.onCompleted: {
                additionalFieldIds.push("SystemId");
            }

            function getAdditionalInputParams(){
                let obj = {}
                obj["ProductId"] = userCollectionViewContainer.productId;
                return obj;
            }
        }
    }

    function onLocalizationChanged(language){
        userCollectionViewContainer.dataController.updateHeaders();
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", userCollectionViewContainer.onLocalizationChanged);
    }

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", userCollectionViewContainer.onLocalizationChanged);
        CachedRoleCollection.updateModel();
        CachedGroupCollection.updateModel();

        collectionFilter.setSortingOrder("ASC");
        collectionFilter.setSortingInfoId("Name");

        // let documentManager = MainDocumentManager.getDocumentManager("Administration");
        if (documentManager){
            userCollectionViewContainer.commandsDelegate.documentManager = documentManager;

            documentManager.registerDocumentView("User", "UserEditor", userDocumentComp);
            documentManager.registerDocumentDataController("User", dataControllerComp);
        }

        table.rowDelegate = tableRowDelegateBaseComp;
    }

    Component {
        id: tableRowDelegateBaseComp;

        TableRowDelegate {
            tableItem: userCollectionViewContainer.table;
            width: userCollectionViewContainer.table.width;
            minHeight: userCollectionViewContainer.table.itemHeight;
            readOnly: userCollectionViewContainer.table.readOnly;
            //            color: selected ? Style.selectedColor : model.SystemId !== "" ? "red" : "transparent";
        }
    }

    onHeadersChanged: {
        let rolesIndex = table.getHeaderIndex("Roles");
        if (rolesIndex >= 0){
            table.setColumnContentComponent(rolesIndex, dataComp);
        }

        let groupsIndex = table.getHeaderIndex("Groups");
        if (groupsIndex >= 0){
            table.setColumnContentComponent(groupsIndex, groupsContentComp);
        }
    }

    Component {
        id: dataComp;

        TableCellDelegateBase {
            id: cellDelegate

            height: 20;

            clip: true;

            onRowIndexChanged: {
                if (!rowDelegate){
                    return
                }

                let username = userCollectionViewContainer.table.elements.getData("Name", rowIndex);
                let roles = userCollectionViewContainer.table.elements.getData("Roles", rowIndex);
                arrowButton.visible = roles !== "";
                if (roles !== ""){
                    let roleList = roles.split(';');
                    name.text = qsTr("View roles") + "(" + roleList.length +  ")";

                    let result = "<b><u>" + username + "</u></b> " + qsTr("has been assigned") + " <b>" + roleList.length + "</b> " + qsTr("roles:");
                    for (let i = 0; i < roleList.length; i++){
                        result += "<p>" + roleList[i] + "</p>";
                    }

                    arrowButton.tooltipText = result;
                }
                else{
                    name.text = qsTr("No roles");
                }
            }

            Text {
                id: name;

                anchors.verticalCenter: parent.verticalCenter;

                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamily;
                color: Style.textColor;

                elide: Text.ElideRight;
            }

            ToolButton {
                id: arrowButton;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: name.right;
                anchors.leftMargin: 5;

                width: 15;
                height: width;

                iconSource: "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal);

                decorator: Component {
                    ToolButtonDecorator {
                        color: "transparent";
                        icon.width: 16;
                    }
                }
            }
        }
    }

    Component {
        id: groupsContentComp;

        TableCellDelegateBase {
            id: cellDelegate

            onRowIndexChanged: {
                if (!rowDelegate){
                    return
                }

                let username = userCollectionViewContainer.table.elements.getData("Name", rowIndex);
                let groups = userCollectionViewContainer.table.elements.getData("Groups", rowIndex);
                arrowButton.visible = groups !== "";
                if (groups !== ""){
                    let groupList = groups.split(';');
                    name2.text =  qsTr("View groups") + "(" + groupList.length +  ")";

                    let result = "<b><u>" + username + "</u></b> " + qsTr("is included into") + " <b>" + groupList.length + "</b> " + qsTr("groups") + ":";
                    for (let i = 0; i < groupList.length; i++){
                        result += "<p>" + groupList[i] + "</p>";
                    }
                    arrowButton.tooltipText = result;
                }
                else{
                    name2.text = qsTr("No groups");
                }
            }

            Item {
                anchors.fill: parent;

                clip: true;

                Text {
                    id: name2;

                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.left: parent.left
                    anchors.leftMargin: Style.size_mainMargin;

                    font.pixelSize: Style.fontSize_common;
                    font.family: Style.fontFamily;
                    color: Style.textColor;

                    elide: Text.ElideRight;
                }

                ToolButton {
                    id: arrowButton;

                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.left: name2.right;
                    anchors.leftMargin: Style.size_smallMargin;

                    width: 15;
                    height: width;

                    visible: name2.text !== "";

                    iconSource: "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal);

                    decorator: Component {
                        ToolButtonDecorator {
                            color: "transparent";
                            icon.width: 16;
                        }
                    }

                }
            }
        }
    }

    property TreeItemModel rolesModel: CachedRoleCollection.collectionModel;

    property TreeItemModel groupsModel: CachedGroupCollection.collectionModel;
    Component {
        id: userDocumentComp;

        UserView {
            id: userEditor;

            rolesModel: userCollectionViewContainer.rolesModel;
            groupsModel: userCollectionViewContainer.groupsModel;
            productId: userCollectionViewContainer.productId;

            commandsDelegateComp: Component {ViewCommandsDelegateBase {
                    view: userEditor;
                }
            }

            commandsControllerComp: Component {CommandsPanelController {
                    commandId: "User";
                    uuid: userEditor.viewId;
                    commandsView: userEditor.commandsView;
                }
            }
        }
    }

    Component {
        id: dataControllerComp;

        GqlRequestDocumentDataController {
            id: requestDocumentDataController

            gqlGetCommandId: ImtauthUsersSdlCommandIds.s_userItem;
            gqlUpdateCommandId: ImtauthUsersSdlCommandIds.s_userUpdate;
            gqlAddCommandId: ImtauthUsersSdlCommandIds.s_userAdd;

            Component.onCompleted: {
                getRequestInputParam.InsertField(UserItemInputTypeMetaInfo.s_productId, userCollectionViewContainer.productId);
                addRequestInputParam.InsertField(UserItemInputTypeMetaInfo.s_productId, userCollectionViewContainer.productId);
                updateRequestInputParam.InsertField(UserItemInputTypeMetaInfo.s_productId, userCollectionViewContainer.productId);
            }

            documentModelComp: Component {
                UserData {}
            }

            payloadModel: UserDataPayload {
                onFinished: {
                    requestDocumentDataController.documentModel = m_userData;
                }
            }

            function getDocumentName(){
                let prefixName = qsTr("Users");

                if (documentName !== ""){
                    return prefixName + " / " + documentName;
                }

                return prefixName + " / " + qsTr("New User");
            }
        }
    }
}


