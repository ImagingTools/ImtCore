import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtqml 1.0

DocumentData {
    id: container;

    documentCompleted: groupEditorCompleted && groupUsersCompleted && groupRolesCompleted;

    property string title: qsTr("User Groups");
    property string username: qsTr("New Group");

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
        let usersPage = multiPageView.getPageById("Users");
        let groupsPage = multiPageView.getPageById("Roles");

        if (generalPage){
            generalPage.documentModel = documentModel;
        }

        if (usersPage){
            usersPage.documentModel = documentModel;
        }

        if (groupsPage){
            groupsPage.documentModel = documentModel;
        }
    }

    function blockEditing(){
        let generalPage = multiPageView.getPageById("General");
        let usersPage = multiPageView.getPageById("Users");
        let groupsPage = multiPageView.getPageById("Roles");

        if (generalPage){
            generalPage.blockEditing();
        }

        if (usersPage){
            usersPage.blockEditing();
        }

        if (groupsPage){
            groupsPage.blockEditing();
        }
    }

    function updateGui(){
        let generalPage = multiPageView.getPageById("General");
        let usersPage = multiPageView.getPageById("Users");
        let groupsPage = multiPageView.getPageById("Roles");

        if (generalPage){
            generalPage.updateGui();
        }

        if (usersPage){
            usersPage.updateGui();
        }

        if (groupsPage){
            groupsPage.updateGui();
        }
    }

    function updateModel(){
        let generalPage = multiPageView.getPageById("General");
        let usersPage = multiPageView.getPageById("Users");
        let groupsPage = multiPageView.getPageById("Roles");

        if (generalPage){
            generalPage.updateModel();
        }

        if (usersPage){
            usersPage.updateModel();
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

    property bool groupEditorCompleted: false;
    Component {
        id: groupEditorComp;

        GroupEditor {
            documentPtr: container;

            onCompletedChanged: {
                if (completed){
                    container.groupEditorCompleted = completed;
                }
            }
        }
    }

    property bool groupUsersCompleted: false;
    Component {
        id: groupUsersComp;

        GroupUsers {
            documentPtr: container;

            onCompletedChanged: {
                if (completed){
                    container.groupUsersCompleted = completed;
                }
            }
        }
    }

    property bool groupRolesCompleted: false;
    Component {
        id: groupRolesComp;

        GroupRoles {
            documentPtr: container;

            onCompletedChanged: {
                if (completed){
                    container.groupRolesCompleted = completed;
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
            multiPageView.addPage("General", "General", groupEditorComp);
            multiPageView.addPage("Users", "Users", groupUsersComp);
            multiPageView.addPage("Roles", "Roles", groupRolesComp);

            multiPageView.selectedIndex = 0;
        }
    }
}
