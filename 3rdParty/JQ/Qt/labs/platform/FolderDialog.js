const Dialog = require("./Dialog")
const String = require("../../../QtQml/String")
const Var = require("../../../QtQml/Var")
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
        acceptLabel: { type: String, value: ''},
        folder: { type: String, value: ''},
        currentFolder: { type: String, value: ''},
        options: { type: Var, value: undefined},
        rejectLabel: { type: String, value: ''},

        acceptLabelChanged: {type:Signal, args:[]},
        folderChanged: {type:Signal, args:[]},
        currentFolderChanged: {type:Signal, args:[]},
        optionsChanged: {type:Signal, args:[]},
        rejectLabelChanged: {type:Signal, args:[]},
    })
}



module.exports = FolderDialog