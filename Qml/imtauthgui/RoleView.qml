import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

ViewBase {
    id: container;

    property TreeItemModel permissionsModel: TreeItemModel {};
    property TreeItemModel rolesModel: TreeItemModel {};

    property string productId: "";

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", container.onLocalizationChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", container.onLocalizationChanged);
    }

    function onLocalizationChanged(language){
        let generalIndex = multiPageView.getIndexById("General");
        if (generalIndex >= 0){
            multiPageView.pagesModel.setProperty(generalIndex, "Name", qsTr("General"))
        }

        let permissionsIndex = multiPageView.getIndexById("Permissions");
        if (permissionsIndex >= 0){
            multiPageView.pagesModel.setProperty(permissionsIndex, "Name", qsTr("Permissions"))
        }
    }

    function updateGui(){
        console.log("RoleView updateGui", model.toJSON());
        let generalPage = multiPageView.getPageById("General");
        let permissionsPage = multiPageView.getPageById("Permissions");

        if (generalPage){
            generalPage.doUpdateGui();
        }
        if (permissionsPage){
            permissionsPage.doUpdateGui();
        }
    }

    function updateModel(){
        console.log("RoleView updateModel", model.toJSON());

        let generalPage = multiPageView.getPageById("General");
        let permissionsPage = multiPageView.getPageById("Permissions");

        if (generalPage){
            generalPage.doUpdateModel();
        }

        if (permissionsPage){
            permissionsPage.doUpdateModel();
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
        id: roleEditorComp;

        RoleEditor {
            model: container.model;

            readOnly: container.readOnly;
            rolesModel: container.rolesModel;
        }
    }

    Component {
        id: rolePermissionsComp;

        RolePermissions {
            model: container.model;
            readOnly: container.readOnly;

            productId: container.productId;

            permissionsModel: container.permissionsModel;
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
            multiPageView.addPage("General", qsTr("General"), roleEditorComp);
            multiPageView.addPage("Permissions", qsTr("Permissions"), rolePermissionsComp);

            multiPageView.currentIndex = 0;
        }
    }
}
