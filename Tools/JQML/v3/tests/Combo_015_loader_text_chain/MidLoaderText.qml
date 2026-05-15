import QtQuick

BaseLoaderText {
    id: root

    property string suffix: "mid"

    function summary() {
        return loaderState() + ":" + suffix
    }
}