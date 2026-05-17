import QtQuick
import prolifeOrdersSdl 1.0
import prolifeAccountsSdl 1.0
import prolifeSensorsSdl 1.0

Item {
    Component.onCompleted: {
        console.log(
            "prolife12",
            ProlifeOrdersSdlCommandIds.s_ordersList,
            ProlifeAccountsSdlCommandIds.s_accountsList,
            ProlifeSensorsSdlCommandIds.s_devicesList
        )
        Qt.quit()
    }
}
