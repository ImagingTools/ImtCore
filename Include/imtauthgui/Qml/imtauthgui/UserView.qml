import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtqml 1.0

DocumentData {
    id: container;

    anchors.fill: parent;

    documentCompleted: userRolesCompleted && userGroupsCompleted;

    property string title: qsTr("Users");
    property string username: qsTr("New User");

    property int mainMargin: 0;
    property int panelWidth: 150;

    onSaved: {
        updateUsername();
    }

    function updateUsername(){
        if (container.documentModel.ContainsKey("Name")){
            let username = documentModel.GetData("Name");
            if (username !== ""){
                container.username = username;
            }
        }
    }

    function beginDocumentModelChanged(){
        updateUsername();

        let generalPage = multiPageView.getPageById("General");
        let rolesPage = multiPageView.getPageById("Roles");
        let groupsPage = multiPageView.getPageById("Groups");

        if (generalPage){
            generalPage.documentModel = documentModel;
        }

        if (rolesPage){
            rolesPage.documentModel = documentModel;
        }

        if (groupsPage){
            groupsPage.documentModel = documentModel;
        }
    }

    function blockEditing(){
        let generalPage = multiPageView.getPageById("General");
        let rolesPage = multiPageView.getPageById("Roles");
        let groupsPage = multiPageView.getPageById("Groups");

        if (generalPage){
            generalPage.blockEditing();
        }

        if (rolesPage){
            rolesPage.blockEditing();
        }

        if (groupsPage){
            groupsPage.blockEditing();
        }
    }

    function updateGui(){
        console.log("UserView updateGui");

        let generalPage = multiPageView.getPageById("General");
        let rolesPage = multiPageView.getPageById("Roles");
        let groupsPage = multiPageView.getPageById("Groups");

        if (generalPage){
            generalPage.updateGui();
        }

        if (rolesPage){
            rolesPage.updateGui();
        }

        if (groupsPage){
            groupsPage.updateGui();
        }
    }

    function updateModel(){
        let generalPage = multiPageView.getPageById("General");
        let rolesPage = multiPageView.getPageById("Roles");
        let groupsPage = multiPageView.getPageById("Groups");

        if (generalPage){
            generalPage.updateModel();
        }

        if (rolesPage){
            rolesPage.updateModel();
        }

        if (groupsPage){
            groupsPage.updateModel();
        }
    }

    Component{
        id: emptyDecorator;
        Item{
            property Item rootItem: null;
        }
    }

    Rectangle {
        id: background;
        anchors.fill: parent;
        color: Style.backgroundColor;
        Loader{
            id: backgroundDecoratorLoader;

            sourceComponent: Style.backGroundDecorator !==undefined ? Style.backGroundDecorator: emptyDecorator;
            onLoaded: {
                if(backgroundDecoratorLoader.item){
                    backgroundDecoratorLoader.item.rootItem = background;
                }
            }
        }
    }

    Row {
        id: header;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.leftMargin: 10;

        height: 40

        spacing: 20;

        AuxButton {
            id: closeButton;

            anchors.verticalCenter: parent.verticalCenter;
            highlighted: Style.highlightedButtons !==undefined ? Style.highlightedButtons : containsMouse;
            width: 25;
            height: width;

            iconSource: "../../../" + Style.getIconPath("Icons/Left", Icon.State.On, Icon.Mode.Normal);

            onClicked: {
                Events.sendEvent(container.uuid + "CommandActivated", "Close")
            }
        }

        Text {
            id: headerText;

            anchors.verticalCenter: parent.verticalCenter;

            font.pixelSize: Style.fontSize_title;
            font.family: Style.fontFamily;

            color: Style.titleColor;

            text: container.title + " / " + container.username;

            Loader{
                id: titleDecoratorLoader;

                sourceComponent: Style.titleDecorator !==undefined ? Style.titleDecorator: emptyDecorator;
                onLoaded: {
                    if(titleDecoratorLoader.item){
                        titleDecoratorLoader.item.rootItem = headerText;
                    }
                }
            }
        }
    }

    Component {
        id: userEditorComp;

        UserEditor {
            documentPtr: container;
        }
    }


    property bool userRolesCompleted: false;
    Component {
        id: userRolesComp;

        UserRoles {
            documentPtr: container;

            onCompletedChanged: {
                if (completed){
                    container.userRolesCompleted = completed;
                }
            }
        }
    }

    property bool userGroupsCompleted: false;
    Component {
        id: userGroupsComp;

        UserGroups {
            documentPtr: container;

            onCompletedChanged: {
                if (completed){
                    container.userGroupsCompleted = completed;
                }
            }
        }
    }

    MultiPageView {
        id: multiPageView;

        anchors.top: header.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;

        Component.onCompleted: {
            multiPageView.addPage("General", "General", userEditorComp);
            multiPageView.addPage("Roles", "Roles", userRolesComp);
            multiPageView.addPage("Groups", "Groups", userGroupsComp);

            multiPageView.selectedIndex = 0;
        }
    }
}
