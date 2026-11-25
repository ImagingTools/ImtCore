import QtQuick

// A word wider than the text: WordWrap lets it out of the width on one line, Wrap and WrapAnywhere break it between its characters
// and keep it inside the width. A word that fits a line of its own is not broken by Wrap.
Item {
    id: root
    width: 300
    height: 200

    Text {
        id: word
        width: 60
        font.pixelSize: 20
        wrapMode: Text.WordWrap
        text: "abcdefghijklmnopqrstuvwxyz"
    }

    Text {
        id: wrap
        y: 30
        width: 60
        font.pixelSize: 20
        wrapMode: Text.Wrap
        text: "abcdefghijklmnopqrstuvwxyz"
    }

    Text {
        id: anywhere
        y: 60
        width: 60
        font.pixelSize: 20
        wrapMode: Text.WrapAnywhere
        text: "abcdefghijklmnopqrstuvwxyz"
    }

    Text {
        id: words
        y: 90
        width: 200
        font.pixelSize: 20
        wrapMode: Text.Wrap
        text: "abc abc abc abc abc abc abc abc"
    }

    Component.onCompleted: {
        console.log("WordWrap", word.lineCount, word.contentWidth > word.width)
        console.log("Wrap", wrap.lineCount > 1, wrap.contentWidth <= wrap.width)
        console.log("WrapAnywhere", anywhere.lineCount > 1, anywhere.contentWidth <= anywhere.width)
        console.log("Wrap of short words", words.lineCount > 1, words.contentWidth <= words.width)
        Qt.quit()
    }
}
