import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

ViewBase {
    id: container;

    anchors.fill: parent;

    property TreeItemModel rolesModel: TreeItemModel {}
    property TreeItemModel groupsModel: TreeItemModel {}

    onModelChanged: {
        console.log("UserView onModelChanged", model.toJSON());
    }

    function updateGui(){
        console.log("UserView updateGui")

        let generalPage = multiPageView.getPageById("General");
        let rolesPage = multiPageView.getPageById("Roles");
        let groupsPage = multiPageView.getPageById("Groups");

        if (generalPage){
            generalPage.doUpdateGui();
        }

        if (rolesPage){
            rolesPage.doUpdateGui();
        }

        if (groupsPage){
            groupsPage.doUpdateGui();
        }
    }

    function updateModel(){
        console.log("UserView updateModel")

        let generalPage = multiPageView.getPageById("General");
        let rolesPage = multiPageView.getPageById("Roles");
        let groupsPage = multiPageView.getPageById("Groups");

        if (generalPage){
            generalPage.doUpdateModel();
        }

        if (rolesPage){
            rolesPage.doUpdateModel();
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
        id: userEditorComp;

        UserEditor {
            model: container.model;
            readOnly: container.readOnly;
        }
    }

    Component {
        id: userRolesComp;

        UserRoles {
            model: container.model;
            readOnly: container.readOnly;

            rolesModel: container.rolesModel;
        }
    }

    Component {
        id: userGroupsComp;

        UserGroups {
            model: container.model;
            readOnly: container.readOnly;

            groupsModel: container.groupsModel;
        }
    }

    MultiPageView {
        id: multiPageView;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;

        Component.onCompleted: {
            multiPageView.addPage("General", "General", userEditorComp);
            multiPageView.addPage("Roles", "Roles", userRolesComp);
            multiPageView.addPage("Groups", "Groups", userGroupsComp);

            multiPageView.currentIndex = 0;
        }
    }
}
