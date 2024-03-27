import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

ViewBase {
    id: container;

    anchors.fill: parent;

    property TreeItemModel rolesModel: TreeItemModel {}
    property TreeItemModel groupsModel: TreeItemModel {}

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

        let index3 = multiPageView.getIndexById("Groups");
        if (index3 >= 0){
            multiPageView.pagesModel.setProperty(index3, "Name", qsTr("Groups"))
        }
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
            multiPageView.addPage("General", qsTr("General"), userEditorComp);
            multiPageView.addPage("Roles", qsTr("Roles"), userRolesComp);
            multiPageView.addPage("Groups", qsTr("Groups"), userGroupsComp);

            multiPageView.currentIndex = 0;
        }
    }
}
