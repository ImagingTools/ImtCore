import QtQuick
import prolifeWorkspaceSdl 1.0
import prolifeLicensesSdl 1.0

Item {
    Component.onCompleted: {
        console.log(
            "prolife13",
            ProlifeWorkspaceSdlCommandIds.s_getTotalSummaryInfo,
            ProlifeLicensesSdlCommandIds.s_softwareProductAdd
        )
        Qt.quit()
    }
}
