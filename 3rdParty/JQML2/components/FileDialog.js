const { Item } = require('./Item')
const { QVar, QBool, QString, QInt } = require('../utils/properties')

class FileDialog extends Item {
    static OpenFile = 0
    static OpenFiles = 1
    static SaveFile = 2

    static DontResolveSymlinks = 0
    static DontConfirmOverwrite = 0
    static ReadOnly = 0
    static HideNameFilterDetails = 0

    static defaultProperties = {
        selectMultiple: { type: QBool, value: false },
        defaultSuffix: { type: QString, value: '' },
        title: { type: QString, value: '' },
        nameFilters: { type: QVar, value: undefined },
        fileUrl: { type: QString, value: '' },
        fileUrls: { type: QVar, value: undefined },
        folder: { type: QString, value: '' },
        selectExisting: { type: QBool, value: false },
        shortcuts: { type: QVar, value: undefined },


        acceptLabel: { type: QString, value: '' },
        currentFile: { type: QString, value: '' },
        currentFiles: { type: QVar, value: undefined },
        fileMode: { type: QInt, value: FileDialog.OpenFile },
        file: { type: QString, value: '' },
        files: { type: QVar, value: undefined },
        options: { type: QVar, value: undefined },
        rejectLabel: { type: QString, value: '' },
        seleectedNameFilter: { type: QVar, value: undefined },
    }


    static defaultSignals = {
        accepted: { params: [] },
        rejected: { params: [] },


    }

    // not implemented yet
 
}

module.exports.FileDialog = FileDialog