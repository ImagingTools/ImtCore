import QtQuick

BaseListViewAlias {
    id: root

    property alias footerText: footer.text

    Text {
        id: footer
        text: "mid"
    }

    function summary() {
        return label() + ":" + footerText
    }
}