import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtguigql 1.0
import imtcontrols 1.0

SingleDocumentWorkspacePageView {
    id: container;
    anchors.fill: parent;

    pageId: "Administration";
    pageName: qsTr("Administration");

    property string productId;
    property string webSocketUrl;

    signal commandsModelChanged(var commandsModel);

    Component.onCompleted: {
        Style.setDecorators(decorators_)
    }

    onWebSocketUrlChanged: {
        console.log("onWebSocketUrlChanged", webSocketUrl);
    }

    Connections {
        target: AuthorizationController;

        function onLoginSuccessful(){
            if (productId !== ""){
                container.startItemSourceComp = administrationViewComp
            }
        }

        function onLogoutSignal(){
        }
    }

    Component {
        id: administrationViewComp;

        AdministrationView {
            productId: container.productId;
        }
    }

    Decorators {
        id: decorators_
    }

    DecoratorsQt {
        id: decoratorsQt;
    }

    SubscriptionManager {
        id: subscriptionManager;
        url: container.webSocketUrl;
        // url: "ws://localhost:8112";
    }

    DialogManagerView {
        anchors.fill: parent;
    }

    Loading {
        id: loading;
        z: 10000;
        anchors.fill: parent;
        visible: false;
        Component.onCompleted: {
            Events.subscribeEvent("StartLoading", loading.start);
            Events.subscribeEvent("StopLoading", loading.stop);
        }

        Component.onDestruction: {
            Events.unSubscribeEvent("StartLoading", loading.start);
            Events.unSubscribeEvent("StopLoading", loading.stop);
        }
    }

    function login(login, password){
        let param = {"Login": login,"Password": password }
        AuthorizationController.login(param)
    }

    function logout(){
        AuthorizationController.userLogout({});
    }
}


