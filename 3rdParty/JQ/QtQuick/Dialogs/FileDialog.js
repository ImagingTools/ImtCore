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
        acceptLabel: { type: String, value: ''},
        currentFile: { type: String, value: ''},
        currentFiles: { type: Var, value: undefined},
        fileMode: { type: Int, value: FileDialog.OpenFile},
        selectedFile: { type: Var, value: ''},
        selectedFiles: { type: Var, value: undefined},
        options: { type: Var, value: undefined},
        rejectLabel: { type: String, value: ''},
        selectedNameFilter: { type: Var, value: undefined},
        nameFilters: { type: Var, value: undefined},

        acceptLabelChanged: {type:Signal, args:[]},
        currentFileChanged: {type:Signal, args:[]},
        currentFilesChanged: {type:Signal, args:[]},
        fileModeChanged: {type:Signal, args:[]},
        selectedFileChanged: {type:Signal, args:[]},
        selectedFilesChanged: {type:Signal, args:[]},
        optionsChanged: {type:Signal, args:[]},
        rejectLabelChanged: {type:Signal, args:[]},
        selectedNameFilterChanged: {type:Signal, args:[]},
        nameFiltersChanged: {type:Signal, args:[]},
    })

    nameFilters = []

    __change(event){
        if(this.fileMode === FileDialog.OpenFiles){
            for(let fileUrl of event.target.files){
                fileUrl.toString = ()=>{return fileUrl}
                fileUrl.replace = ()=>{return fileUrl}
                fileUrl.split = (delimiter)=>{return fileUrl.name.split(delimiter)}
            }

            this.selectedFiles = event.target.selectedFiles
        } else {
            let fileUrl = event.target.files[0]
            fileUrl.toString = ()=>{return fileUrl}
            fileUrl.replace = ()=>{return fileUrl}
            fileUrl.split = (delimiter)=>{return fileUrl.name.split(delimiter)}

            this.selectedFile = selectedFileUrl
        }
        
        this.accepted()
    }

    SLOT_fileModeChanged(oldValue, newValue){
        this.__DOM.multiple = (newValue === FileDialog.OpenFiles ? "multiple" : "")
    }

    SLOT_nameFiltersChanged(oldValue, newValue){
        if(newValue.length){
            let matchResult = newValue.join(',').match(/\.\w+/g)
            if(matchResult){
                this.__DOM.accept = matchResult.join(',')
            } else {
                this.__DOM.accept = ''
            }
            
        } else {
            this.__DOM.accept = ''
        }
    }
    SLOT_visibleChanged(oldValue, newValue){
        if(newValue){
            this.__DOM.value = ""
            this.__DOM.click()
            this.visible = false
        }
    }

    open(){
        if(this.fileMode === FileDialog.SaveFile) {
            this.accepted()
        } else {
            this.visible = true
        }
        
    }
}



module.exports = FileDialog