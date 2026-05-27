import QtQuick 2.12
import QtQuick.Window 2.2
import imtcontrols 1.0

/*!
    Demo/manual-verification window for the imtcontrols Popup + Menu stack.

    Not registered in the qmldir; load directly with QQmlApplicationEngine
    when you want to eyeball the behaviour.
*/
Window {
    id: win
    width: 800
    height: 600
    visible: true
    title: "imtcontrols Popup/Menu gallery"

    color: "#f5f5f5"

    // ---- 1) Basic popup -----------------------------------------------
    Rectangle {
        id: openBtn
        x: 20; y: 60
        width: 160; height: 32
        color: openMA.containsMouse ? "#cce5ff" : "#e0e0e0"
        radius: 4
        Text { anchors.centerIn: parent; text: "Open basic popup" }
        MouseArea {
            id: openMA
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                basicPopup.open()
            }
        }
    }
    Popup {
        id: basicPopup
        x: 0; y: 0
        width: 240; height: 120
        modal: true
        parent: openBtn
        contentItem: Rectangle {
            color: "white"
            border.color: "#888"; border.width: 1
            Text { anchors.centerIn: parent; text: "Modal popup\nEscape or click outside to close" }
        }
    }

    // ---- 2) Menu -------------------------------------------------------
    Rectangle {
        id: menuBtn
        x: 200; y: 60
        width: 160; height: 32
        color: menuMA.containsMouse ? "#cce5ff" : "#e0e0e0"
        radius: 4
        Text { anchors.centerIn: parent; text: "Open menu" }
        MouseArea {
            id: menuMA
            anchors.fill: parent
            onClicked: {
                fileMenu.x = 0; fileMenu.y = menuBtn.height;
                fileMenu.popup(menuBtn);
            }
        }
        Menu {
            id: fileMenu
            MenuItem { text: "New";  onTriggered: console.log("New") }
            MenuItem { text: "Open"; onTriggered: console.log("Open") }
            MenuItem { text: "Save"; checkable: true }
            MenuSeparator {}
            Menu {
                title: "Recent"
                MenuItem { text: "file1.txt" }
                MenuItem { text: "file2.txt" }
                MenuItem { text: "file3.txt" }
            }
            MenuSeparator {}
            MenuItem { text: "Quit"; onTriggered: Qt.quit() }
        }
    }

    // ---- 3) MenuBar ---------------------------------------------------
    MenuBar {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        Menu {
            title: "File"
            MenuItem { text: "New" }
            MenuItem { text: "Open" }
            MenuSeparator {}
            MenuItem { text: "Quit"; onTriggered: Qt.quit() }
        }
        Menu {
            title: "Edit"
            MenuItem { text: "Undo" }
            MenuItem { text: "Redo" }
        }
        Menu {
            title: "View"
            MenuItem { text: "Zoom in" }
            MenuItem { text: "Zoom out" }
        }
    }
}
