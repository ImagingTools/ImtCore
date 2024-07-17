const Item = require("../../../QtQuick/Item")
const Signal = require("../../../QtQml/Signal")

class FolderDialog extends Item {
    static OpenFile = 0
    static OpenFiles = 1
    static SaveFile = 2

    static DontResolveSymlinks = 0
    static DontConfirmOverwrite = 0
    static ReadOnly = 0
    static HideNameFilterDetails = 0

    static meta = Object.assign({}, Item.meta, {

    })
}

module.exports = FolderDialog