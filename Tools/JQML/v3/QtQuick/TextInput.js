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

    static MaximumUndoDepth = 100

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
        canUndo: { type: Bool, value: false},
        canRedo: { type: Bool, value: false},

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
        canUndoChanged: {type:Signal, args:[]},
        canRedoChanged: {type:Signal, args:[]},

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
        this.__clearUndoHistory()

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

            if(this.__isShortcutKey(e, QtEnums.Key_Z)){
                e.preventDefault()
                e.stopPropagation()
                if(e.shiftKey){
                    this.redo()
                } else {
                    this.undo()
                }
            } else if(this.__isShortcutKey(e, QtEnums.Key_Y) && !e.shiftKey){
                e.preventDefault()
                e.stopPropagation()
                this.redo()
            } else if(this.__isShortcutKey(e, QtEnums.Key_C) || this.__isShortcutKey(e, QtEnums.Key_V) || this.__isShortcutKey(e, QtEnums.Key_X)){
                // Handled by the native copy/paste/cut events (see impl.oncopy, impl.onpaste, impl.oncut)
                e.stopPropagation()
            } else if(this.__isShortcutKey(e, QtEnums.Key_A)){
                e.preventDefault()
                e.stopPropagation()
                this.selectAll()
            } else if(e.key === QtEnums.Key_Delete && e.shiftKey && !e.ctrlKey && !e.altKey && !e.metaKey){
                e.preventDefault()
                e.stopPropagation()
                this.cut()
            } else if(e.key === QtEnums.Key_Backspace){
                e.preventDefault()
                e.stopPropagation()
                if(this.__deleteWithDirection(false, this.selectionStart, this.selectionEnd)) this.textEdited()
            } else if(e.key === QtEnums.Key_Delete){
                e.preventDefault()
                e.stopPropagation()
                if(this.__deleteWithDirection(true, this.selectionStart, this.selectionEnd)) this.textEdited()
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

        impl.oncopy = (e)=>{
            e.preventDefault()
            this.__syncSelectionFromDOM()
            if(this.echoMode !== TextInput.Normal || !e.clipboardData) return

            let text = this.text.slice(this.selectionStart, this.selectionEnd)
            if(text) e.clipboardData.setData('text/plain', text)
        }

        impl.oncut = (e)=>{
            e.preventDefault()
            this.__syncSelectionFromDOM()
            if(this.readOnly || this.echoMode !== TextInput.Normal || !e.clipboardData) return
            if(this.selectionStart === this.selectionEnd) return

            e.clipboardData.setData('text/plain', this.text.slice(this.selectionStart, this.selectionEnd))
            this.__replaceText(this.selectionStart, this.selectionEnd, '', '')
            this.textEdited()
        }

        impl.onpaste = (e)=>{
            e.preventDefault()
            if(this.readOnly) return

            this.__syncSelectionFromDOM()
            let text = e.clipboardData ? e.clipboardData.getData('text/plain') : ''
            if(this.__pasteText(text, this.selectionStart, this.selectionEnd)) this.textEdited()
        }

        impl.onbeforeinput = (e)=>{
            if(e.inputType === 'historyUndo' || e.inputType === 'historyRedo'){
                e.preventDefault()
                if(e.inputType === 'historyUndo'){
                    this.undo()
                } else {
                    this.redo()
                }
                return
            }

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

            switch(e.inputType){
                case 'insertText': {
                    let insertedText = e.data == null ? '' : e.data
                    this.__replaceText(inputSelectionStart, inputSelectionEnd, insertedText, 'insert')
                    break
                }
                case 'insertFromPaste': {
                    let pastedText = e.data
                    if(pastedText == null && e.dataTransfer) pastedText = e.dataTransfer.getData('text/plain')
                    this.__replaceText(inputSelectionStart, inputSelectionEnd, this.__normalizeInsertedText(pastedText), '')
                    break
                }
                case 'deleteByCut': {
                    this.__replaceText(inputSelectionStart, inputSelectionEnd, '', '')
                    break
                }
                case 'deleteContentBackward': {
                    this.__deleteWithDirection(false, inputSelectionStart, inputSelectionEnd)
                    break
                }
                case 'deleteContentForward': {
                    this.__deleteWithDirection(true, inputSelectionStart, inputSelectionEnd)
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

    __deleteWithDirection(isForward, start, end){
        if(start === end){
            if(isForward){
                if(start >= this.text.length){
                    this.__renderImplText()
                    this.select(start, start)
                    return false
                }
                end = start + 1
            } else {
                if(start === 0){
                    this.__renderImplText()
                    this.select(0, 0)
                    return false
                }
                start = start - 1
            }
        }

        let previousText = this.text
        this.__replaceText(start, end, '', isForward ? 'deleteForward' : 'deleteBackward')
        return this.text !== previousText
    }

    __isShortcutKey(e, qtKey){
        if(!(e.ctrlKey || e.metaKey) || e.altKey) return false
        if(e.keyCode === qtKey) return true

        // Non-latin keyboard layouts: shortcuts use the physical key
        return typeof e.code === 'string' && /^Key[A-Z]$/.test(e.code) && e.code.charCodeAt(3) === qtKey
    }

    __normalizeInsertedText(text){
        if(text == null) return ''
        return text.toString().replace(/\r\n|\r|\n/g, ' ')
    }

    __replaceText(start, end, insertedText, undoMergeKind){
        let result = this.__applyTextInsertWithLimit(start, end, insertedText)
        if(result.newText !== this.text){
            this.__pushUndoState(undoMergeKind)
            this.__setTextInternal(result.newText)
        } else {
            this.__renderImplText()
        }
        this.__restoreCursorAfterEdit(result)
        this.__undoMergeCursor = this.selectionEnd
        return result
    }

    __pasteText(text, start, end){
        let insertedText = this.__normalizeInsertedText(text)
        if(!insertedText && start === end) return false

        let previousText = this.text
        this.__replaceText(start, end, insertedText, '')
        return this.text !== previousText
    }

    __writeClipboardText(text){
        if(typeof navigator !== 'undefined' && navigator.clipboard && navigator.clipboard.writeText){
            navigator.clipboard.writeText(text).catch(()=>{})
            return
        }

        // Fallback for non-secure contexts where navigator.clipboard is not available
        let handler = (e)=>{
            e.preventDefault()
            e.stopImmediatePropagation()
            if(e.clipboardData) e.clipboardData.setData('text/plain', text)
        }
        document.addEventListener('copy', handler, true)
        try {
            document.execCommand('copy')
        } catch (e) {
        } finally {
            document.removeEventListener('copy', handler, true)
        }
    }

    __setTextInternal(text){
        this.__internalTextChange = true
        try {
            this.text = text
        } finally {
            this.__internalTextChange = false
        }
    }

    __pushUndoState(undoMergeKind){
        if(!this.__undoStack) this.__clearUndoHistory()

        let canMerge = undoMergeKind
            && undoMergeKind === this.__undoMergeKind
            && this.__undoStack.length > 0
            && this.selectionStart === this.selectionEnd
            && this.selectionStart === this.__undoMergeCursor

        if(!canMerge){
            this.__undoStack.push({ text: this.text, selectionStart: this.selectionStart, selectionEnd: this.selectionEnd })
            if(this.__undoStack.length > TextInput.MaximumUndoDepth) this.__undoStack.shift()
        }

        this.__redoStack = []
        this.__undoMergeKind = undoMergeKind
        this.__updateUndoRedoState()
    }

    __applyHistoryState(state){
        this.__setTextInternal(state.text)
        let start = Math.max(0, Math.min(state.selectionStart, this.text.length))
        let end = Math.max(start, Math.min(state.selectionEnd, this.text.length))
        this.select(start, end)
        this.__undoMergeKind = ''
        this.__updateUndoRedoState()
    }

    __clearUndoHistory(){
        this.__undoStack = []
        this.__redoStack = []
        this.__undoMergeKind = ''
        this.__undoMergeCursor = -1
        this.__updateUndoRedoState()
    }

    __updateUndoRedoState(){
        let canUndo = this.__undoStack.length > 0
        let canRedo = this.__redoStack.length > 0
        if(this.canUndo !== canUndo) this.canUndo = canUndo
        if(this.canRedo !== canRedo) this.canRedo = canRedo
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

    forceActiveFocus(){
        super.forceActiveFocus()
        this.__ensureDomFocus()
    }

    __ensureDomFocus(){
        if(!this.__impl || !this.enabled || !this.visible) return
        if(typeof document === 'undefined') return

        if(document.activeElement !== this.__impl){
            this.__impl.focus()
        }

        if(this.selectionStart !== this.selectionEnd){
            try {
                this.select(this.selectionStart, this.selectionEnd)
            } catch (e) {}
        }
    }

    SLOT_activeFocusChanged(oldValue, newValue){
        if(newValue){
            this.__ensureDomFocus()
            return
        }

        if(this.__impl && typeof document !== 'undefined' && document.activeElement === this.__impl){
            this.__impl.blur()
        }

        if(this.validator){
            if(this.validator.validate(this.text)) this.editingFinished()
        } else {
            this.editingFinished()
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

    __syncCursorToEnd(){
        let pos = this.text.length
        this.cursorPosition = pos
        this.selectionStart = pos
        this.selectionEnd = pos
        this.selectedText = ''

        if(!this.__impl) return
        if(!this.activeFocus && typeof document !== 'undefined' && document.activeElement !== this.__impl) return

        this.select(pos, pos)
    }

    __restoreCursorAfterEdit(result){
        let cursor = result && this.text === result.newText ? result.cursor : this.text.length
        cursor = Math.max(0, Math.min(cursor, this.text.length))
        this.select(cursor, cursor)
    }

    SLOT_textChanged(oldValue, newValue){
        let maximumLength = this.__getMaximumLength()
        if(this.text.length > maximumLength){
            this.text = this.text.slice(0, maximumLength)
            return
        }

        if(!this.__internalTextChange) this.__clearUndoHistory()

        this.__renderImplText()
        this.__syncCursorToEnd()

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
        if(this.echoMode !== TextInput.Normal) return

        let text = this.text.slice(this.selectionStart, this.selectionEnd)
        if(text) this.__writeClipboardText(text)
    }
    cut(){
        if(this.readOnly || this.echoMode !== TextInput.Normal) return

        let start = this.selectionStart
        let end = this.selectionEnd
        if(start === end) return

        this.__writeClipboardText(this.text.slice(start, end))
        this.__replaceText(start, end, '', '')
        this.textEdited()
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
        if(result.newText === this.text) return

        this.__pushUndoState('')
        this.__setTextInternal(result.newText)
    }
    isRightToLeft(start, end){

    }
    moveCursorSelection(position, mode){

    }
    paste(){
        if(this.readOnly) return
        if(typeof navigator === 'undefined' || !navigator.clipboard || !navigator.clipboard.readText) return

        let start = this.selectionStart
        let end = this.selectionEnd
        navigator.clipboard.readText().then((text) => {
            if(this.__pasteText(text, start, end)) this.textEdited()
        }).catch(()=>{})
    }
    positionAt(x, y, position){

    }
    positionToRectangle(pos){

    }
    redo(){
        if(this.readOnly || !this.__redoStack || !this.__redoStack.length) return

        this.__undoStack.push({ text: this.text, selectionStart: this.selectionStart, selectionEnd: this.selectionEnd })
        this.__applyHistoryState(this.__redoStack.pop())
        this.textEdited()
    }
    remove(start, end){
        let newText = this.text.slice(0, start) + this.text.slice(end, this.text.length)
        if(newText === this.text) return

        this.__pushUndoState('')
        this.__setTextInternal(newText)
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
        if(this.readOnly || !this.__undoStack || !this.__undoStack.length) return

        this.__redoStack.push({ text: this.text, selectionStart: this.selectionStart, selectionEnd: this.selectionEnd })
        this.__applyHistoryState(this.__undoStack.pop())
        this.textEdited()
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