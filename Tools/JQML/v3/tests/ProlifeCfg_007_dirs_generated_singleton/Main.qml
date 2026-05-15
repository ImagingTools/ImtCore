import QtQuick
import imtappApplicationSdl 1.0

Item {
    Component.onCompleted: {
        console.log("prolifecfg7", ImtappApplicationSdlCommandIds.s_getApplicationInfo)
        Qt.quit()
    }
}
