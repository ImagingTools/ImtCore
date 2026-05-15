import QtQuick
import prolifeDeviceCollectionDocumentManagerSdl 1.0
import prolifeOrdersSdl 1.0
import prolifeAccountsSdl 1.0

Item {
    Component.onCompleted: {
        console.log(
            "prolife16",
            ProlifeDeviceCollectionDocumentManagerSdlCommandIds.s_updateDeviceFromRepresentation,
            ProlifeDeviceCollectionDocumentManagerSdlCommandIds.s_updateIotDeviceFromRepresentation,
            ProlifeOrdersSdlCommandIds.s_ordersList,
            ProlifeAccountsSdlCommandIds.s_accountsList
        )
        Qt.quit()
    }
}
