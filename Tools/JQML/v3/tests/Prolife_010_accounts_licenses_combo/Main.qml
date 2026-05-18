import QtQuick
import prolifeAccountsSdl 1.0
import prolifeLicensesSdl 1.0

Item {
    AccountDataInput { id: account }
    SoftwareProductDataInput { id: license }

    Component.onCompleted: {
        account.m_name = "ACME"
        license.m_name = "Suite-X"

        console.log(
            "prolife10",
            account.__typename,
            account.m_name,
            license.__typename,
            license.m_name,
            ProlifeAccountsSdlCommandIds.s_accountAdd,
            ProlifeLicensesSdlCommandIds.s_softwareProductAdd
        )
        Qt.quit()
    }
}
