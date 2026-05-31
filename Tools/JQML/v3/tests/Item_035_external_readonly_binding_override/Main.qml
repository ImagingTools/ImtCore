import QtQuick

Item {
    ReadonlyBindingChild {
        id: child
        value: 9
    }

    Component.onCompleted: {
        console.log("fixed", child.fixed, "value", child.value)
        Qt.quit()
    }
}
