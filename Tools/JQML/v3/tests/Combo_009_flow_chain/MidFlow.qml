import QtQuick

BaseFlow {
    id: root

    property string label: "mid"

    function details() {
        return label + ":" + flowTag()
    }
}
