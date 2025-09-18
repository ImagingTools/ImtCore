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

        acceptLabelChanged: {type:Signal, args:[]},
        folderChanged: {type:Signal, args:[]},
        currentFileChanged: {type:Signal, args:[]},
        currentFilesChanged: {type:Signal, args:[]},
        fileModeChanged: {type:Signal, args:[]},
        fileChanged: {type:Signal, args:[]},
        filesChanged: {type:Signal, args:[]},
        optionsChanged: {type:Signal, args:[]},
        rejectLabelChanged: {type:Signal, args:[]},
        seleectedNameFilterChanged: {type:Signal, args:[]},
        nameFiltersChanged: {type:Signal, args:[]},
    })
}



module.exports = FileDialog