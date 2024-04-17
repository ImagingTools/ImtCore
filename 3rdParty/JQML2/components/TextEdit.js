const { Item } = require('./Item')
const { QColor, QBool, QReal, QFont, QVar, QString } = require('../utils/properties')

class TextEdit extends Item {
    static AlignLeft = 0
    static AlignRight = 1
    static AlignHCenter = 2
    static AlignJustify = 3
    static AlignTop = 4
    static AlignBottom = 5
    static AlignVCenter = 6

    static NoWrap = 0
    static WordWrap = 1
    static WrapAnywhere = 2
    static Wrap = 3

    static defaultProperties = {
        text: { type: QString, value: '', changed: '$textChanged' },
        color: { type: QColor, value: 'black', changed: '$colorChanged' },
        selectionColor: { type: QColor, value: '#000080', changed: '$selectionColorChanged' },
        selectedTextColor: { type: QColor, value: '#fff', changed: '$selectedTextColorChanged' },
        horizontalAlignment: { type: QReal, value: TextEdit.AlignLeft, changed: '$horizontalAlignmentChanged' },
        verticalAlignment: { type: QReal, value: TextEdit.AlignTop, changed: '$verticalAlignmentChanged' },
        wrapMode: { type: QReal, value: TextEdit.NoWrap, changed: '$wrapModeChanged' },
        readOnly: { type: QBool, value: false, changed: '$readOnlyChanged' },
        font: { type: QFont, changed: '$fontChanged' },
        contentHeight: { type: QReal, value: 0 },
        contentWidth: { type: QReal, value: 0 },
        paintedHeight: { type: QReal, value: 0 },
        paintedWidth: { type: QReal, value: 0 },
        cursorRectangle: { type: QVar, value: undefined },
    }

    static defaultSignals = {
		editingFinished: { params: [] },
    }

