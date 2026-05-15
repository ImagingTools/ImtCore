import QtQuick

Item {
    ChildAliasItem {
        id: child
        title: "main-init"
        value: 42
    }

    Component.onCompleted: {
        console.log("before", child.title, child.value)
        child.title = "main-updated"
        child.value = 77
        console.log("after", child.title, child.value)
        Qt.quit()
    }
}
