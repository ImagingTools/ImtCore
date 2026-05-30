import QtQuick

MidListView {
    id: root

    onReady: {
        console.log("combo6-main", message(), entryCount())
        Qt.quit()
    }
}
