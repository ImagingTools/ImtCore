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
        Events.unSubscribeEvent("OnLocalizationChanged", container.onLocalizationChanged);
    }

    function onLocalizationChanged(language){
        console.log("Administration onLocalizationChanged", language);

//        for (let i = 0; i < leftMenuModel.count; i++){
//            let id = leftMenuModel.get(i).Id;
//            if (id === "Roles"){
//                console.log("id == Roles");

//                leftMenuModel.setProperty(i, "Name", qsTr("Roles"));
//            }
//            else if (id === "Users"){
//                leftMenuModel.setProperty(i, "Name", qsTr("Users"));
//            }
//            else if (id === "Groups"){
//                leftMenuModel.setProperty(i, "Name", qsTr("Groups"));
//            }
//        }
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
            multiPageView.addPage("Roles", "Roles", "RoleCollectionView.qml", roleCollectionComp);
            multiPageView.addPage("Users", "Users", "UserCollectionView.qml", userCollectionComp);
            multiPageView.addPage("Groups", "Groups", "UserGroupCollectionView.qml", userGroupCollectionComp);

            multiPageView.selectedIndex = 0;
        }

        onSelectedIndexChanged: {
            if (selectedIndex >= 0){
                headerText.text = qsTr("Administration") + " / " + pagesModel.get(selectedIndex).Name;
            }
        }
    }
}
