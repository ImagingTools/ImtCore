const { Item } = require('./Item')
const { QVar, QBool, QString, QInt, QVisibleNot } = require('../utils/properties')

class FileDialog extends Item {
    static OpenFile = 0
    static OpenFiles = 1
    static SaveFile = 2

    static DontResolveSymlinks = 0
    static DontConfirmOverwrite = 0
    static ReadOnly = 0
    static HideNameFilterDetails = 0

    static defaultProperties = {
        visible: { type: QVisibleNot, value: false, changed: '$visibleChanged' },

        selectMultiple: { type: QBool, value: false, changed: '$selectMultipleChanged' },
        defaultSuffix: { type: QString, value: '' },
        title: { type: QString, value: '' },
        nameFilters: { type: QVar, value: undefined, changed: '$nameFiltersChanged' },
        fileUrl: { type: QVar, value: '' },
        fileUrls: { type: QVar, value: undefined },
        folder: { type: QString, value: '' },
        selectExisting: { type: QBool, value: false },
        shortcuts: { type: QVar, value: {
            'desktop': '',
            'documents': '',
            'home': '',
            'music': '',
            'movies': '',
            'pictures': '',
        } },

        acceptLabel: { type: QString, value: '' },
        currentFile: { type: QString, value: '' },
        currentFiles: { type: QVar, value: undefined },
        fileMode: { type: QInt, value: FileDialog.OpenFile },
        file: { type: QVar, value: '' },
        files: { type: QVar, value: undefined },
        options: { type: QVar, value: undefined },
        rejectLabel: { type: QString, value: '' },
        seleectedNameFilter: { type: QVar, value: undefined },

        selectedFile: { type: QVar, value: '' },
        selectedFiles: { type: QVar, value: undefined },
    }


    static defaultSignals = {
        accepted: { params: [] },
        rejected: { params: [] },


    }

    constructor(parent,exCtx,exModel) {
        super(parent,exCtx,exModel)

        this.setStyle({
            display: 'none'
        })
        
        this.$input = document.createElement('input')
        this.$input.type = 'file'
        this.getDom().appendChild(this.$input)

        this.$dom.setAttribute('unvisible', '')
        this.$dom.removeAttribute('visible', '')

        this.$input.addEventListener('change', (e)=>{
            if(this.getPropertyValue('selectMultiple')){
                this.getProperty('fileUrls').reset(e.target.files)
                this.getProperty('files').reset(e.target.files)
                this.getProperty('selectedFiles').reset(e.target.files)
                for(let fileUrl of e.target.files){
                    fileUrl.toString = ()=>{return fileUrl}
                    fileUrl.replace = ()=>{return fileUrl}
                    fileUrl.split = (delimiter)=>{return fileUrl.name.split(delimiter)}
                }
            } else {
                let fileUrl = e.target.files[0]
                fileUrl.toString = ()=>{return fileUrl}
                fileUrl.replace = ()=>{return fileUrl}
                fileUrl.split = (delimiter)=>{return fileUrl.name.split(delimiter)}
                this.getProperty('fileUrl').reset(fileUrl)
                this.getProperty('file').reset(fileUrl)
                this.getProperty('selectedFile').reset(fileUrl)
            }
            
            if(this.$signals.accepted) this.$signals.accepted()
        })
    }

    $show(){

    }
    $selectMultipleChanged(){
        this.$input.multiple = this.getPropertyValue('selectMultiple') ? "multiple" : ""
    }
    $nameFiltersChanged(){
        let nameFilters = this.getPropertyValue('nameFilters')
        if(nameFilters.length){
            let matchResult = nameFilters.join(',').match(/\.\w+/g)
            if(matchResult){
                this.$input.accept = matchResult.join(',')
            } else {
                this.$input.accept = ''
            }
            
        } else {
            this.$input.accept = ''
        }
    }
    $visibleChanged(){
        if(this.getPropertyValue('visible')){
            if(this.$dom) {
                this.$dom.setAttribute('visible', '')
                this.$dom.removeAttribute('unvisible', '')
            }

            this.$input.value = ""
            this.$input.click()
            this.getProperty('visible').reset(false)
        } else {
            if(this.$dom) {
                this.$dom.setAttribute('unvisible', '')
                this.$dom.removeAttribute('visible', '')
            }
        }
    }

    open(){
        if(this.getPropertyValue('fileMode') === FileDialog.SaveFile) {
            if(this.$signals.accepted) this.$signals.accepted()
        } else {
            this.getProperty('visible').reset(true)
        }
        
    }

    destroy(){
        this.$input.remove()
        super.destroy()
    }
 
}

module.exports.FileDialog = FileDialog