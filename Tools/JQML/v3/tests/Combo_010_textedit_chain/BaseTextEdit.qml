import QtQuick

Item {
    id: root

    property string baseText: "alpha"

    function currentText() {
        return root.baseText
    }
}
