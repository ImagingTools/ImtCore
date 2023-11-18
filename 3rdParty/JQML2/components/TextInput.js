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
        validator: { type: QVar, value: undefined },
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

    constructor(parent){
        super(parent)
        
        this.$input = document.createElement('input')
        this.$input.type = 'text'
        this.$input.style.outline = 'none'
        this.$input.style.border = 'none'
        this.$input.style.background = 'none'
        this.$input.style.width = '100%'
        this.$input.style.height = '1em'
        this.$input.style.font = 'inherit'
        this.$input.style.color = 'inherit'
        this.getDom().appendChild(this.$input)
        MouseController.add(this)

        this.$input.oninput = (e)=>{
            if(this.getPropertyValue('validator')){
                if(this.getPropertyValue('validator').validate(this.$input.value)){
                    this.getProperty('text').reset(this.$input.value)
                    this.getProperty('acceptableInput') .reset(true)
                    if(this.$signals.textEdited) this.$signals.textEdited()
                } else {
                    this.$input.value = this.getPropertyValue('text')
                    this.getProperty('acceptableInput') .reset(false)
                }
            } else {
                this.getProperty('text').reset(this.$input.value)
                this.getProperty('acceptableInput') .reset(true)
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

    $selectionColorChanged(){

    }

    $selectedTextColorChanged(){

    }

    $maximumLengthChanged(){
        this.$input.maxlength = this.getPropertyValue('maximumLength')
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

    $textChanged(){
        this.$input.value = this.getPropertyValue('text')
        this.applyMetrics()
    }

    $fontChanged(){
        if(this.getPropertyValue('text')){
            this.setStyle({
                fontWeight: this.getProperty('font').getPropertyValue('bold') ? 'bold' : 'normal',
                fontSize: this.getProperty('font').getPropertyValue('pixelSize'),
            })
            this.applyMetrics()
        } else {
            this.setStyle({
                fontWeight: this.getProperty('font').getPropertyValue('bold') ? 'bold' : 'normal',
                fontSize: this.getProperty('font').getPropertyValue('pixelSize'),
            })
        }
    }

    applyMetrics(){
        let textMetrics = TextFontController.measureText(this.getPropertyValue('text'), this.getProperty('font').getPropertyValue('pixelSize'), this.getProperty('font').getPropertyValue('family'), this.getProperty('width').auto ? 0 : this.getProperty('width').get(), this.getPropertyValue('wrapMode'))

        this.getProperty('contentWidth').reset(textMetrics.width)
        // this.getProperty('contentHeight').reset(textMetrics.height)
        this.getProperty('contentHeight').reset(this.$input.scrollHeight)
    }

    selectAll(){
        this.$input.select()
    }

    onWheel(x, y, deltaX, deltaY){
        if(this.getPropertyValue('enabled')) {
            
        }
    }

    onMouseDown(x, y, button){
        if(this.getPropertyValue('enabled')) {
            
        }
    }
    onMouseUp(x, y, button){
        if(this.getPropertyValue('enabled')) {
            this.$input.focus()
        } 
    }
    onMouseMove(x, y){
        if(this.getPropertyValue('enabled')) {
           
        }
    }

    $destroy(){
        this.$input.remove()
        MouseController.remove(this)
        super.$destroy()     
    }
}

module.exports.TextInput = TextInput