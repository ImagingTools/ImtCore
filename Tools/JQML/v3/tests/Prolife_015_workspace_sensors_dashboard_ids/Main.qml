import QtQuick
import prolifeWorkspaceSdl 1.0
import prolifeSensorsSdl 1.0

Item {
    Component.onCompleted: {
        console.log(
            "prolife15",
            ProlifeWorkspaceSdlCommandIds.s_getTotalSummaryInfo,
            ProlifeWorkspaceSdlCommandIds.s_getOrderCreationLineChart,
            ProlifeSensorsSdlCommandIds.s_devicesList,
            ProlifeSensorsSdlCommandIds.s_transferLicenses
        )
        Qt.quit()
    }
}
