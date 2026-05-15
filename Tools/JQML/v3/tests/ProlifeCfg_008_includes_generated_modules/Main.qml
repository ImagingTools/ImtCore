import QtQuick
import imtappApplicationSdl 1.0
import imtauthUsersSdl 1.0

Item {
    Component.onCompleted: {
        console.log("prolifecfg8", ImtappApplicationSdlCommandIds.s_getApplicationInfo, ImtauthUsersSdlCommandIds.s_usersList)
        Qt.quit()
    }
}
