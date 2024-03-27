import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

ViewBase {
    id: container;

    property TreeItemModel groupsModel: TreeItemModel {}
    property TreeItemModel rolesModel: TreeItemModel {}
    property TreeItemModel usersModel: TreeItemModel {}

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", container.onLocalizationChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", container.onLocalizationChanged);
    }

    function onLocalizationChanged(language){
        let index1 = multiPageView.getIndexById("General");
        if (index1 >= 0){
            multiPageView.pagesModel.setProperty(index1, "Name", qsTr("General"))
        }

        let index2 = multiPageView.getIndexById("Roles");
        if (index2 >= 0){
            multiPageView.pagesModel.setProperty(index2, "Name", qsTr("Roles"))
        }

        let index3 = multiPageView.getIndexById("Users");
        if (index3 >= 0){
            multiPageView.pagesModel.setProperty(index3, "Name", qsTr("Users"))
        }
    }

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
            multiPageView.addPage("General", qsTr("General"), groupEditorComp);
            multiPageView.addPage("Users", qsTr("Users"), groupUsersComp);
            multiPageView.addPage("Roles", qsTr("Roles"), groupRolesComp);

            multiPageView.currentIndex = 0;
        }
    }
}
