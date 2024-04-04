const { Item } = require('./Item')
const { QColor, QBool, QReal, QFont, QVar, QString, QInt } = require('../utils/properties')

class TextInput extends Item {
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

    static Normal = 0
    static Password = 1
    static NoEcho = 2
    static PasswordEchoOnEdit = 3

    static defaultProperties = {
        text: { type: QString, value: '', changed: '$textChanged' },
        color: { type: QColor, value: 'black', changed: '$colorChanged' },
        selectionColor: { type: QColor, value: '#000080', changed: '$selectionColorChanged' },
        selectedTextColor: { type: QColor, value: '#fff', changed: '$selectedTextColorChanged' },
        horizontalAlignment: { type: QReal, value: TextInput.AlignLeft, changed: '$horizontalAlignmentChanged' },
        verticalAlignment: { type: QReal, value: TextInput.AlignTop, changed: '$verticalAlignmentChanged' },
        wrapMode: { type: QReal, value: TextInput.NoWrap, changed: '$wrapModeChanged' },
        echoMode: { type: QReal, value: TextInput.Normal, changed: '$echoModeChanged' },
        readOnly: { type: QBool, value: false, changed: '$readOnlyChanged' },
        acceptableInput: { type: QBool, value: true },
        validator: { type: QVar, value: undefined, changed: '$validatorChanged' },
        font: { type: QFont, changed: '$fontChanged' },
        contentHeight: { type: QReal, value: 0 },
        contentWidth: { type: QReal, value: 0 },
        inputMask: { type: QString, value: '' },
        maximumLength: { type: QInt, value: 32767, changed: '$maximumLengthChanged' },
    }

    static defaultSignals = {
        accepted: { params: [] },
		editingFinished: { params: [] },
		textEdited: { params: [] },
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
        
        this.$form = document.createElement('form')
        this.$form.style.outline = 'none'
        this.$form.style.border = 'none'
        this.$form.style.background = 'none'
        this.$form.style.width = '100%'
        // this.$form.style.height = '1em'
        this.$form.style.font = 'inherit'
        this.$form.style.color = 'inherit'
        this.$form.autocomplete = "new-password"
        this.getDom().appendChild(this.$form)

        this.$input = document.createElement('input')
        this.$input.type = 'text'
        this.$input.style.outline = 'none'
        this.$input.style.border = 'none'
        this.$input.style.background = 'none'
        this.$input.style.width = '100%'
        // this.$input.style.height = '1em'
        this.$input.style.font = 'inherit'
        this.$input.style.color = 'inherit'
        this.$input.style.textAlign = 'inherit'
        this.$input.style.cursor = 'defult'
        this.$input.autocomplete = "new-password"
        this.$input.onfocus = ()=>{
            this.forceActiveFocus()
            // this.getProperty('activeFocus').reset(true)
        }
        this.$form.appendChild(this.$input)
        MouseController.add(this)

        this.$input.oninput = (e)=>{
            if(this.getPropertyValue('validator')){
                if(this.getPropertyValue('validator').hasPartialMatch(this.$input.value)){
                    this.getProperty('text').reset(this.$input.value)
                    // this.getProperty('acceptableInput').reset(true)
                    if(this.$signals.textEdited) this.$signals.textEdited()
                } else {
                    this.$input.value = this.getPropertyValue('text')
                    // this.getProperty('acceptableInput').reset(false)
                }
            } else {
                this.getProperty('text').reset(this.$input.value)
                this.getProperty('acceptableInput').reset(true)
                if(this.$signals.textEdited) this.$signals.textEdited()
            }
        }
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

    $maximumLengthChanged(){
        this.$input.maxlength = this.getPropertyValue('maximumLength')
    }

    $horizontalAlignmentChanged(){
        switch(this.getPropertyValue('horizontalAlignment')){
            case Text.AlignLeft: this.setStyle({ justifyContent: 'flex-start', textAlign: 'left' }); break;
            case Text.AlignRight: this.setStyle({ justifyContent: 'flex-end', textAlign: 'right' }); break;
            case Text.AlignHCenter: this.setStyle({ justifyContent: 'center', textAlign: 'center' }); break;
            case Text.AlignJustify: this.setStyle({ justifyContent: 'normal', textAlign: 'justify' }); break;
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

    $echoModeChanged(){
        switch(this.getPropertyValue('echoMode')){
			case TextInput.Normal: {
				this.$input.type = "text"
				break
            }
			case TextInput.Password:{
				this.$input.type = "password"
				break
            }
		}
    }

    $readOnlyChanged(){
        if(this.getPropertyValue('readOnly')){
            this.$input.readOnly = true
        } else {
            this.$input.readOnly = false
        }
    }

    $validatorChanged(){
        if(this.getPropertyValue('validator')){
            if(this.getPropertyValue('validator').validate(this.getPropertyValue('text'))){
                this.getProperty('acceptableInput').reset(true)
            } else {
                this.getProperty('acceptableInput').reset(false)
            }
        } else {
            this.getProperty('acceptableInput').reset(true)
        }
    }

    $textChanged(){
        this.$input.value = this.getPropertyValue('text')
        this.$validatorChanged()
        this.applyMetrics()
    }

    $fontChanged(){
        this.setStyle({
            fontWeight: this.getProperty('font').getPropertyValue('bold') ? 'bold' : 'normal',
            fontSize: `${this.getProperty('font').getPropertyValue('pixelSize')}px`,
            fontFamily: `${this.getProperty('font').getPropertyValue('family')}`,
        })
        if(this.getPropertyValue('text'))this.applyMetrics()
    }

    $focusChanged(){
        if(this.getPropertyValue('focus')){    
            this.$input.focus()
        } else {  
            this.$input.blur()
        }
        super.$focusChanged()
    }

    $activeFocusChanged(){
        if(this.getPropertyValue('activeFocus')){
            this.$input.focus()
        } else {
            this.$input.blur()
            if(this.$signals.editingFinished) this.$signals.editingFinished()
        }
        super.$activeFocusChanged()
    }

    $visibleChanged(){
        if(this.getPropertyValue('visible') && this.getPropertyValue('activeFocus')){
            this.$input.focus()
        }
    }

    applyMetrics(){
        let textMetrics = TextFontController.measureText(this.getPropertyValue('text'), this.getProperty('font').getPropertyValue('pixelSize'), this.getProperty('font').getPropertyValue('family'), this.getProperty('width').auto ? 0 : this.getProperty('width').get(), this.getPropertyValue('wrapMode'))

        this.getProperty('contentWidth').reset(textMetrics.width)
        // this.getProperty('contentHeight').reset(textMetrics.height)
        this.getProperty('contentHeight').reset(this.$input.scrollHeight)
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

    onKeyDown(key){
        if(key === 'Enter' || key === 'Return'){
            if(this.getPropertyValue('validator')){
                if(this.getPropertyValue('validator').validate(this.$input.value)){
                    if(this.$signals.accepted) this.$signals.accepted()
                    if(this.$signals.editingFinished) this.$signals.editingFinished()
                    this.getProperty('acceptableInput').reset(true)
                } else {
                    this.getProperty('acceptableInput').reset(false)
                }
            } else {
                if(this.$signals.accepted) this.$signals.accepted()
                if(this.$signals.editingFinished) this.$signals.editingFinished()
                this.getProperty('acceptableInput').reset(true)
            }
            
        }
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
        this.$input.remove()
        this.$form.remove()
        MouseController.remove(this)
        super.destroy()     
    }

}

module.exports.TextInput = TextInput