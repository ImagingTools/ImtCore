import QtQuick
import prolifeAccountsSdl 1.0
import prolifeOrdersSdl 1.0
import prolifeLicensesSdl 1.0

Item {
    Component.onCompleted: {
        console.log(
            "prolife14",
            AccountCollectionInfo.listCommandId,
            ProlifeAccountsSdlCommandIds.s_accountAdd,
            ProlifeOrdersSdlCommandIds.s_orderAdd,
            ProlifeLicensesSdlCommandIds.s_softwareProductAdd
        )
        Qt.quit()
    }
}
