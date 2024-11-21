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
        acceptLabel: { type: String, value: '', signalName: 'acceptLabelChanged' },
        folder: { type: String, value: '', signalName: 'folderChanged' },
        currentFile: { type: String, value: '', signalName: 'currentFileChanged' },
        currentFiles: { type: Var, value: undefined, signalName: 'currentFilesChanged' },
        fileMode: { type: Int, value: FileDialog.OpenFile, signalName: 'fileModeChanged' },
        file: { type: Var, value: undefined, signalName: 'fileChanged' },
        files: { type: Var, value: undefined, signalName: 'filesChanged' },
        options: { type: Var, value: undefined, signalName: 'optionsChanged' },
        rejectLabel: { type: String, value: '', signalName: 'rejectLabelChanged' },
        seleectedNameFilter: { type: Var, value: undefined, signalName: 'seleectedNameFilterChanged' },
        nameFilters: { type: Var, value: undefined, signalName: 'nameFiltersChanged' },

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

FileDialog.initialize()

module.exports = FileDialog