import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtqml 1.0

DocumentData {
    id: container;

    property string productId: "";
    property string title;
    property string roleName: qsTr("New Role");

    property int mainMargin: 0;
    property int panelWidth: 150;

    documentCompleted: roleEditorCompleted && rolePermissionsCompleted;

    onProductIdChanged: {
        console.log("onProductIdChanged", container.productId);

        container.title = container.productId + " / " + qsTr("Roles");
    }

    onSaved: {
        updateRoleName();
    }

    function updateRoleName(){
        if (container.documentModel.ContainsKey("Name")){
            let roleName = container.documentModel.GetData("Name");

            if (roleName != ""){
                container.roleName = roleName;
            }
        }
    }

    function beginDocumentModelChanged(){
        updateRoleName();

        if (container.productId === "" && container.documentModel.ContainsKey("ProductId")){
            container.productId = container.documentModel.GetData("ProductId")
        }
        else{
            container.documentModel.SetData("ProductId", container.productId)
        }

        if (container.documentModel.ContainsKey("ProductId")){
            container.productId = container.documentModel.GetData("ProductId")
            console.log("productId", container.productId);
        }

        let generalPage = multiPageView.getPageById("General");
        let permissionsPage = multiPageView.getPageById("Permissions");

        if (generalPage){
            generalPage.documentModel = container.documentModel;
        }

        if (permissionsPage){
            permissionsPage.documentModel = container.documentModel;
        }
    }

    function blockEditing(){
        let generalPage = multiPageView.getPageById("General");
        let permissionsPage = multiPageView.getPageById("Permissions");

        if (generalPage){
            generalPage.blockEditing();
        }

        if (permissionsPage){
            permissionsPage.blockEditing();
        }
    }

    function updateGui(){
        let generalPage = multiPageView.getPageById("General");
        let permissionsPage = multiPageView.getPageById("Permissions");

        if (generalPage){
            generalPage.updateGui();
        }
        if (permissionsPage){
            permissionsPage.updateGui();
        }
    }

    function updateModel(){
        let generalPage = multiPageView.getPageById("General");
        let permissionsPage = multiPageView.getPageById("Permissions");

        if (generalPage){
            generalPage.updateModel();
        }

        if (permissionsPage){
            permissionsPage.updateModel();
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

        z: 10;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.leftMargin: 10;

        height: 40;

        spacing: 10;

        clip: true;

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

            text: container.title + " / " + container.roleName;

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

    property bool roleEditorCompleted: false;
    Component {
        id: roleEditorComp;

        RoleEditor {
            onCompletedChanged: {
                if (completed){
                    container.roleEditorCompleted = completed;
                }
            }
        }
    }

    property bool rolePermissionsCompleted: false;
    Component {
        id: rolePermissionsComp;

        RolePermissions {
            onCompletedChanged: {
                if (completed){
                    container.rolePermissionsCompleted = completed;
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
            multiPageView.addPage("General", "General", "qrc:/qml/imtauthgui/RoleEditor.qml", roleEditorComp);
            multiPageView.addPage("Permissions", "Permissions", "qrc:/qml/imtauthgui/RolePermissions.qml", rolePermissionsComp);

            multiPageView.selectedIndex = 0;
        }

        onPageLoaded: {
            console.log("MultiPageView onPageLoaded", pageItem);

            if (pageItem.documentPtr !== undefined){
                pageItem.documentPtr = container;
            }
        }
    }
}
