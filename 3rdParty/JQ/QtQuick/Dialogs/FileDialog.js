const Dialog = require("./Dialog")
const Var = require("../../QtQml/Var")
const String = require("../../QtQml/String")
const Int = require("../../QtQml/Int")
const Signal = require("../../QtQml/Signal")

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
        currentFile: { type: String, value: '', signalName: 'currentFileChanged' },
        currentFiles: { type: Var, value: undefined, signalName: 'currentFilesChanged' },
        fileMode: { type: Int, value: FileDialog.OpenFile, signalName: 'fileModeChanged' },
        selectedFile: { type: Var, value: undefined, signalName: 'selectedFileChanged' },
        selectedFiles: { type: Var, value: undefined, signalName: 'selectedFilesChanged' },
        options: { type: Var, value: undefined, signalName: 'optionsChanged' },
        rejectLabel: { type: String, value: '', signalName: 'rejectLabelChanged' },
        selectedNameFilter: { type: Var, value: undefined, signalName: 'selectedNameFilterChanged' },
        nameFilters: { type: Var, value: undefined, signalName: 'nameFiltersChanged' },

        acceptLabelChanged: {type:Signal, slotName:'onAcceptLabelChanged', args:[]},
        currentFileChanged: {type:Signal, slotName:'onCurrentFileChanged', args:[]},
        currentFilesChanged: {type:Signal, slotName:'onCurrentFilesChanged', args:[]},
        fileModeChanged: {type:Signal, slotName:'onFileModeChanged', args:[]},
        selectedFileChanged: {type:Signal, slotName:'onSelectedFileChanged', args:[]},
        selectedFilesChanged: {type:Signal, slotName:'onSelectedFilesChanged', args:[]},
        optionsChanged: {type:Signal, slotName:'onOptionsChanged', args:[]},
        rejectLabelChanged: {type:Signal, slotName:'onRejectLabelChanged', args:[]},
        selectedNameFilterChanged: {type:Signal, slotName:'onSelectedNameFilterChanged', args:[]},
        nameFiltersChanged: {type:Signal, slotName:'onNameFiltersChanged', args:[]},
    })
}



module.exports = FileDialog