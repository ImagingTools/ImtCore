import QtQuick
import prolifeWorkspaceSdl 1.0
import prolifeAccountsSdl 1.0

Item {
    Component.onCompleted: {
        const totalSummaryLooksLikeQuery = ProlifeWorkspaceSdlCommandIds.s_getTotalSummaryInfo.indexOf('Get') === 0
        const orderChartLooksLikeQuery = ProlifeWorkspaceSdlCommandIds.s_getOrderCreationLineChart.indexOf('Get') === 0
        const accountsMatch = AccountCollectionInfo.listCommandId === ProlifeAccountsSdlCommandIds.s_accountsList

        console.log(
            "prolife18",
            ProlifeWorkspaceSdlCommandIds.s_getTotalSummaryInfo,
            ProlifeWorkspaceSdlCommandIds.s_getOrderCreationLineChart,
            AccountCollectionInfo.listCommandId,
            ProlifeAccountsSdlCommandIds.s_accountsList,
            totalSummaryLooksLikeQuery,
            orderChartLooksLikeQuery,
            accountsMatch
        )
        Qt.quit()
    }
}
