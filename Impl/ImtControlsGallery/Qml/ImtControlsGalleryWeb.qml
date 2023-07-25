import QtQuick 2.0
import Acf 1.0

Item {
    id: window;

    anchors.fill: parent;

    ImtControlsGalleryMain {
        id: application;

        anchors.fill: parent;

        systemStatus: "NO_ERROR";

        function updateModels(){
            console.log("window updateModels");
            thumbnailDecoratorGui.authorizationPageAlias.visible = true;
        }

        Component.onCompleted: {
            designProvider.applyDesignSchema("Light");

            application.firstModelsInit();
        }
    }
}
