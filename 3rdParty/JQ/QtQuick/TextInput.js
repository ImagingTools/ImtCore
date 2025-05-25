const Item = require('./Item')
const Bool = require('../QtQml/Bool')
const Var = require('../QtQml/Var')
const Int = require('../QtQml/Int')
const String = require('../QtQml/String')
const Color = require('../QtQml/Color')
const Real = require('../QtQml/Real')
const Font = require('../QtQml/Font')
const Signal = require('../QtQml/Signal')
const QtEnums = require('../Qt/enums')
const JQApplication = require("../core/JQApplication")

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

    static meta = Object.assign({}, Item.meta, {
        text: { type: String, value: '', signalName: 'textChanged' },
        color: { type: Color, value: 'black', signalName: 'colorChanged' },
        selectionColor: { type: Color, value: '#000080', signalName: 'selectionColorChanged' },
        selectedTextColor: { type: Color, value: '#ffffff', signalName: 'selectedTextColorChanged' },
        horizontalAlignment: { type: Real, value: TextInput.AlignLeft, signalName: 'horizontalAlignmentChanged' },
        verticalAlignment: { type: Real, value: TextInput.AlignTop, signalName: 'verticalAlignmentChanged' },
        wrapMode: { type: Real, value: TextInput.NoWrap, signalName: 'wrapModeChanged' },
        echoMode: { type: Real, value: TextInput.Normal, signalName: 'echoModeChanged' },
        readOnly: { type: Bool, value: false, signalName: 'readOnlyChanged' },
        activeFocusOnPress: { type: Bool, value: true, signalName: 'activeFocusOnPressChanged' },
        acceptableInput: { type: Bool, value: true, signalName: 'acceptableInputChanged' },
        validator: { type: Var, value: undefined, signalName: 'validatorChanged' },
        font: { type: Font, signalName: 'fontChanged' },
        contentHeight: { type: Real, value: 0, signalName: 'contentHeightChanged' },
        contentWidth: { type: Real, value: 0, signalName: 'contentWidthChanged' },
        inputMask: { type: String, value: '', signalName: 'inputMaskChanged' },
        maximumLength: { type: Int, value: 32767, signalName: 'maximumLengthChanged' },
        selectionStart: { type: Int, value: 0, signalName: 'selectionStartChanged' },
        selectionEnd: { type: Int, value: 0, signalName: 'selectionEndChanged' },
        padding: { type: Real, value: 0, signalName: 'paddingChanged' },
        leftPadding: { type: Real, value: 0, signalName: 'leftPaddingChanged' },
        topPadding: { type: Real, value: 0, signalName: 'topPaddingChanged' },
        rightPadding: { type: Real, value: 0, signalName: 'rightPaddingChanged' },
        bottomPadding: { type: Real, value: 0, signalName: 'bottomPaddingChanged' },
        selectedText: { type: String, value: '', signalName: 'selectedTextChanged' },
        selectByMouse: { type: Bool, value: true, signalName: 'selectByMouseChanged' },

        textChanged: {type:Signal, slotName:'onTextChanged', args:[]},
        colorChanged: {type:Signal, slotName:'onColorChanged', args:[]},
        selectionColorChanged: {type:Signal, slotName:'onSelectionColorChanged', args:[]},
        selectedTextColorChanged: {type:Signal, slotName:'onSelectedTextColorChanged', args:[]},
        horizontalAlignmentChanged: {type:Signal, slotName:'onHorizontalAlignmentChanged', args:[]},
        verticalAlignmentChanged: {type:Signal, slotName:'onVerticalAlignmentChanged', args:[]},
        wrapModeChanged: {type:Signal, slotName:'onWrapModeChanged', args:[]},
        echoModeChanged: {type:Signal, slotName:'onEchoModeChanged', args:[]},
        readOnlyChanged: {type:Signal, slotName:'onReadOnlyChanged', args:[]},
        activeFocusOnPressChanged: {type:Signal, slotName:'onActiveFocusOnPressChanged', args:[]},
        acceptableInputChanged: {type:Signal, slotName:'onAcceptableInputChanged', args:[]},
        validatorChanged: {type:Signal, slotName:'onValidatorChanged', args:[]},
        fontChanged: {type:Signal, slotName:'onFontChanged', args:[]},
        contentHeightChanged: {type:Signal, slotName:'onContentHeightChanged', args:[]},
        contentWidthChanged: {type:Signal, slotName:'onContentWidthChanged', args:[]},
        inputMaskChanged: {type:Signal, slotName:'onInputMaskChanged', args:[]},
        maximumLengthChanged: {type:Signal, slotName:'onMaximumLengthChanged', args:[]},
        selectionStartChanged: {type:Signal, slotName:'onSelectionStartChanged', args:[]},
        selectionEndChanged: {type:Signal, slotName:'onSelectionEndChanged', args:[]},
        paddingChanged: {type:Signal, slotName:'onPaddingChanged', args:[]},
        leftPaddingChanged: {type:Signal, slotName:'onLeftPaddingChanged', args:[]},
        topPaddingChanged: {type:Signal, slotName:'onTopPaddingChanged', args:[]},
        rightPaddingChanged: {type:Signal, slotName:'onRightPaddingChanged', args:[]},
        bottomPaddingChanged: {type:Signal, slotName:'onBottomPaddingChanged', args:[]},
        selectedTextChanged: {type:Signal, slotName:'onSelectedTextChanged', args:[]},
        selectByMouseChanged: {type:Signal, slotName:'onSelectByMouseChanged', args:[]},

        accepted: {type:Signal, slotName:'onAccepted', args:[]},
        editingFinished: {type:Signal, slotName:'onEditingFinished', args:[]},
        textEdited: {type:Signal, slotName:'onTextEdited', args:[]},
    })

    static create(parent, ...args){
        let obj = super.create(parent, ...args)
        obj.__DOM.classList.add('TextInput')
        obj.__createImpl()
        JQApplication.MouseController.add(obj)
        return obj
    }

    __createImpl(){
        let dom = this.__getDOM()
        let impl = document.createElement('div')
        impl.classList.add('impl')
        impl.innerHTML = '&#8203'
        dom.appendChild(impl)
        this.__impl = impl

        impl.setAttribute('contenteditable', true)

        impl.onfocus = ()=>{
            this.forceActiveFocus()
        }

        impl.onblur = ()=>{
            this.activeFocus = false
        }

        impl.onkeydown = (e)=>{
            let selection = document.getSelection()

            if(selection.rangeCount && this.text !== ''){
                let range = selection.getRangeAt(0)

                this.selectionStart = range.startOffset
                this.selectionEnd = range.endOffset
            } else {
                this.selectionStart = 0
                this.selectionEnd = 0
            }

            if(e.code === QtEnums.Key_C && e.ctrlKey){
                e.preventDefault()
                e.stopPropagation()
                this.copy()
            } else if(e.code === QtEnums.Key_V && e.ctrlKey){
                e.preventDefault()
                e.stopPropagation()
                this.paste()
            } else if(e.code === QtEnums.Key_X && e.ctrlKey){
                e.preventDefault()
                e.stopPropagation()
                this.cut()
            } else if(e.code === QtEnums.Key_A && e.ctrlKey){
                e.preventDefault()
                e.stopPropagation()
                this.selectAll()
            } else if(e.key === QtEnums.Key_Enter){
                e.preventDefault()
                e.stopPropagation()

                if(this.validator){
                    if(this.validator.validate(this.text)){
                        this.accepted()
                        this.editingFinished()
                        this.acceptableInput = true
                    } else {
                        this.acceptableInput = false
                    }
                } else {
                    this.accepted()
                    this.editingFinished()
                    this.acceptableInput = true
                }
            }
        }
        impl.oninput = (e)=>{
            let selection = document.getSelection()
            selection.removeAllRanges()

            let buff = this.text.split('') 
            switch(e.inputType){
                case 'insertText': {
                    buff.splice(this.selectionStart, this.selectionEnd-this.selectionStart, e.data)
                    this.text = buff.join('')
                    this.select(this.selectionEnd+1-(this.selectionEnd-this.selectionStart), this.selectionEnd+1-(this.selectionEnd-this.selectionStart))
                    break
                }
                case 'insertFromPaste': {
                    this.paste()
                    break
                }
                case 'deleteByCut': {
                    this.cut()
                    break
                }
                case 'deleteContentBackward': {
                    let data = []
                    if(this.selectionStart === this.selectionEnd){
                        data = buff.splice(this.selectionStart-1, this.selectionEnd-(this.selectionStart-1))
                    } else {
                        data = buff.splice(this.selectionStart, this.selectionEnd-this.selectionStart)
                    }
                    this.text = buff.join('')
                    this.select(this.selectionEnd-data.length, this.selectionEnd-data.length)
                    break
                }
                case 'deleteContentForward': {
                    let data = []
                    if(this.selectionStart === this.selectionEnd){
                        data = buff.splice(this.selectionStart, this.selectionEnd+1-this.selectionStart)
                    } else {
                        data = buff.splice(this.selectionStart, this.selectionEnd-this.selectionStart)
                    }
                    this.text = buff.join('')
                    let offset = data.length > 1 ? data.length : 0
                    this.select(this.selectionEnd-offset, this.selectionEnd-offset)
                    break
                }
            }

            this.textEdited()
        }

        return impl
    }

    __setImplStyle(style){
        if(this.__impl) {
            for(let name in style){
                this.__impl.style[name] = style[name]
            }
        }
    }

    __updateGeometry(){
        // let text = this.text
        // if(text){
        //     if(text[text.length-1] === '\n') text += '.'
        // } else {
        //     text = '.'
        // }

        // let textMetrics = JQApplication.TextController.measureText(text, this.font, this.__getObject('width').__auto ? 0 : this.width, this.wrapMode, 0)
        
        // // let textMetrics = this.__impl.getBoundingClientRect()

        // this.__getObject('width').__setAuto(this.__impl.scrollWidth)
        // this.__getObject('height').__setAuto(this.__impl.scrollHeight)

        // this.contentWidth = textMetrics.width
        // this.contentHeight = textMetrics.height
        // this.paintedWidth = textMetrics.width
        // this.paintedHeight = textMetrics.height
        
    }

    SLOT_focusChanged(oldValue, newValue){
        super.SLOT_focusChanged()
        if(this.focus){
            if(!(this.parent instanceof JQModules.QtQuick.FocusScope)){
                this.activeFocus = true
            }
        }
    }

    SLOT_horizontalAlignmentChanged(oldValue, newValue){
        switch(this.horizontalAlignment){
            case TextInput.AlignLeft: {
                this.__setImplStyle({
                    alignItems: 'flex-start'
                })
                break
            }
            case TextInput.AlignRight: {
                this.__setImplStyle({
                    alignItems: 'flex-end'
                })
                break
            }
            case TextInput.AlignHCenter: {
                this.__setImplStyle({
                    alignItems: 'center'
                })
                break
            }
            case TextInput.AlignJustify: {
                this.__setImplStyle({
                    alignItems: 'normal'
                })
                break
            }
        }
    }

    SLOT_verticalAlignmentChanged(oldValue, newValue){
        switch(this.verticalAlignment){
            case TextInput.AlignTop: {
                this.__setImplStyle({
                    justifyContent: 'flex-start'
                })
                break
            }
            case TextInput.AlignBottom: {
                this.__setImplStyle({
                    justifyContent: 'flex-end'
                })
                break
            }
            case TextInput.AlignVCenter: {
                this.__setImplStyle({
                    justifyContent: 'center'
                })
                break
            }
            case TextInput.AlignJustify: {
                this.__setImplStyle({
                    justifyContent: 'normal'
                })
                break
            }
        }
    }

    SLOT_activeFocusChanged(oldValue, newValue){
        if(!this.activeFocus){
            this.__impl.blur()
            this.editingFinished()
        }
    }

    SLOT_echoModeChanged(oldValue, newValue){
        if(this.text === ''){
            this.__impl.innerHTML = '&#8203'
            return
        }

        if(this.echoMode === TextInput.Password){
            this.__impl.innerText = this.text.replaceAll(/./g, '*')
        } else {
            this.__impl.innerText = this.text
        }
    }

    SLOT_textChanged(oldValue, newValue){
        if(this.text === ''){
            this.__impl.innerHTML = '&#8203'
            return
        }

        if(this.echoMode === TextInput.Password){
            this.__impl.innerText = this.text.replaceAll(/./g, '*')
        } else {
            this.__impl.innerText = this.text
        }

        this.__updateGeometry()
    }

    onFontChanged(oldValue, newValue){
        this.__setDOMStyle({
            fontWeight: this.font.bold == true ? 'bold' : 'normal',
            fontSize: this.font.pixelSize+'px',
            fontFamily: `'${this.font.family}'`,
            textDecoration: this.font.underline == true ? 'underline' : 'unset',
        })

        this.__updateGeometry()
    }

    clear(){
        this.text = ''
    }
    copy(){
        navigator.clipboard.writeText(this.text.slice(this.selectionStart, this.selectionEnd))
    }
    cut(){
        let text = this.text.slice(this.selectionStart, this.selectionEnd)
        navigator.clipboard.writeText(text).then(()=>{
            this.text = this.text.slice(0, this.selectionStart) + this.text.slice(this.selectionEnd, this.text.length)
            this.select(this.selectionStart, this.selectionStart)
        })
    }
    deselect(){
        document.getSelection().removeAllRanges()
    }
    ensureVisible(position){

    }
    getText(start, end){
        return this.text.slice(start, end)
    }
    insert(position, text){
        this.text = this.text.slice(0, position) + text + this.text.slice(position, this.text.length)
    }
    isRightToLeft(start, end){

    }
    moveCursorSelection(position, mode){

    }
    paste(){
        navigator.clipboard.readText().then((text) => {
            let buff = this.text.split('')
            let replaced = buff.splice(this.selectionStart, this.selectionEnd-this.selectionStart, text)
            this.text = buff.join('')
            this.select(this.selectionEnd-replaced.length+text.length, this.selectionEnd-replaced.length+text.length)
        })
    }
    positionAt(x, y, position){

    }
    positionToRectangle(pos){

    }
    redo(){

    }
    remove(start, end){
        this.text = this.text.slice(0, start) + this.text.slice(end, this.text.length)
    }
    select(start, end){
        let selection = document.getSelection()
        selection.removeAllRanges()
        let range = new Range()
        if(this.__impl.childNodes.length){
            range.setStart(this.__impl.childNodes[0], start)
            range.setEnd(this.__impl.childNodes[0], end)
        } else {
            range.setStart(this.__impl, 0)
            range.setEnd(this.__impl, 0)
        }
        selection.addRange(range)
        this.selectionStart = start
        this.selectionEnd = end
    }
    selectAll(){
        this.select(0, this.text.length)
    }
    selectWord(){

    }
    undo(){

    }

    __destroy(){
        JQApplication.MouseController.remove(this)
        super.__destroy()
    }
}



module.exports = TextInput