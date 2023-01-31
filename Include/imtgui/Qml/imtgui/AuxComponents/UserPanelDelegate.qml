import QtQuick 2.12
import Acf 1.0

Item {
    id: root;

    function logout(){
        authorizationPage.tokenProvider.login = "";
        //thumbnailDecoratorContainer.clearModels();
        Events.sendEvent("clearModels");
        authorizationPage.visible = true;
    }
}

