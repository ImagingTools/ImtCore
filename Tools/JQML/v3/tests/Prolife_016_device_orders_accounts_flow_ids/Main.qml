import QtQuick
import prolifeDeviceCollectionDocumentServiceSdl 1.0
import prolifeOrdersSdl 1.0
import prolifeAccountsSdl 1.0

Item {
    Component.onCompleted: {
        console.log(
            "prolife16",
            ProlifeDeviceCollectionDocumentServiceSdlCommandIds.s_updateDeviceFromRepresentation,
            ProlifeDeviceCollectionDocumentServiceSdlCommandIds.s_updateIotDeviceFromRepresentation,
            ProlifeOrdersSdlCommandIds.s_ordersList,
            ProlifeAccountsSdlCommandIds.s_accountsList
        )
        Qt.quit()
    }
}
