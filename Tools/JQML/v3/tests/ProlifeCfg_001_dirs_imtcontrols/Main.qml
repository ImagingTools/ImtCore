import QtQuick
import imtcontrols 1.0

Item {
    BaseClass {
        id: base
    }

    ControlBase {
        id: ctrl
        width: 40
        height: 22
    }

    Component.onCompleted: {
        console.log("prolifecfg1", base !== null, ctrl.width, ctrl.height)
        Qt.quit()
    }
}
