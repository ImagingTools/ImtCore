import QtQuick

Item {
    id: root

    // Names such as "Normal", "Horizontal" and "Unchecked" are ints in Qt, so an int property accepts them.
    property int echoMode: TextInput.Normal
    property int orientation: Qt.Horizontal
    property int checkState: Qt.Unchecked
    property int edge: Qt.LeftEdge

    Component.onCompleted: {
        console.log("echo", TextInput.Normal, TextInput.NoEcho, TextInput.Password, TextInput.PasswordEchoOnEdit, echoMode)
        console.log("orient", Qt.Horizontal, Qt.Vertical, orientation)
        orientation = Qt.Vertical
        console.log("orient-set", orientation)
        console.log("check", Qt.Unchecked, Qt.PartiallyChecked, Qt.Checked, checkState)
        checkState = Qt.Checked
        console.log("check-set", checkState)
        console.log("edge", Qt.TopEdge, Qt.LeftEdge, Qt.RightEdge, Qt.BottomEdge, edge)
        Qt.quit()
    }
}
