import QtQuick

// a base type sets font.bold, a derived document binds it (ButtonImpl / DesktopSidebarMenu of QmlAppTemplate)
Text {
    font.pixelSize: 12
    font.bold: false
    text: "base"
}
