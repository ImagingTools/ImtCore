import QtQuick
import imtcontrols 1.0

Item {
    ControlBase {
        id: ctrl
        decorator: Component {
            Rectangle {
                width: 80
                height: 30
            }
        }
    }

    Component.onCompleted: {
        console.log("prolifecfg2", ctrl.width, ctrl.height, ctrl.decorator_ !== null)
        Qt.quit()
    }
}
