import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0

Item {
    id: container;

    anchors.fill: parent;

    property alias multiPageView: multiPageView;
    signal multiPageUpdated();

    property string productId: context.appName;

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", container.onLocalizationChanged);

        CachedGroupCollection.updateModel();
        CachedUserCollection.updateModel();
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
        }
    }

    Component {
        id: userCollectionComp;

        UserCollectionView {
            productId: container.productId;
        }
    }

    Component {
        id: userGroupCollectionComp;

        UserGroupCollectionView {
            productId: container.productId;
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

            ok = PermissionsController.checkPermission("ViewUsers");
            if (ok){
                multiPageView.addPage("Users", qsTr("Users"), userCollectionComp);
            }

            ok = PermissionsController.checkPermission("ViewGroups");
            if (ok){
                multiPageView.addPage("Groups", qsTr("Groups"), userGroupCollectionComp);
            }

            multiPageView.currentIndex = 0;
            container.multiPageUpdated();
        }
    }
}
