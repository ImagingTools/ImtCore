import QtQuick

Item {
    property int modality: Qt.ApplicationModal

    Component.onCompleted: {
        console.log("values", Qt.NonModal, Qt.WindowModal, Qt.ApplicationModal)
        console.log("prop", modality)
        modality = Qt.WindowModal
        console.log("window", modality)
        modality = Qt.NonModal
        console.log("none", modality)
        Qt.quit()
    }
}
