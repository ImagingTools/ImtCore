import QtQuick

Item {
    id: root

    // Quotes inside a regular expression used to desync escapeSourceBreaks' string tracking,
    // so the line break after the regex was escaped as `\n` and the function was rejected.
    function unquote(value) {
        var text = value.replace(/^['"]|['"]$/g, "").trim();
        return text.split(",")[0]
    }

    Component.onCompleted: {
        console.log("ok", unquote("'Arial'"))
        console.log("ok", unquote("\"Times\""))
        Qt.quit()
    }
}
