import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0

Rectangle {
    id: container;

    anchors.fill: parent;

    color: Style.backgroundColor;

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", container.onLocalizationChanged);
    }

    Component.onDestruction: {
        console.log("Administration onDestruction");

        Events.unSubscribeEvent("OnLocalizationChanged", container.onLocalizationChanged);
    }

    function onLocalizationChanged(language){
        console.log("Administration onLocalizationChanged", language);

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

    Component {
        id: roleCollectionComp;

        RoleCollectionView {
        }
    }

    Component {
        id: userCollectionComp;

        UserCollectionView {
        }
    }

    Component {
        id: userGroupCollectionComp;

        UserGroupCollectionView {
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

            multiPageView.addPage("Roles", qsTr("Roles"), roleCollectionComp);
            multiPageView.addPage("Users", qsTr("Users"), userCollectionComp);
            multiPageView.addPage("Groups", qsTr("Groups"), userGroupCollectionComp);

            multiPageView.currentIndex = 0;
        }
    }
}
