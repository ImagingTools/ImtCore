import QtQuick
import prolifeWorkspaceSdl 1.0
import prolifeOrdersSdl 1.0
import prolifeSensorsSdl 1.0

Item {
    ChartInput { id: chart }
    OrdersListInput { id: orders }
    DevicesListInput { id: devices }

    Component.onCompleted: {
        console.log(
            "prolife9",
            chart.__typename,
            orders.__typename,
            devices.__typename,
            ProlifeWorkspaceSdlCommandIds.s_getTotalSummaryInfo,
            ProlifeOrdersSdlCommandIds.s_ordersList,
            ProlifeSensorsSdlCommandIds.s_devicesList
        )
        Qt.quit()
    }
}
