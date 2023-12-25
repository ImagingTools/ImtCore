import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Rectangle {
    id: container;

    anchors.fill: parent;

    color: Style.backgroundColor;

    property Item documentManagerPtr: null;

    function addHeader(header){
        headerText.text += "/ " + header;
    }

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

        if (multiPageView.selectedIndex >= 0){
            headerText.text = qsTr("Administration") + " / " + multiPageView.pagesModel.get(multiPageView.selectedIndex).Name;
        }
    }

    Row {
        id: header;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.leftMargin: 10;

        height: 40

        spacing: 20;

        Text {
            id: headerText;

            anchors.verticalCenter: parent.verticalCenter;

            font.pixelSize: Style.fontSize_title;
            font.family: Style.fontFamily;

            color: Style.titleColor;
        }
    }

    Component {
        id: roleCollectionComp;

        RoleCollectionView {
            documentManagerPtr: container.documentManagerPtr;
        }
    }

    Component {
        id: userCollectionComp;

        UserCollectionView {
            documentManagerPtr: container.documentManagerPtr;
        }
    }

    Component {
        id: userGroupCollectionComp;

        UserGroupCollectionView {
            documentManagerPtr: container.documentManagerPtr;
        }
    }

    MultiPageView {
        id: multiPageView;

        anchors.top: header.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;

        Component.onCompleted: {
            updateModel();
        }

        onSelectedIndexChanged: {
            console.log("onSelectedIndexChanged", selectedIndex);

            if (selectedIndex >= 0){
                headerText.text = qsTr("Administration") + " / " + pagesModel.get(selectedIndex).Name;
            }
        }

        function updateModel(){
            multiPageView.clear();

            multiPageView.addPage("Roles", qsTr("Roles"), roleCollectionComp);
            multiPageView.addPage("Users", qsTr("Users"), userCollectionComp);
            multiPageView.addPage("Groups", qsTr("Groups"), userGroupCollectionComp);

            multiPageView.selectedIndex = 0;
        }
    }
}
