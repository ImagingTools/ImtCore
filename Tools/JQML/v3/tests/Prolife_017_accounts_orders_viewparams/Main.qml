import QtQuick
import prolifeAccountsSdl 1.0
import prolifeOrdersSdl 1.0

Item {
    Component.onCompleted: {
        const accountsMatch = AccountCollectionInfo.listCommandId === ProlifeAccountsSdlCommandIds.s_accountsList
        const ordersMatch = OrderCollectionInfo.listCommandId === ProlifeOrdersSdlCommandIds.s_ordersList

        console.log(
            "prolife17",
            ProlifeAccountsSdlCommandIds.s_accountsList,
            ProlifeOrdersSdlCommandIds.s_ordersList,
            AccountCollectionInfo.listCommandId,
            OrderCollectionInfo.listCommandId,
            accountsMatch,
            ordersMatch
        )
        Qt.quit()
    }
}
