import QtQuick
import QtQuick.Controls

// the image of an icon of a Button (QQuickIconLabel::syncImage): drawn in the default icon color of the style when the
// icon has no color, in its own color else (QQuickIconImage: a color with alpha draws the picture in it)
Item {
    Button { id: plain; icon.source: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR42mP8z8BQDwAEhQGAhKmMIQAAAABJRU5ErkJggg=="; background: null }
    Button { id: colored; icon.source: plain.icon.source; icon.color: "red" }
    Button { id: highlighted; icon.source: plain.icon.source; highlighted: true }
    function image(b) {
        for (let i = 0; i < b.contentItem.children.length; i++)
            if (b.contentItem.children[i].objectName === "image") return b.contentItem.children[i]
        return null
    }
    Component.onCompleted: {
        for (let b of [plain, colored, highlighted]) {
            let im = image(b)
            console.log(b.icon.color, im ? im.color : "-")
        }
        Qt.quit()
    }
}
