import QtQuick

Item {
    id: root

    property int wrapMode: Text.NoWrap
    property int elideMode: Text.ElideRight

    Text {
        id: label
        text: "hello"
        wrapMode: Text.WordWrap
        elide: Text.ElideMiddle
    }

    Component.onCompleted: {
        console.log("wrap", Text.NoWrap, Text.WordWrap, Text.WrapAnywhere, Text.Wrap, Text.WrapAtWordBoundaryOrAnywhere)
        console.log("elide", Text.ElideLeft, Text.ElideRight, Text.ElideMiddle, Text.ElideNone)
        console.log("props", wrapMode, elideMode)
        console.log("text", label.wrapMode, label.elide)
        Qt.quit()
    }
}
