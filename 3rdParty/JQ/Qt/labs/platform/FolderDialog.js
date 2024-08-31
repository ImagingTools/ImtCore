const Dialog = require("./Dialog")
const Signal = require("../../../QtQml/Signal")

class FolderDialog extends Dialog {
    static OpenFile = 0
    static OpenFiles = 1
    static SaveFile = 2

    static DontResolveSymlinks = 0
    static DontConfirmOverwrite = 0
    static ReadOnly = 0
    static HideNameFilterDetails = 0

    static meta = Object.assign({}, Dialog.meta, {

    })
}

module.exports = FolderDialog