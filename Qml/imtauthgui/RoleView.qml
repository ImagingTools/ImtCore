import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

ViewBase {
    id: container;

    property string productId: "";

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
        }
    }

    Component {
        id: rolePermissionsComp;

        RolePermissions {
            model: container.model;
            readOnly: container.readOnly;

            productId: container.productId;
        }
    }

    MultiPageView {
        id: multiPageView;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;

        Component.onCompleted: {
            multiPageView.addPage("General", "General", roleEditorComp);
            multiPageView.addPage("Permissions", "Permissions", rolePermissionsComp);

            multiPageView.currentIndex = 0;
        }
    }
}
