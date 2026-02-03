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
        activeFocusOnPress: { type: QBool, value: true },
        acceptableInput: { type: QBool, value: true },
        validator: { type: QVar, value: undefined, changed: '$validatorChanged' },
        font: { type: QFont, changed: '$fontChanged' },
        contentHeight: { type: QReal, value: 0 },
        contentWidth: { type: QReal, value: 0 },
        inputMask: { type: QString, value: '' },
        maximumLength: { type: QInt, value: 32767, changed: '$maximumLengthChanged' },
        selectionStart: { type: QInt, value: 0 },
        selectionEnd: { type: QInt, value: 0 },
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
        this.$input.style.cursor = 'unset'
        this.$input.onfocus = ()=>{
            if(!this.getPropertyValue('activeFocus') && !this.getPropertyValue('readOnly')) this.forceActiveFocus()
        }
        // this.$input.onblur = ()=>{
        //     this.getProperty('activeFocus').reset(false)
        // }
        this.getDom().appendChild(this.$input)
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

            this.$updateSelection()
        }
    }

    $updateSelection(){
        this.getProperty('selectionStart').reset(this.$input.selectionStart)
        this.getProperty('selectionEnd').reset(this.$input.selectionEnd)
    }

    $colorChanged(){
        let rgba = this.getProperty('color').toRGBA()
        this.setStyle({
            color: `rgba(${rgba.r},${rgba.g},${rgba.b},${this.getProperty('color').get() === 'transparent' ? 0 : rgba.a * this.getPropertyValue('opacity')})`
        })
    }

    // forceActiveFocus(){
    //     this.getProperty('focus').reset(true)
    //     if(!this.getPropertyValue('readOnly')) this.getProperty('activeFocus').reset(true)
    //     this.$input.focus()
    // }

    $selectionColorChanged(){

    }

    $selectedTextColorChanged(){

    }

    $maximumLengthChanged(){
        this.$input.setAttribute('maxLength', this.getPropertyValue('maximumLength'))
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
        this.$updateSelection()
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
        super.$focusChanged()
        if(this.getPropertyValue('focus')){    
            this.$input.focus()
        } else {  
            this.$input.blur()
        }
    }

    $activeFocusChanged(){
        super.$activeFocusChanged()
        if(this.getPropertyValue('activeFocus')){
            this.$input.focus()
        } else {
            this.$input.blur()
            if(this.$signals.editingFinished) this.$signals.editingFinished()
        }
    }

    $visibleChanged(){
        super.$visibleChanged()

        if(this.getPropertyValue('visible') && this.getPropertyValue('activeFocus')){
            this.$input.focus()
        }
    }

    applyMetrics(){
        let textMetrics = TextFontController.measureText(this.getPropertyValue('text'), this.getProperty('font'), this.getProperty('width').auto ? 0 : this.getProperty('width').get(), this.getPropertyValue('wrapMode'))

        this.getProperty('contentWidth').reset(textMetrics.width)
        // this.getProperty('contentHeight').reset(textMetrics.height)
        this.getProperty('contentHeight').reset(this.$input.scrollHeight)
    }

    cut(){
        if(this.getPropertyValue('readOnly')) return
        
        const start = this.$input.selectionStart
        const end = this.$input.selectionEnd
        if(start !== end && navigator.clipboard){
            const selectedText = this.$input.value.substring(start, end)
            navigator.clipboard.writeText(selectedText).then(() => {
                const newText = this.$input.value.substring(0, start) + this.$input.value.substring(end)
                this.getProperty('text').reset(newText)
                this.$input.setSelectionRange(start, start)
                this.$updateSelection()
            }).catch(err => {
                console.error('Failed to cut text:', err)
            })
        }
    }

    copy(){
        const start = this.$input.selectionStart
        const end = this.$input.selectionEnd
        if(start !== end && navigator.clipboard){
            const selectedText = this.$input.value.substring(start, end)
            navigator.clipboard.writeText(selectedText).catch(err => {
                console.error('Failed to copy text:', err)
            })
        }
    }

    paste(){
        if(!this.getPropertyValue('readOnly') && navigator.clipboard){
            navigator.clipboard.readText().then(clipboardText => {
                const start = this.$input.selectionStart
                const end = this.$input.selectionEnd
                const currentText = this.$input.value
                const newText = currentText.substring(0, start) + clipboardText + currentText.substring(end)
                
                // Check validator if present
                if(this.getPropertyValue('validator')){
                    if(!this.getPropertyValue('validator').hasPartialMatch(newText)){
                        return // Don't paste if validation fails
                    }
                }
                
                // Update text and cursor position
                this.getProperty('text').reset(newText)
                const newCursorPos = start + clipboardText.length
                this.$input.setSelectionRange(newCursorPos, newCursorPos)
                this.$updateSelection()
            }).catch(err => {
                console.error('Failed to paste text:', err)
            })
        }
    }

    clear(){
        if(!this.getPropertyValue('readOnly')){
            this.getProperty('text').reset('')
        }
    }

    redo(){
        document.execCommand('redo')
    }

    undo(){
        document.execCommand('undo')
    }

    remove(start, end){
        this.text = this.text.slice(0, start) + this.text.slice(end, this.text.length)
    }

    select(start, end){
        this.$input.setSelectionRange(start, end)
        this.getProperty('selectionStart').reset(start)
        this.getProperty('selectionEnd').reset(end)
    }

    selectAll(){
        this.$input.select()
        this.$updateSelection()
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
        MouseController.remove(this)
        super.destroy()     
    }

}

module.exports.TextInput = TextInput