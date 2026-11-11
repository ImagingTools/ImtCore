import QtQuick

// The top panel of an application: a Loader of 70 px loads a control whose size is the size of a decorator (80 px) that is given
// by the application when it is completed, after the Loader has loaded the control. A Binding gives the size of the control to the
// decorator. What the sizes are when everything has settled, and when the decorator is there before the Loader loads.
Item {
    id: root
    width: 400
    height: 300
    property Component decorators: null

    Component {
        id: decoratorComp
        Item {
            height: 80
            width: 400
        }
    }

    Component {
        id: panelComp
        FocusScope {
            id: panel
            property var decorator_: null
            property Component decorator: root.decorators
            width: decorator_ ? decorator_.width : 0
            height: decorator_ ? decorator_.height : 0

            onDecoratorChanged: {
                if (decorator) {
                    decorator_ = decorator.createObject(panel)
                    bindHeight.target = decorator_
                }
            }

            Binding {
                id: bindHeight
                property: "height"
                value: panel.height
            }
        }
    }

    Loader {
        id: late
        width: 400
        height: 70
        sourceComponent: panelComp
    }

    Timer {
        interval: 50
        running: true
        onTriggered: {
            console.log("before", late.item.height, late.item.decorator_ === null ? "no decorator" : late.item.decorator_.height)
            root.decorators = decoratorComp
            settle.start()
        }
    }

    Timer {
        id: settle
        interval: 100
        onTriggered: {
            console.log("after", late.item.height, late.item.decorator_.height)
            reload.start()
        }
    }

    Timer {
        id: settle2
        interval: 100
        onTriggered: {
            console.log("reloaded", late.item.height, late.item.decorator_.height)
            Qt.quit()
        }
    }

    Timer {
        id: reload
        interval: 50
        onTriggered: {
            late.active = false
            late.active = true
            settle2.start()
        }
    }
}
