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
        acceptLabel: { type: String, value: '', signalName: 'acceptLabelChanged' },
        folder: { type: String, value: '', signalName: 'folderChanged' },
        currentFolder: { type: String, value: '', signalName: 'currentFolderChanged' },
        options: { type: Var, value: undefined, signalName: 'optionsChanged' },
        rejectLabel: { type: String, value: '', signalName: 'rejectLabelChanged' },

        acceptLabelChanged: {type:Signal, slotName:'onAcceptLabelChanged', args:[]},
        folderChanged: {type:Signal, slotName:'onFolderChanged', args:[]},
        currentFolderChanged: {type:Signal, slotName:'onCurrentFolderChanged', args:[]},
        optionsChanged: {type:Signal, slotName:'onOptionsChanged', args:[]},
        rejectLabelChanged: {type:Signal, slotName:'onRejectLabelChanged', args:[]},
    })
}

FolderDialog.initialize()

module.exports = FolderDialog