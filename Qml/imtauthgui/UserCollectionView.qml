import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0


RemoteCollectionView {
    id: userCollectionViewContainer;

    collectionId: "Users";
    visibleMetaInfo: true;

    dataControllerComp: Component {CollectionRepresentation {
        collectionId: "Users";
    }
    }

    commandsControllerComp: Component {CommandsRepresentationProvider {
        commandId: "Users";
        uuid: userCollectionViewContainer.viewId;
    }
    }

    commandsDelegateComp: Component {DocumentCollectionViewDelegate {
        collectionView: userCollectionViewContainer;

        documentTypeId: "User";
        viewTypeId: "UserEditor";
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

        let documentManager = MainDocumentManager.getDocumentManager("Administration");
        if (documentManager){
            userCollectionViewContainer.commandsDelegate.documentManager = documentManager;

            documentManager.registerDocumentView("User", "UserEditor", userDocumentComp);
            documentManager.registerDocumentDataController("User", dataControllerComp);
        }
    }

    onHeadersChanged: {
        let rolesIndex = table.getHeaderIndex("Roles");
        let groupsIndex = table.getHeaderIndex("Groups");

        table.setColumnContentComponent(rolesIndex, dataComp);
        table.setColumnContentComponent(groupsIndex, groupsContentComp);
    }

    Component {
        id: dataComp;

        Item {
            id: item;

            height: 20;

            clip: true;

            property Item tableCellDelegate: null;

            onTableCellDelegateChanged: {
                if (item.tableCellDelegate != null){
                    let username = userCollectionViewContainer.table.elements.GetData("Name", tableCellDelegate.rowIndex);
                    let roles = userCollectionViewContainer.table.elements.GetData("Roles", tableCellDelegate.rowIndex);
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

                onClicked: {
                    console.log("Roles Button onClicked");
                }
            }
        }
    }

    Component {
        id: groupsContentComp;

        Item {
            id: item2;

            property Item tableCellDelegate: null;

            Component.onCompleted: {
                let loader = parent;
                let tableCellDelegate = loader.parent;

                if (tableCellDelegate.mainMouseArea){
                    tableCellDelegate.mainMouseArea.hoverEnabled = false;
                }
            }

            onTableCellDelegateChanged: {
                if (item2.tableCellDelegate != null){
                    let username = userCollectionViewContainer.table.elements.GetData("Name", tableCellDelegate.rowIndex);
                    let groups = userCollectionViewContainer.table.elements.GetData("Groups", tableCellDelegate.rowIndex);
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
            }

            Text {
                id: name2;

                anchors.verticalCenter: parent.verticalCenter;

                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamily;
                color: Style.textColor;

                elide: Text.ElideRight;
            }

            ToolButton {
                id: arrowButton;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: name2.right;
                anchors.leftMargin: 5;

                width: 15;
                height: width;

                visible: name2.text !== "";

                iconSource: "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal);
            }
        }
    }

    property TreeItemModel rolesModel: CachedRoleCollection.rolesModel;

    property TreeItemModel groupsModel: CachedGroupCollection.collectionModel;
    Component {
        id: userDocumentComp;

        UserView {
            id: userEditor;

            rolesModel: userCollectionViewContainer.rolesModel;
            groupsModel: userCollectionViewContainer.groupsModel;

            commandsDelegateComp: Component {ViewCommandsDelegateBase {
                view: userEditor;
            }
            }

            commandsControllerComp: Component {CommandsRepresentationProvider {
                commandId: "User";
                uuid: userEditor.viewId;
            }
            }
        }
    }

    Component {
        id: dataControllerComp;

        GqlDocumentDataController {
            gqlGetCommandId: "UserItem";
            gqlUpdateCommandId: "UserUpdate";
            gqlAddCommandId: "UserAdd";

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


