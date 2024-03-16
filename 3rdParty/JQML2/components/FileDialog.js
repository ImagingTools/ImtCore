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
        fileUrl: { type: QString, value: '' },
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

    constructor(parent,exCtx,exModel) {
        super(parent,exCtx,exModel)

        this.setStyle({
            display: 'none'
        })
        
        this.$input = document.createElement('input')
        this.$input.type = 'file'
        this.getDom().appendChild(this.$input)

        this.$input.addEventListener('change', ()=>{
            if(this.getPropertyValue('selectMultiple')){
                this.getProperty('fileUrls').reset(e.target.files)
                this.getProperty('files').reset(e.target.files)
                for(let fileUrl of e.target.files){
                    fileUrl.toString = ()=>{return this.getPropertyValue('fileUrl')}
                    fileUrl.replace = ()=>{return this.getPropertyValue('fileUrl')}
                }
            } else {
                let fileUrl = e.target.files[0]
                fileUrl.toString = ()=>{return this.getPropertyValue('fileUrl')}
                fileUrl.replace = ()=>{return this.getPropertyValue('fileUrl')}
                this.getProperty('fileUrl').reset(fileUrl)
                this.getProperty('file').reset(fileUrl)
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
            this.$input.accept = nameFilters.join(',').match(/\.\w+/g).join(',')
        } else {
            this.$input.accept = ''
        }
    }
    $visibleChanged(){
        if(this.getPropertyValue('visible')){
            this.$input.value = ""
            this.$input.click()
            this.getProperty('visible').reset(false)
        }
    }

    open(){
        if(this.$signals.accepted) this.$signals.accepted()
    }

    destroy(){
        this.$input.remove()
        super.destroy()
    }
 
}

module.exports.FileDialog = FileDialog