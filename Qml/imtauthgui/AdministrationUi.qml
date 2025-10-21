import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtguigql 1.0
import imtcontrols 1.0

SingleDocumentWorkspacePageView {
    id: container;
    anchors.fill: parent
    pageId: "Administration";
    pageName: qsTr("Administration");

    property string productId;
    property string webSocketUrl;

    signal commandsModelChanged(var commandsModel);

    Component.onCompleted: {
        Style.setDecorators(decorators_)
    }

	onProductIdChanged: {
		AuthorizationController.productId = productId
	}

    Connections {
        target: AuthorizationController;

		function onLoggedIn(){
            console.log("AuthorizationController onLoggedIn", container.productId)
            if (container.productId !== ""){
                container.startItemSourceComp = administrationViewComp
            }
        }

		function onLoggedOut(){
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
        id: subscriptionManager
        url: container.webSocketUrl
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

    function setLoginData(accessToken, username, permissions){
        console.log("setLoginData", accessToken, username, permissions)
		AuthorizationController.setLoginData("", accessToken, "", username, "", permissions)
	}

    function login(login, password){
        console.log("login", login, password)
        AuthorizationController.login(login, password)
    }

    function logout(){
        AuthorizationController.logoutForce();
    }
}


