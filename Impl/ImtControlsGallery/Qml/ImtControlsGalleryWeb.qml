import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0

Item {
    id: window;

    anchors.fill: parent;

    ImtControlsGalleryMain {
        id: application;

        anchors.fill: parent;

        Component.onCompleted: {
            designProvider.setDesignSchema("Light");

            application.firstModelsInit();
        }
    }
}
