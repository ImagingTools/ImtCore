import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0
import imtdocgui 1.0

Item {
    id: administrationContainer;

    anchors.fill: parent;

    property alias multiPageView: multiPageView;
    signal multiPageUpdated();

    property string productId: context.appId;
    property var documentManager: MainDocumentManager.getDocumentManager("Administration");

    property TreeItemModel rolesModel;
    property TreeItemModel usersModel;
    property TreeItemModel groupsModel;

    function getHeaders(){
        if (administrationContainer.productId === ""){
            console.error("Unable to get additional parameters. Product-ID is empty");
            return {};
        }

        let obj = {}
        obj["ProductId"] = administrationContainer.productId;

        return obj;
    }

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", administrationContainer.onLocalizationChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", administrationContainer.onLocalizationChanged);
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
        color: Style.backgroundColor2;
    }

    Component {
        id: roleCollectionComp;

        RoleCollectionView {
            productId: administrationContainer.productId;
            documentManager: administrationContainer.documentManager;
            rolesModel: administrationContainer.rolesModel;

            function getHeaders(){
                return administrationContainer.getHeaders()
            }
        }
    }

    Component {
        id: userCollectionComp;

        UserCollectionView {
            productId: administrationContainer.productId;
            documentManager: administrationContainer.documentManager;
            rolesModel: administrationContainer.rolesModel;
            groupsModel: administrationContainer.groupsModel;

            function getHeaders(){
                return administrationContainer.getHeaders()
            }
        }
    }

    Component {
        id: userGroupCollectionComp;

        UserGroupCollectionView {
            productId: administrationContainer.productId;
            documentManager: administrationContainer.documentManager;
            rolesModel: administrationContainer.rolesModel;
            groupsModel: administrationContainer.groupsModel;
            usersModel: administrationContainer.usersModel;

            function getHeaders(){
                return administrationContainer.getHeaders()
            }
        }
    }

    MultiPageView {
        id: multiPageView;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;

        Component.onCompleted: {
            updateModel();
        }

        function updateModel(){
            multiPageView.clear();

            let ok = PermissionsController.checkPermission("ViewRoles");
            if (ok){
                roleCollectionDataProvider.productId = administrationContainer.productId;
                roleCollectionDataProvider.updateModel();

                multiPageView.addPage("Roles", qsTr("Roles"), roleCollectionComp);
            }
            else{
                console.warn("Role collection cannot be displayed. Error: Permission denied");
            }

            ok = PermissionsController.checkPermission("ViewUsers");
            if (ok){
                userCollectionDataProvider.updateModel();
                multiPageView.addPage("Users", qsTr("Users"), userCollectionComp);
            }
            else{
                console.warn("User collection cannot be displayed. Error: Permission denied");
            }

            ok = PermissionsController.checkPermission("ViewGroups");
            if (ok){
                groupCollectionDataProvider.updateModel();
                multiPageView.addPage("Groups", qsTr("Groups"), userGroupCollectionComp);
            }
            else{
                console.warn("Group collection cannot be displayed. Error: Permission denied");
            }

            multiPageView.currentIndex = 0;
            administrationContainer.multiPageUpdated();
        }
    }

    RoleCollectionDataProvider {
        id: roleCollectionDataProvider;
        productId: administrationContainer.productId;

        onModelUpdated: {
            administrationContainer.rolesModel = collectionModel;
        }

        function getHeaders(){
            return administrationContainer.getHeaders()
        }
    }

    UserCollectionDataProvider {
        id: userCollectionDataProvider;

        onModelUpdated: {
            administrationContainer.usersModel = collectionModel;
        }

        function getHeaders(){
            return administrationContainer.getHeaders()
        }
    }

    GroupCollectionDataProvider {
        id: groupCollectionDataProvider;
        onModelUpdated: {
            administrationContainer.groupsModel = collectionModel;
        }
        function getHeaders(){
            return administrationContainer.getHeaders();
        }
    }
}
