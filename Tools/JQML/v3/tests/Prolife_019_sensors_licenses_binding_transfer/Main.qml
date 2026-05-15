import QtQuick
import prolifeSensorsSdl 1.0
import prolifeLicensesSdl 1.0

Item {
    Component.onCompleted: {
        const devicesMatch = DeviceCollectionInfo.listCommandId === ProlifeSensorsSdlCommandIds.s_devicesList
        const softwareMatch = SoftwareProductCollectionInfo.listCommandId === ProlifeLicensesSdlCommandIds.s_softwareProductsList

        console.log(
            "prolife19",
            DeviceCollectionInfo.listCommandId,
            ProlifeSensorsSdlCommandIds.s_transferLicenses,
            ProlifeSensorsSdlCommandIds.s_devicesList,
            SoftwareProductCollectionInfo.listCommandId,
            ProlifeLicensesSdlCommandIds.s_softwareProductsList,
            ProlifeLicensesSdlCommandIds.s_softwareProductAdd,
            devicesMatch,
            softwareMatch
        )
        Qt.quit()
    }
}