    $textinput = true

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)

        this.setStyle({
            fontSize: '14px',
            fontFamily: 'Segoe UI',
            pointerEvents: 'auto',
            touchAction: 'auto',
        })

        this.impl = document.createElement('span')
        this.impl.style.position = 'absolute'
        this.impl.style.left = '0'
        this.impl.style.top = '0'
        this.impl.style.right = '0'
        this.impl.style.bottom = '0'
        this.impl.style.opacity = '0'
        this.getDom().appendChild(this.impl)
        
        this.$input = document.createElement('textarea')
        this.$input.style.outline = 'none'
        this.$input.style.border = 'none'
        this.$input.style.background = 'none'
        this.$input.style.width = '100%'
        this.$input.style.maxHeight = '100%'
        this.$input.style.height = '100%'
        this.$input.style.font = 'inherit'
        this.$input.style.color = 'inherit'
        this.$input.style.zIndex = 'inherit'
        this.$input.style.resize = 'none'
        this.$input.style.cursor = 'unset'
        this.getDom().appendChild(this.$input)
        MouseController.add(this)

        this.$input.oninput = (e)=>{
            this.getProperty('text').reset(this.$input.value)
        }

        this.getProperty('width').setCompute(()=>{return this.getProperty('contentWidth').get()})
        this.getProperty('height').setCompute(()=>{return Math.max(this.getProperty('contentHeight').get(), this.getProperty('font').getProperty('pixelSize').get())})
    }

    cut(){

    }

    copy(){
        
    }

    paste(){
        
    }

    clear(){
        
    }

    redo(){
        
    }

    undo(){
        
    }

    remove(start, end){

    }

    select(start, end){

    }

    selectAll(){
        this.$input.select()
    }

    $colorChanged(){
        let rgba = this.getProperty('color').toRGBA()
        this.setStyle({
            color: `rgba(${rgba.r},${rgba.g},${rgba.b},${this.getProperty('color').get() === 'transparent' ? 0 : rgba.a * this.getPropertyValue('opacity')})`
        })
    }

    forceActiveFocus(){
        super.forceActiveFocus()
        this.getProperty('activeFocus').reset(true)
        this.$input.focus()
    }

    $selectionColorChanged(){

    }

    $selectedTextColorChanged(){

    }

    $horizontalAlignmentChanged(){
        switch(this.getPropertyValue('horizontalAlignment')){
            case Text.AlignLeft: this.setStyle({ justifyContent: 'flex-start' }); break;
            case Text.AlignRight: this.setStyle({ justifyContent: 'flex-end' }); break;
            case Text.AlignHCenter: this.setStyle({ justifyContent: 'center' }); break;
            case Text.AlignJustify: this.setStyle({ justifyContent: 'normal' }); break;
        }
    }

    $verticalAlignmentChanged(){
        switch(this.getPropertyValue('verticalAlignment')){
            case Text.AlignTop: this.setStyle({ alignItems: 'flex-start' }); break;
            case Text.AlignBottom: this.setStyle({ alignItems: 'flex-end' }); break;
            case Text.AlignVCenter: this.setStyle({ alignItems: 'center' }); break;
        }
    }

    $wrapModeChanged(){

    }

    $readOnlyChanged(){
        if(this.getPropertyValue('readOnly')){
            this.$input.readOnly = true
        } else {
            this.$input.readOnly = false
        }
    }

    $textChanged(){
        this.$input.value = this.getPropertyValue('text')
        this.applyMetrics()
    }

    $fontChanged(){
        this.setStyle({
            fontWeight: this.getProperty('font').getPropertyValue('bold') ? 'bold' : 'normal',
            fontSize: `${this.getProperty('font').getPropertyValue('pixelSize')}px`,
            fontFamily: `${this.getProperty('font').getPropertyValue('family')}`,
        })
        
        this.applyMetrics()
    }

    $focusChanged(){
        if(this.getPropertyValue('focus')){
            this.$input.focus()
        } else {
            this.$input.blur()
        }
        super.$focusChanged()
    }

    $visibleChanged(){
        if(this.getPropertyValue('visible') && this.getPropertyValue('activeFocus')){
            this.$input.focus()
        }
    }

    applyMetrics(){
        let isHTML = false
        if(this.getPropertyValue('textFormat') === Text.AutoText){
            let regexp = /<[^<>]+>/g
            if(this.getPropertyValue('text') && (regexp.test(this.getPropertyValue('text')) || this.getPropertyValue('text').indexOf('\n') >= 0)){
                isHTML = true
            } else {
                isHTML = false
            }
        } else if(this.getPropertyValue('textFormat') === Text.PlainText){
            isHTML = false
        } else {
            isHTML = true
        }

        if(isHTML){
            this.impl.innerHTML = this.getPropertyValue('text').replaceAll('\n', '<br>') + '.'
            this.updateGeometry()

        } else {
            let textMetrics = TextFontController.measureText(this.getPropertyValue('text'), this.getProperty('font'), this.getProperty('width').auto ? 0 : this.getProperty('width').get(), this.getPropertyValue('wrapMode'))
            
            this.impl.innerHTML = this.getPropertyValue('text').replaceAll('\n', '<br>') + '.'
            this.getProperty('width').setAuto(textMetrics.width)
            this.getProperty('height').setAuto(textMetrics.height)

            this.getProperty('contentWidth').reset(textMetrics.width)
            this.getProperty('contentHeight').reset(textMetrics.height)
            this.getProperty('paintedWidth').reset(textMetrics.width)
            this.getProperty('paintedHeight').reset(textMetrics.height)
            
        }


        
    }

    updateGeometry(){
        this.setStyle({
            minHeight: 0,
            height: 0,
        })
        this.getProperty('width').setAuto(this.impl.scrollWidth)
        this.getProperty('height').setAuto(Math.max(this.impl.scrollHeight, this.getProperty('font').getProperty('pixelSize').get()))
        
        this.getProperty('contentWidth').reset(this.impl.scrollWidth)
        this.getProperty('contentHeight').reset(Math.max(this.impl.scrollHeight, this.getProperty('font').getProperty('pixelSize').get()))
        this.getProperty('paintedWidth').reset(this.impl.scrollWidth)
        this.getProperty('paintedHeight').reset(Math.max(this.impl.scrollHeight, this.getProperty('font').getProperty('pixelSize').get()))

        this.setStyle({
            minHeight: `${this.getPropertyValue('height')}px`,
            height: `${this.getPropertyValue('height')}px`,
        })
    }

    onKeyDown(key){

    }

    onWheel(x, y, deltaX, deltaY){
        if(this.getPropertyValue('enabled')) {
            
        }
    }

    onDoubleClick(x, y, button){
        if(this.getPropertyValue('enabled')) {
            let btn = 0
            switch(button){
                case 0: btn = Qt.LeftButton; break;
                case 1: btn = Qt.MiddleButton; break;
                case 2: btn = Qt.RightButton; break;
            }	
            if(Qt.LeftButton & btn) {
                this.getProperty('focus').reset(true)
                this.getProperty('activeFocus').reset(true)
                this.$input.focus()
                return true
            }
            return false
        }
    }

    
    onMouseDown(x, y, button){
        if(this.getPropertyValue('enabled')) {
            let btn = 0
            switch(button){
                case 0: btn = Qt.LeftButton; break;
                case 1: btn = Qt.MiddleButton; break;
                case 2: btn = Qt.RightButton; break;
            }	
            if(Qt.LeftButton & btn) {
                this.getProperty('focus').reset(true)
                this.getProperty('activeFocus').reset(true)
                this.$input.focus()
                return true
            }
            return false
        }
    }
    onMouseUp(x, y, button){
        if(this.getPropertyValue('enabled')) {
            this.$input.focus()
        } 
    }
    onMouseMove(x, y){
        if(this.getPropertyValue('enabled')) {
            this.getPropertyValue('context').setStyle({
                cursor: 'text'
            })
        }
    }

    destroy(){
        this.impl.remove()
        this.$input.remove()
        MouseController.remove(this)
        super.destroy()     
    }

}

module.exports.TextEdit = TextEdit