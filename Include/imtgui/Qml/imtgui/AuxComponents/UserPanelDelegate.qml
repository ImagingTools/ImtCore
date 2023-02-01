import QtQuick 2.12
import Acf 1.0

Item {
    id: root;

    function logout(){
        //thumbnailDecoratorContainer.clearModels();
//        authorizationPage.tokenProvider.login = "";
//        authorizationPage.visible = true;
        Events.sendEvent("clearModels");
        Events.sendEvent("logout");
    }
}

