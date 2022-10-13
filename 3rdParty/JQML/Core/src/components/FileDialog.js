import {Item} from './Item'
import {Signal} from '../utils/Signal'

export class FileDialog extends Item {
    constructor(parent) {
        super(parent)
        this.visible = false

        // visible: false;
        // property bool selectMultiple: false;
        // property string defaultSuffix: '';
        // property string title: '';
        // property var nameFilters: [];
        // property var fileUrl: '';
        // property var fileUrls: [];
        // property string folder: '';
        // property bool selectExisting: false;
    
        // signal accepted;
        // signal rejected;

        this.$cP('selectMultiple', false).connect(this.$selectMultipleChanged.bind(this))
        this.$cP('defaultSuffix', '')
        this.$cP('title', '')
        this.$cP('nameFilters', []).connect(this.$nameFiltersChanged.bind(this))
        this.$cP('fileUrl', '')
        this.$cP('fileUrls', [])
        this.$cP('folder', '')
        this.$cP('selectExisting', false)


        this.$s.accepted = Signal()
        this.$s.rejected = Signal()
    }

    $domCreate(){
        super.$domCreate()
        this.dom.style.display = 'none'
        this.impl = document.createElement("input")
        this.impl.type = 'file'
        this.dom.appendChild(this.impl)
        this.impl.addEventListener('change', this.$dialogChanged.bind(this))
    }

    $show(){

    }
    $selectMultipleChanged(){
        this.impl.multiple = value ? "multiple" : ""
    }
    $nameFiltersChanged(){
        if(this.nameFilters.length){
            this.impl.accept = this.nameFilters.join(',').match(/\.\w+/g).join(',')
        } else {
            this.impl.accept = ''
        }
    }
    $visibleChanged(){
        if(this.visible){
            this.impl.value = ""
            this.impl.click()
            this.visible = false
        }
    }
    $dialogChanged(e){
        if(this.selectMultiple){
            this.fileUrls = e.target.files
        } else {
            this.fileUrl = e.target.files[0]
        }
        
        this.$s.accepted()
    }

    open(){
        this.$s.accepted()
    }

    $destroy(){
        this.impl.remove()
        super.$destroy()
    }
}

QML.FileDialog = FileDialog