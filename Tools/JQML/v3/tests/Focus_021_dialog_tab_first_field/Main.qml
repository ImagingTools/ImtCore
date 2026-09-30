import QtQuick

Window {
    id: root
    width: 360
    height: 400
    visible: true

    function dump(tag) {
        console.log(tag,
            "tf1", tf1.focus, tf1.activeFocus, tf1Input.focus, tf1Input.activeFocus,
            "tf2", tf2.focus, tf2.activeFocus, tf2Input.focus, tf2Input.activeFocus,
            "tf3", tf3.activeFocus, tf3Input.activeFocus,
            "tf4", tf4.activeFocus, tf4Input.activeFocus
        )
    }

    FocusScope {
        id: testDialog
        anchors.fill: parent

        property var focusTargetList: []
        property Item currentItem: null
        property int selectedButtonIndex: -1

        function setButtonActive(buttonIdArg) {
        }

        function setConnections() {
            for (let i = 0; i < testDialog.focusTargetList.length; i++) {
                let item = testDialog.focusTargetList[i]
                item.activeFocusChanged.connect(function() {
                    if (item.activeFocus) {
                        for (let j = 0; j < testDialog.focusTargetList.length; j++) {
                            if (testDialog.focusTargetList[j] !== item) {
                                testDialog.focusTargetList[j].focus = false
                            }
                        }
                        testDialog.currentItem = item
                    }
                })
            }
        }

        function handleTab() {
            if (!testDialog.focusTargetList.length) {
                return
            }

            let index = -1
            if (!testDialog.currentItem && testDialog.selectedButtonIndex < 0) {
                testDialog.setButtonActive("")
                testDialog.focusTargetList[0].forceActiveFocus()
                return
            }

            for (let i = 0; i < testDialog.focusTargetList.length; i++) {
                if (testDialog.focusTargetList[i] === testDialog.currentItem) {
                    if (i < testDialog.focusTargetList.length - 1) {
                        index = i + 1
                    }
                    testDialog.selectedButtonIndex = -1
                    testDialog.setButtonActive("")
                    break
                }
            }

            if (index == -1) {
                index = 0
                testDialog.setButtonActive("")
            }

            if (index >= 0) {
                testDialog.focusTargetList[index].forceActiveFocus()
            }
        }

        Column {
            id: column
            x: 20
            y: 20
            width: parent.width - 40
            spacing: 20

            FocusScope {
                id: tf1
                width: parent.width
                height: 32

                onFocusChanged: {
                    if (tf1.focus) {
                        tf1Input.focus = true
                        tf1Input.forceActiveFocus()
                    } else {
                        tf1Input.focus = false
                    }
                }

                TextInput {
                    id: tf1Input
                    anchors.fill: parent
                    text: "TextField1"
                }
            }

            FocusScope {
                id: tf2
                width: parent.width
                height: 32

                onFocusChanged: {
                    if (tf2.focus) {
                        tf2Input.focus = true
                        tf2Input.forceActiveFocus()
                    } else {
                        tf2Input.focus = false
                    }
                }

                TextInput {
                    id: tf2Input
                    anchors.fill: parent
                    text: "TextField2"
                }
            }

            FocusScope {
                id: tf3
                width: parent.width
                height: 32

                onFocusChanged: {
                    if (tf3.focus) {
                        tf3Input.focus = true
                        tf3Input.forceActiveFocus()
                    } else {
                        tf3Input.focus = false
                    }
                }

                TextInput {
                    id: tf3Input
                    anchors.fill: parent
                    text: "TextField3"
                }
            }

            FocusScope {
                id: tf4
                width: parent.width
                height: 32

                onFocusChanged: {
                    if (tf4.focus) {
                        tf4Input.focus = true
                        tf4Input.forceActiveFocus()
                    } else {
                        tf4Input.focus = false
                    }
                }

                TextInput {
                    id: tf4Input
                    anchors.fill: parent
                    text: "TextField4"
                }
            }
        }
    }

    Shortcut {
        id: tabShortcut
        sequence: "Tab"
        onActivated: testDialog.handleTab()
    }

    Timer {
        id: stepTimer
        interval: 50
        repeat: false
        onTriggered: {
            testDialog.focusTargetList = column.children
            testDialog.setConnections()
            console.log("focus021-targets", testDialog.focusTargetList.length)
            console.log("focus021-dialog", testDialog.focus, testDialog.activeFocus)

            root.dump("focus021-open")

            tabShortcut.activated()
            root.dump("focus021-tab1")

            tabShortcut.activated()
            root.dump("focus021-tab2")

            Qt.quit()
        }
    }

    Component.onCompleted: {
        stepTimer.start()
    }
}
