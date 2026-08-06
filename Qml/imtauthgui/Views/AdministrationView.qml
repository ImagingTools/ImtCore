import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtqmlutils 1.0
import imtauthgui 1.0

Item {
    id: administrationContainer;
    objectName: "AdministrationView"

    anchors.fill: parent;

    property alias multiPageView: multiPageView;
    signal multiPageUpdated();

	property string productId: AuthorizationController.productId
    property GqlBasedUserAdministrationApiClient apiClient: GqlBasedUserAdministrationApiClient {
        productId: administrationContainer.productId
    }

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", administrationContainer.onLocalizationChanged);
        apiClient.setRolePermissionsTenantId(apiClient.tenantId);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", administrationContainer.onLocalizationChanged);
    }

    Connections {
        target: administrationContainer.apiClient
        function onTenantIdChanged() {
            administrationContainer.apiClient.setRolePermissionsTenantId(administrationContainer.apiClient.tenantId);
        }
    }

    function closeAllDocumentsForManager(manager){
        if (!manager){
            return;
        }

        let ids = manager.getOpenedDocumentIds();
        for (let i = 0; i < ids.length; ++i){
            manager.closeDocument(ids[i]);
        }
    }

    function onLocalizationChanged(language){
        let rolesIndex = multiPageView.getIndexById("Roles");
        if (rolesIndex >= 0){
            multiPageView.pagesModel.setProperty(rolesIndex, "name", qsTr("Roles"))
        }

        let usersIndex = multiPageView.getIndexById("Users");
        if (usersIndex >= 0){
            multiPageView.pagesModel.setProperty(usersIndex, "name", qsTr("Users"))
        }

        let groupsIndex = multiPageView.getIndexById("Groups");
        if (groupsIndex >= 0){
            multiPageView.pagesModel.setProperty(groupsIndex, "name", qsTr("Groups"))
        }
    }

    Rectangle {
        anchors.fill: parent;
        color: Style.baseColor;
    }

    Component {
        id: roleCollectionComp;

        MultiDocumentCollectionView {
            documentManager: administrationContainer.apiClient.roleDocumentManager;
            contentColor: Style.baseColor;

            Component.onCompleted: {
                setCollectionViewComp(qsTr("Roles"), roleListComp)
            }
        }
    }

    Component {
        id: roleListComp;

        RoleCollectionView {
            productId: administrationContainer.productId;
            documentManager: administrationContainer.apiClient.roleDocumentManager;
            contentColor: Style.baseColor;
        }
    }

    Component {
        id: userCollectionComp;

        MultiDocumentCollectionView {
            documentManager: administrationContainer.apiClient.userDocumentManager;
            contentColor: Style.baseColor;

            Component.onCompleted: {
                setCollectionViewComp(qsTr("Users"), userListComp)
            }
        }
    }

    Component {
        id: userListComp;

        UserCollectionView {
            productId: administrationContainer.productId;
            documentManager: administrationContainer.apiClient.userDocumentManager;
            contentColor: Style.baseColor;
        }
    }

    Component {
        id: userGroupCollectionComp;

        MultiDocumentCollectionView {
            documentManager: administrationContainer.apiClient.groupDocumentManager;
            contentColor: Style.baseColor;

            Component.onCompleted: {
                setCollectionViewComp(qsTr("Groups"), userGroupListComp)
            }
        }
    }

    Component {
        id: userGroupListComp;

        UserGroupCollectionView {
            productId: administrationContainer.productId;
            documentManager: administrationContainer.apiClient.groupDocumentManager;
            contentColor: Style.baseColor;
        }
    }

    MultiPageView {
        id: multiPageView;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;
        
        NavigableItem {
            parentSegment: "Administration"
            paths: ["Roles", "Users", "Groups"]
            onActivated: {
                let index = paths.indexOf(matchedPath)
                multiPageView.block = true
                multiPageView.currentIndex = index

                administrationContainer.closeAllDocumentsForManager(administrationContainer.apiClient.roleDocumentManager)
                administrationContainer.closeAllDocumentsForManager(administrationContainer.apiClient.userDocumentManager)
                administrationContainer.closeAllDocumentsForManager(administrationContainer.apiClient.groupDocumentManager)

                if (restPath.length >= 2){
                    let documentTypeId = restPath[0]
                    let documentId = restPath[1]

                    let manager = null
                    if (matchedPath === "Roles") manager = administrationContainer.apiClient.roleDocumentManager
                    else if (matchedPath === "Users") manager = administrationContainer.apiClient.userDocumentManager
                    else if (matchedPath === "Groups") manager = administrationContainer.apiClient.groupDocumentManager

                    if (manager){
                        manager.openDocument(documentTypeId, documentId)
                    }
                }

                multiPageView.block = false
            }
        }

        Component.onCompleted: {
            updateModel();
        }

        property bool block: false
        onCurrentIndexChanged: {
            if (currentIndex >= 0 && multiPageView.pagesModel.count > currentIndex){
                let pageId = multiPageView.pagesModel.get(currentIndex).id
                if (!block){
                    NavigationController.push("Administration/" + pageId)
                }
            }
        }

        function updateModel(){
            multiPageView.clear();

            let ok = PermissionsController.checkPermission("ViewRoles");
            if (ok){
                multiPageView.addPage("Roles", qsTr("Roles"), roleCollectionComp, "Icons/Role");
            }
            else{
                console.warn("Role collection cannot be displayed. Error: Permission denied");
            }

            ok = PermissionsController.checkPermission("ViewUsers");
            if (ok){
                multiPageView.addPage("Users", qsTr("Users"), userCollectionComp, "Icons/Account");
            }
            else{
                console.warn("User collection cannot be displayed. Error: Permission denied");
            }

            ok = PermissionsController.checkPermission("ViewGroups");
            if (ok){
                multiPageView.addPage("Groups", qsTr("Groups"), userGroupCollectionComp, "Icons/MultipleUser");
            }
            else{
                console.warn("Group collection cannot be displayed. Error: Permission denied");
            }

            multiPageView.currentIndex = 0;
            administrationContainer.multiPageUpdated();
        }
    }
}
