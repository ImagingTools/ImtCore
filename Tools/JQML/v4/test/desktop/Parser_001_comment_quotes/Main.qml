import QtQuick

Item {
    id: root

    // Quotes inside a // comment used to break escapeSourceBreaks and reject the function.
    function describe(width) {
        // console.log("[TestArea] cell created", "width", width);
        return width
    }

    Component.onCompleted: {
        console.log("ok", describe(12))
        Qt.quit()
    }
}
