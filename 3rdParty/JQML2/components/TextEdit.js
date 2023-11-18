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
    }

    static defaultSignals = {
		editingFinished: { params: [] },
    }

    constructor(parent){
        super(parent)
        
        this.$input = document.createElement('textarea')
        this.$input.style.outline = 'none'
        this.$input.style.border = 'none'
        this.$input.style.background = 'none'
        this.$input.style.width = '100%'
        this.$input.style.font = 'inherit'
        this.getDom().appendChild(this.$input)
        MouseController.add(this)
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
        this.getProperty('contentHeight').reset(textMetrics.height)
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

module.exports.TextEdit = TextEdit