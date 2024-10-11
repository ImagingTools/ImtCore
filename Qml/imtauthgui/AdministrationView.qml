import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0
import imtdocgui 1.0

Item {
    id: container;

    anchors.fill: parent;

    property alias multiPageView: multiPageView;
    signal multiPageUpdated();

    property string productId: context.appName;
    property var documentManager: MainDocumentManager.getDocumentManager("Administration");

    function getHeaders(){
        if (container.productId === ""){
            console.error("Unable to get additional parameters. Product-ID is empty");
            return null;
        }

        let obj = {}
        obj["ProductId"] = container.productId;

        return obj;
    }

    Component.onCompleted: {
        console.log("AdministrationView.qml onCompleted");
        Events.subscribeEvent("OnLocalizationChanged", container.onLocalizationChanged);

        CachedGroupCollection.updateModel();
        CachedUserCollection.updateModel();

        CachedRoleCollection.productId = container.productId;
        CachedRoleCollection.updateModel();
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", container.onLocalizationChanged);

        CachedGroupCollection.clearModel();
        CachedUserCollection.clearModel();
        CachedRoleCollection.clearModel();
    }

    function onLocalizationChanged(language){
        let rolesIndex = multiPageView.getIndexById("Roles");
        if (rolesIndex >= 0){
            multiPageView.pagesModel.setProperty(rolesIndex, "Name", qsTr("Roles"))
        }

        let usersIndex = multiPageView.getIndexById("Users");
        if (usersIndex >= 0){
            multiPageView.pagesModel.setProperty(usersIndex, "Name", qsTr("Users"))
        }

        let groupsIndex = multiPageView.getIndexById("Groups");
        if (groupsIndex >= 0){
            multiPageView.pagesModel.setProperty(groupsIndex, "Name", qsTr("Groups"))
        }
    }

    Rectangle {
        anchors.fill: parent;
        color: Style.backgroundColor2;
    }

    Component {
        id: roleCollectionComp;

        RoleCollectionView {
            productId: container.productId;
            documentManager: container.documentManager;

            function getHeaders(){
                return container.getHeaders()
            }

        }
    }

    Component {
        id: userCollectionComp;

        UserCollectionView {
            productId: container.productId;
            documentManager: container.documentManager;

            function getHeaders(){
                return container.getHeaders()
            }
        }
    }

    Component {
        id: userGroupCollectionComp;

        UserGroupCollectionView {
            productId: container.productId;
            documentManager: container.documentManager;

            function getHeaders(){
                return container.getHeaders()
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
                multiPageView.addPage("Roles", qsTr("Roles"), roleCollectionComp);
            }
            else{
                console.warn("Role collection cannot be displayed. Error: Permission denied");
            }

            ok = PermissionsController.checkPermission("ViewUsers");
            if (ok){
                multiPageView.addPage("Users", qsTr("Users"), userCollectionComp);
            }
            else{
                console.warn("User collection cannot be displayed. Error: Permission denied");
            }

            ok = PermissionsController.checkPermission("ViewGroups");
            if (ok){
                multiPageView.addPage("Groups", qsTr("Groups"), userGroupCollectionComp);
            }
            else{
                console.warn("Group collection cannot be displayed. Error: Permission denied");
            }

            multiPageView.currentIndex = 0;
            container.multiPageUpdated();
        }
    }
}
