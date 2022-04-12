Item {
    visible: false;
    property bool selectMultiple: false;
    property string defaultSuffix: '';
    property string title: '';
    property var nameFilters: [];
    property var fileUrl: '';
    property var fileUrls: [];
    property string folder: '';
    property bool selectExisting: false;

    signal accepted;
    signal rejected;


    function getTag() { return 'input' }
    onCompleted: {
        this.element.dom.type = 'file'
        this.style('display', 'none')
        this.element.dom.addEventListener('change', this._dialogChanged.bind(this))
    }

    onVisibleChanged: {
        if(this.visible){
            this.element.dom.value = ""
            this.element.dom.click()
            this.visible = false
        }
    }
    onSelectMultipleChanged: {
        this.element.dom.multiple = value ? "multiple" : ""
    }
    onNameFiltersChanged: {
        if(value.length){
            this.element.dom.accept = value.join(',').match(/\.\w+/g).join(',')
        } else {
            this.element.dom.accept = ''
        }
        
    }
    function _dialogChanged(e){
        if(this.selectMultiple){
            //this.fileUrl = e.target.files[0]
            this.fileUrls = e.target.files
        } else {
            this.fileUrl = e.target.files[0]
        }
        
        this.accepted()
    }

    function open(){
        this.accepted()
    }
}