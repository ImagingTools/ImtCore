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
        text: { type: String, value: ''},
        color: { type: Color, value: 'black'},
        selectionColor: { type: Color, value: '#000080'},
        selectedTextColor: { type: Color, value: '#ffffff'},
        horizontalAlignment: { type: Real, value: TextInput.AlignLeft},
        verticalAlignment: { type: Real, value: TextInput.AlignTop},
        wrapMode: { type: Real, value: TextInput.NoWrap},
        echoMode: { type: Real, value: TextInput.Normal},
        readOnly: { type: Bool, value: false},
        activeFocusOnPress: { type: Bool, value: true},
        acceptableInput: { type: Bool, value: true},
        validator: { type: Var, value: undefined},
        font: { type: Font},
        contentHeight: { type: Real, value: 0},
        contentWidth: { type: Real, value: 0},
        inputMask: { type: String, value: ''},
        maximumLength: { type: Int, value: 32767},
        selectionStart: { type: Int, value: 0},
        selectionEnd: { type: Int, value: 0},
        padding: { type: Real, value: 0},
        leftPadding: { type: Real, value: 0},
        topPadding: { type: Real, value: 0},
        rightPadding: { type: Real, value: 0},
        bottomPadding: { type: Real, value: 0},
        selectedText: { type: String, value: ''},
        selectByMouse: { type: Bool, value: true},
        passwordCharacter: { type: String, value: ''},
        cursorPosition: { type: Int, value: 0},

        textChanged: {type:Signal, args:[]},
        colorChanged: {type:Signal, args:[]},
        selectionColorChanged: {type:Signal, args:[]},
        selectedTextColorChanged: {type:Signal, args:[]},
        horizontalAlignmentChanged: {type:Signal, args:[]},
        verticalAlignmentChanged: {type:Signal, args:[]},
        wrapModeChanged: {type:Signal, args:[]},
        echoModeChanged: {type:Signal, args:[]},
        readOnlyChanged: {type:Signal, args:[]},
        activeFocusOnPressChanged: {type:Signal, args:[]},
        acceptableInputChanged: {type:Signal, args:[]},
        validatorChanged: {type:Signal, args:[]},
        fontChanged: {type:Signal, args:[]},
        contentHeightChanged: {type:Signal, args:[]},
        contentWidthChanged: {type:Signal, args:[]},
        inputMaskChanged: {type:Signal, args:[]},
        maximumLengthChanged: {type:Signal, args:[]},
        selectionStartChanged: {type:Signal, args:[]},
        selectionEndChanged: {type:Signal, args:[]},
        paddingChanged: {type:Signal, args:[]},
        leftPaddingChanged: {type:Signal, args:[]},
        topPaddingChanged: {type:Signal, args:[]},
        rightPaddingChanged: {type:Signal, args:[]},
        bottomPaddingChanged: {type:Signal, args:[]},
        selectedTextChanged: {type:Signal, args:[]},
        selectByMouseChanged: {type:Signal, args:[]},
        passwordCharacterChanged: {type:Signal, args:[]},
        cursorPositionChanged: {type:Signal, args:[]},

        accepted: {type:Signal, args:[]},
        editingFinished: {type:Signal, args:[]},
        textEdited: {type:Signal, args:[]},
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
            if(!this.activeFocus) this.activeFocus = true

            if(this.parent instanceof JQModules.QtQuick.FocusScope && !this.parent.activeFocus){
                this.parent.activeFocus = true
            }
        }

        impl.onblur = ()=>{
            this.activeFocus = false
        }

        impl.onkeydown = (e)=>{
            this.__syncSelectionFromDOM()

            if(e.keyCode === QtEnums.Key_C && e.ctrlKey){
                e.preventDefault()
                e.stopPropagation()
                this.copy()
            } else if(e.keyCode === QtEnums.Key_V && e.ctrlKey){
                e.preventDefault()
                e.stopPropagation()
                this.paste()
            } else if(e.keyCode === QtEnums.Key_X && e.ctrlKey){
                e.preventDefault()
                e.stopPropagation()
                this.cut()
            } else if(e.keyCode === QtEnums.Key_A && e.ctrlKey){
                e.preventDefault()
                e.stopPropagation()
                this.selectAll()
            } else if(e.key === QtEnums.Key_Backspace){
                e.preventDefault()
                e.stopPropagation()
                this.__deleteWithDirection(false)
                this.textEdited()
            } else if(e.key === QtEnums.Key_Delete){
                e.preventDefault()
                e.stopPropagation()
                this.__deleteWithDirection(true)
                this.textEdited()
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

        impl.onbeforeinput = ()=>{
            this.__syncSelectionFromDOM()
            this.__beforeInputSelectionStart = this.selectionStart
            this.__beforeInputSelectionEnd = this.selectionEnd
        }

        impl.oninput = (e)=>{
            let inputSelectionStart = Number.isFinite(this.__beforeInputSelectionStart) ? this.__beforeInputSelectionStart : this.selectionStart
            let inputSelectionEnd = Number.isFinite(this.__beforeInputSelectionEnd) ? this.__beforeInputSelectionEnd : this.selectionEnd
            this.__beforeInputSelectionStart = undefined
            this.__beforeInputSelectionEnd = undefined

            let selection = document.getSelection()
            selection.removeAllRanges()

            let buff = this.text.split('') 
            switch(e.inputType){
                case 'insertText': {
                    let previousText = this.text
                    let insertedText = e.data == null ? '' : e.data
                    let result = this.__applyTextInsertWithLimit(inputSelectionStart, inputSelectionEnd, insertedText)
                    this.text = result.newText
                    if(this.text === previousText){
                        this.__renderImplText()
                    }
                    this.select(Math.min(result.cursor, this.text.length), Math.min(result.cursor, this.text.length))
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
                    let cursor = inputSelectionStart
                    if(inputSelectionStart === inputSelectionEnd){
                        if(inputSelectionStart === 0){
                            this.__renderImplText()
                            this.select(0, 0)
                            break
                        }
                        buff.splice(inputSelectionStart-1, 1)
                        cursor = inputSelectionStart-1
                    } else {
                        buff.splice(inputSelectionStart, inputSelectionEnd-inputSelectionStart)
                    }
                    this.text = buff.join('')
                    this.select(cursor, cursor)
                    break
                }
                case 'deleteContentForward': {
                    let cursor = inputSelectionStart
                    if(inputSelectionStart === inputSelectionEnd){
                        if(inputSelectionStart >= buff.length){
                            this.__renderImplText()
                            this.select(inputSelectionStart, inputSelectionStart)
                            break
                        }
                        buff.splice(inputSelectionStart, 1)
                    } else {
                        buff.splice(inputSelectionStart, inputSelectionEnd-inputSelectionStart)
                    }
                    this.text = buff.join('')
                    this.select(cursor, cursor)
                    break
                }
            }

            this.textEdited()
        }

        return impl
    }

    __onMouseDown(mouse){
        if(this.activeFocusOnPress && this.enabled && this.visible && this.__impl){
            this.__impl.focus()
        }
    }

    __setImplStyle(style){
        if(this.__impl) {
            for(let name in style){
                this.__impl.style[name] = style[name]
            }
        }
    }

    __updateGeometry(){
        if(this.__impl){
            let textMetrics = JQApplication.TextController.measureTextFast(this.text, this.font)
            this.contentWidth = textMetrics.width
            this.contentHeight = textMetrics.height
        }
    }

    __checkValidator(){
        if(this.validator){
            if(this.validator.validate(this.text)){
                this.acceptableInput = true
            } else {
                this.acceptableInput = false
            }
        } else {
            this.acceptableInput = true
        }
    }

    __getMaximumLength(){
        let maximumLength = Number.isFinite(this.maximumLength) ? Math.floor(this.maximumLength) : 32767
        if(maximumLength < 0) maximumLength = 0
        return maximumLength
    }

    __applyTextInsertWithLimit(start, end, insertedText){
        let safeStart = Math.max(0, Math.min(start, this.text.length))
        let safeEnd = Math.max(safeStart, Math.min(end, this.text.length))
        let maximumLength = this.__getMaximumLength()
        let availableLength = maximumLength - (this.text.length - (safeEnd - safeStart))
        if(availableLength < 0) availableLength = 0

        let safeInsertedText = insertedText == null ? '' : insertedText.toString()
        if(safeInsertedText.length > availableLength){
            safeInsertedText = safeInsertedText.slice(0, availableLength)
        }

        let newText = this.text.slice(0, safeStart) + safeInsertedText + this.text.slice(safeEnd)
        let cursor = safeStart + safeInsertedText.length
        return { newText, cursor }
    }

    __syncSelectionFromDOM(){
        let selection = document.getSelection()
        if(!selection || !selection.rangeCount){
            this.selectionStart = 0
            this.selectionEnd = 0
            return
        }

        let range = selection.getRangeAt(0)
        if(!this.__impl.contains(range.startContainer) || !this.__impl.contains(range.endContainer)){
            this.selectionStart = 0
            this.selectionEnd = 0
            return
        }

        let start = range.startOffset
        let end = range.endOffset
        if(range.startContainer === this.__impl){
            start = range.startOffset === 0 ? 0 : this.text.length
        }
        if(range.endContainer === this.__impl){
            end = range.endOffset === 0 ? 0 : this.text.length
        }

        this.selectionStart = Math.max(0, Math.min(start, this.text.length))
        this.selectionEnd = Math.max(this.selectionStart, Math.min(end, this.text.length))
    }

    __deleteWithDirection(isForward){
        let start = this.selectionStart
        let end = this.selectionEnd

        if(start === end){
            if(isForward){
                if(start >= this.text.length){
                    this.select(start, start)
                    return
                }
                end = start + 1
            } else {
                if(start === 0){
                    this.select(0, 0)
                    return
                }
                start = start - 1
            }
        }

        this.text = this.text.slice(0, start) + this.text.slice(end, this.text.length)
        this.select(start, start)
    }

    SLOT_readOnlyChanged(oldValue, newValue){
        this.__impl.setAttribute('contenteditable', !newValue)
    }

    SLOT_validatorChanged(oldValue, newValue){
        this.__checkValidator()
    }

    SLOT_maximumLengthChanged(oldValue, newValue){
        let maximumLength = this.__getMaximumLength()
        if(this.text.length > maximumLength){
            this.text = this.text.slice(0, maximumLength)
            this.select(maximumLength, maximumLength)
        }
    }

    SLOT_focusChanged(oldValue, newValue){
        super.SLOT_focusChanged(oldValue, newValue)
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
        if(!newValue){
            this.__impl.blur()

            if(this.validator){
                if(this.validator.validate(this.text)) this.editingFinished()
            } else {
                this.editingFinished()
            }
            
        }
    }

    SLOT_echoModeChanged(oldValue, newValue){
        if(this.text === ''){
            this.__impl.innerHTML = '&#8203'
            return
        }

        if(this.echoMode === TextInput.Password){
            this.__impl.innerText = this.text.replaceAll(/./g, this.passwordCharacter.length ? this.passwordCharacter[0] : '●')
        } else {
            this.__impl.innerText = this.text
        }
    }

    __renderImplText(){
        if(this.text === ''){
            this.__impl.innerHTML = '&#8203'
            return
        }

        if(this.echoMode === TextInput.Password){
            this.__impl.innerText = this.text.replaceAll(/./g, this.passwordCharacter.length ? this.passwordCharacter[0] : '●')
        } else {
            this.__impl.innerText = this.text
        }
    }

    SLOT_textChanged(oldValue, newValue){
        let maximumLength = this.__getMaximumLength()
        if(this.text.length > maximumLength){
            this.text = this.text.slice(0, maximumLength)
            return
        }

        this.__renderImplText()

        this.__checkValidator()

        this.__updateGeometry()
    }

    SLOT_colorChanged(oldValue, newValue){
        let rgba = Color.getRGBA(this.__proxy, 'color', this.__self.constructor.meta.color)
        this.__setDOMStyle({
            color: `rgba(${rgba.r},${rgba.g},${rgba.b},${this.__proxy.color === 'transparent' ? 0 : rgba.a * this.opacity})`
        })
    }

    SLOT_visibleChanged(oldValue, newValue){
        super.SLOT_visibleChanged(oldValue, newValue)

        if(newValue && this.activeFocus){
            this.__impl.focus()
        }
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
        let result = this.__applyTextInsertWithLimit(position, position, text)
        this.text = result.newText
    }
    isRightToLeft(start, end){

    }
    moveCursorSelection(position, mode){

    }
    paste(){
        navigator.clipboard.readText().then((text) => {
            let result = this.__applyTextInsertWithLimit(this.selectionStart, this.selectionEnd, text)
            this.text = result.newText
            this.select(result.cursor, result.cursor)
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
        this.selectedText = this.text.substring(start, end)
    }
    selectAll(){
        this.select(0, this.text.length)
    }
    selectWord(){

    }
    undo(){

    }

    __onMouseDown(mouse){
        if(!this.enabled || !this.visible) return

        if(!mouse.target){
            mouse.target = this
        }
    }
    __onMouseUp(mouse){
        if(!this.enabled || !this.visible) return

        if(mouse.target === this){
            mouse.target = null
        }
    }

    __destroy(){
        JQApplication.MouseController.remove(this)
        super.__destroy()
    }
}



module.exports = TextInput