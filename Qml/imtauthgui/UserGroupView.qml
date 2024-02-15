import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

ViewBase {
    id: container;

    property TreeItemModel groupsModel: TreeItemModel {}
    property TreeItemModel rolesModel: TreeItemModel {}
    property TreeItemModel usersModel: TreeItemModel {}

    function updateGui(){
        let generalPage = multiPageView.getPageById("General");
        let usersPage = multiPageView.getPageById("Users");
        let groupsPage = multiPageView.getPageById("Roles");

        if (generalPage){
            generalPage.doUpdateGui();
        }

        if (usersPage){
            usersPage.doUpdateGui();
        }

        if (groupsPage){
            groupsPage.doUpdateGui();
        }
    }

    function updateModel(){
        let generalPage = multiPageView.getPageById("General");
        let usersPage = multiPageView.getPageById("Users");
        let groupsPage = multiPageView.getPageById("Roles");

        if (generalPage){
            generalPage.doUpdateModel();
        }

        if (usersPage){
            usersPage.doUpdateModel();
        }

        if (groupsPage){
            groupsPage.doUpdateModel();
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

    Component {
        id: groupEditorComp;

        GroupEditor {
            model: container.model;
            readOnly: container.readOnly;

            groupsModel: container.groupsModel;
        }
    }

    Component {
        id: groupUsersComp;

        GroupUsers {
            model: container.model;
            readOnly: container.readOnly;

            usersModel: container.usersModel;
        }
    }

    Component {
        id: groupRolesComp;

        GroupRoles {
            model: container.model;
            readOnly: container.readOnly;

            rolesModel: container.rolesModel;
        }
    }

    MultiPageView {
        id: multiPageView;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;

        Component.onCompleted: {
            multiPageView.addPage("General", "General", groupEditorComp);
            multiPageView.addPage("Users", "Users", groupUsersComp);
            multiPageView.addPage("Roles", "Roles", groupRolesComp);

            multiPageView.currentIndex = 0;
        }
    }
}
