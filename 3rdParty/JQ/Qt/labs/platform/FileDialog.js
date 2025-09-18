const Dialog = require("./Dialog")
const String = require("../../../QtQml/String")
const Var = require("../../../QtQml/Var")
const Int = require("../../../QtQml/Int")
const Signal = require("../../../QtQml/Signal")

class FileDialog extends Dialog {
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
        currentFile: { type: String, value: ''},
        currentFiles: { type: Var, value: undefined},
        fileMode: { type: Int, value: FileDialog.OpenFile},
        file: { type: Var, value: undefined},
        files: { type: Var, value: undefined},
        options: { type: Var, value: undefined},
        rejectLabel: { type: String, value: ''},
        seleectedNameFilter: { type: Var, value: undefined},
        nameFilters: { type: Var, value: undefined},

        acceptLabelChanged: {type:Signal, slotName:'onAcceptLabelChanged', args:[]},
        folderChanged: {type:Signal, slotName:'onFolderChanged', args:[]},
        currentFileChanged: {type:Signal, slotName:'onCurrentFileChanged', args:[]},
        currentFilesChanged: {type:Signal, slotName:'onCurrentFilesChanged', args:[]},
        fileModeChanged: {type:Signal, slotName:'onFileModeChanged', args:[]},
        fileChanged: {type:Signal, slotName:'onFileChanged', args:[]},
        filesChanged: {type:Signal, slotName:'onFilesChanged', args:[]},
        optionsChanged: {type:Signal, slotName:'onOptionsChanged', args:[]},
        rejectLabelChanged: {type:Signal, slotName:'onRejectLabelChanged', args:[]},
        seleectedNameFilterChanged: {type:Signal, slotName:'onSeleectedNameFilterChanged', args:[]},
        nameFiltersChanged: {type:Signal, slotName:'onNameFiltersChanged', args:[]},
    })
}



module.exports = FileDialog