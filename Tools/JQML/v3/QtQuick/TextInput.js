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
        let impl = document.createElement('input')
        impl.classList.add('impl')
        impl.type = 'text'
        impl.spellcheck = false
        impl.value = this.text || ''
        dom.appendChild(impl)
        this.__impl = impl

        this.__applyEchoMode()
        this.__applyReadOnly()
        this.__applyEnabled()
        this.__applyMaximumLength()
        this.__applyHorizontalAlignment()
        this.__applyVerticalAlignment()
        this.__applySelectionColors()

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

        impl.onmousedown = (e)=>{
            if(!this.activeFocusOnPress) e.preventDefault()
        }

        impl.onmouseup = ()=>{
            if(!this.selectByMouse && !this.__programmaticSelect && this.__impl.selectionStart !== this.__impl.selectionEnd){
                let pos = this.__impl.selectionEnd
                this.__impl.setSelectionRange(pos, pos)
            }
            this.__syncSelectionFromDOM()
        }

        impl.onkeydown = (e)=>{
            if(this.__isEditShortcut(e)){
                e.stopPropagation()
                return
            }

            if(e.key === 'Backspace' || e.key === 'Delete' || e.key === 'Home' || e.key === 'End'){
                e.stopPropagation()
            }

            if(e.key === 'Delete' && e.shiftKey && !e.ctrlKey && !e.altKey && !e.metaKey){
                e.preventDefault()
                e.stopPropagation()
                this.cut()
                return
            }

            if(e.key === 'Enter'){
                e.preventDefault()
                e.stopPropagation()
                this.__acceptInput()
            }
        }

        impl.oncopy = (e)=>{
            if(this.echoMode === TextInput.Normal) return
            e.preventDefault()
        }

        impl.oncut = (e)=>{
            if(!this.readOnly && this.echoMode === TextInput.Normal) return
            e.preventDefault()
        }

        impl.oninput = (e)=>{
            if(this.__settingImplValue) return

            let changed = this.__applyDomValue()
            if(this.__programmaticInput) return

            this.__noteUserEdit(e)
            if(changed) this.textEdited()
        }

        impl.onchange = ()=>{
            if(this.__applyDomValue()){
                this.canUndo = true
                this.canRedo = false
                this.textEdited()
            }
        }

        impl.onanimationstart = (e)=>{
            if(!e || e.animationName !== 'jqml-autofill-start') return
            if(this.__applyDomValue()){
                this.canUndo = true
                this.canRedo = false
                this.textEdited()
            }
        }

        this.__onSelectionChange = ()=>{
            if(typeof document === 'undefined') return
            if(document.activeElement !== this.__impl) return
            this.__syncSelectionFromDOM()
        }
        document.addEventListener('selectionchange', this.__onSelectionChange)

        return impl
    }

    __onMouseDown(mouse){
        if(!this.enabled || !this.visible) return

        if(!mouse.target) mouse.target = this

        if(this.activeFocusOnPress && this.__impl){
            this.__impl.focus()
        }
    }

    __onMouseUp(mouse){
        if(!this.enabled || !this.visible) return

        if(mouse.target === this){
            mouse.target = null
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

    __isShortcutKey(e, qtKey){
        if(!(e.ctrlKey || e.metaKey) || e.altKey) return false
        if(e.keyCode === qtKey) return true

        return typeof e.code === 'string' && /^Key[A-Z]$/.test(e.code) && e.code.charCodeAt(3) === qtKey
    }

    __isEditShortcut(e){
        return this.__isShortcutKey(e, QtEnums.Key_Z)
            || (this.__isShortcutKey(e, QtEnums.Key_Y) && !e.shiftKey)
            || this.__isShortcutKey(e, QtEnums.Key_A)
            || this.__isShortcutKey(e, QtEnums.Key_C)
            || this.__isShortcutKey(e, QtEnums.Key_V)
            || this.__isShortcutKey(e, QtEnums.Key_X)
    }

    __normalizeInsertedText(text){
        if(text == null) return ''
        return text.toString().replace(/\r\n|\r|\n/g, ' ')
    }

    __isPasswordEcho(){
        return this.echoMode === TextInput.Password || this.echoMode === TextInput.PasswordEchoOnEdit
    }

    __applyEchoMode(){
        let impl = this.__impl
        if(!impl) return

        let value = this.text || ''
        let start = impl.selectionStart
        let end = impl.selectionEnd
        let nextType = this.__isPasswordEcho() ? 'password' : 'text'

        impl.style.color = ''
        impl.style.caretColor = ''

        if(impl.type !== nextType) impl.type = nextType

        if(this.echoMode === TextInput.NoEcho){
            impl.style.color = 'transparent'
            impl.style.caretColor = this.__caretColor || 'black'
        }

        if(impl.type === 'password'){
            impl.setAttribute('autocapitalize', 'off')
            impl.setAttribute('autocorrect', 'off')
        } else {
            impl.removeAttribute('autocapitalize')
            impl.removeAttribute('autocorrect')
        }

        if(impl.value !== value){
            this.__settingImplValue = true
            try {
                impl.value = value
            } finally {
                this.__settingImplValue = false
            }
        }

        if(this.activeFocus && Number.isFinite(start) && Number.isFinite(end)){
            try {
                impl.setSelectionRange(start, end)
            } catch (e) {}
        }
    }

    __applyReadOnly(){
        if(this.__impl) this.__impl.readOnly = !!this.readOnly
    }

    __applyEnabled(){
        if(this.__impl) this.__impl.disabled = !this.enabled
    }

    __applyMaximumLength(){
        if(!this.__impl) return
        this.__impl.maxLength = this.__getMaximumLength()
    }

    __applyHorizontalAlignment(){
        let textAlign = 'left'
        switch(this.horizontalAlignment){
            case TextInput.AlignRight: {
                textAlign = 'right'
                break
            }
            case TextInput.AlignHCenter: {
                textAlign = 'center'
                break
            }
            case TextInput.AlignJustify: {
                textAlign = 'justify'
                break
            }
        }
        this.__setImplStyle({ textAlign: textAlign })
    }

    __applyVerticalAlignment(){
        if(!this.__impl) return

        let height = this.height || 0
        let fontSize = this.font && this.font.pixelSize ? this.font.pixelSize : 14
        let extra = Math.max(0, height - fontSize)
        let paddingTop = 0
        let paddingBottom = extra

        if(this.verticalAlignment === TextInput.AlignBottom){
            paddingTop = extra
            paddingBottom = 0
        } else if(this.verticalAlignment === TextInput.AlignVCenter || this.verticalAlignment === TextInput.AlignJustify){
            paddingTop = Math.floor(extra / 2)
            paddingBottom = extra - paddingTop
        }

        this.__setImplStyle({
            paddingTop: paddingTop + 'px',
            paddingBottom: paddingBottom + 'px',
        })
    }

    __applySelectionColors(){
        if(!this.__impl) return

        let selection = Color.getRGBA(this.__proxy, 'selectionColor', this.__self.constructor.meta.selectionColor)
        let selectedText = Color.getRGBA(this.__proxy, 'selectedTextColor', this.__self.constructor.meta.selectedTextColor)
        this.__impl.style.setProperty('--jq-selection-color', `rgba(${selection.r},${selection.g},${selection.b},${selection.a})`)
        this.__impl.style.setProperty('--jq-selected-text-color', `rgba(${selectedText.r},${selectedText.g},${selectedText.b},${selectedText.a})`)
    }

    __applyDomValue(){
        if(!this.__impl || this.__settingImplValue) return false

        let value = this.__normalizeInsertedText(this.__impl.value)
        if(value === this.text){
            this.__syncSelectionFromDOM()
            return false
        }

        this.__fromDom = true
        try {
            this.text = value
        } finally {
            this.__fromDom = false
        }
        this.__syncSelectionFromDOM()
        return true
    }

    __noteUserEdit(e){
        let inputType = e && e.inputType
        if(inputType === 'historyUndo'){
            this.canRedo = true
            return
        }
        if(inputType === 'historyRedo'){
            this.canUndo = true
            return
        }
        this.canUndo = true
        this.canRedo = false
    }

    __setImplValue(value, moveCursorToEnd){
        let impl = this.__impl
        if(!impl) return

        this.__settingImplValue = true
        try {
            impl.value = value
        } finally {
            this.__settingImplValue = false
        }

        if(!moveCursorToEnd) return

        let pos = value.length
        try {
            impl.setSelectionRange(pos, pos)
        } catch (e) {}

        this.__syncingSelection = true
        try {
            this.cursorPosition = pos
            this.selectionStart = pos
            this.selectionEnd = pos
            this.selectedText = ''
        } finally {
            this.__syncingSelection = false
        }
    }

    __syncSelectionFromDOM(){
        if(!this.__impl || this.__syncingSelection) return

        let start = this.__impl.selectionStart
        let end = this.__impl.selectionEnd
        if(!Number.isFinite(start) || !Number.isFinite(end)) return
        if(start > end){
            let tmp = start
            start = end
            end = tmp
        }

        start = Math.max(0, Math.min(start, this.text.length))
        end = Math.max(start, Math.min(end, this.text.length))

        this.__syncingSelection = true
        try {
            this.selectionStart = start
            this.selectionEnd = end
            this.selectedText = this.text.substring(start, end)
            this.cursorPosition = end
        } finally {
            this.__syncingSelection = false
        }
    }

    __replaceRange(start, end, text){
        let impl = this.__impl
        if(!impl) return

        let current = impl.value || ''
        let safeStart = Math.max(0, Math.min(start, current.length))
        let safeEnd = Math.max(safeStart, Math.min(end, current.length))
        let maximumLength = this.__getMaximumLength()
        let availableLength = maximumLength - (current.length - (safeEnd - safeStart))
        if(availableLength < 0) availableLength = 0

        let inserted = this.__normalizeInsertedText(text)
        if(inserted.length > availableLength) inserted = inserted.slice(0, availableLength)

        this.__programmaticInput = true
        try {
            if(typeof impl.setRangeText === 'function'){
                impl.setRangeText(inserted, safeStart, safeEnd, 'end')
            } else {
                impl.value = current.slice(0, safeStart) + inserted + current.slice(safeEnd)
                let pos = safeStart + inserted.length
                impl.setSelectionRange(pos, pos)
                impl.dispatchEvent(new Event('input', { bubbles: true }))
            }
        } finally {
            this.__programmaticInput = false
        }

        if(impl.value !== this.text){
            this.__fromDom = true
            try {
                this.text = impl.value
            } finally {
                this.__fromDom = false
            }
        }
        this.__syncSelectionFromDOM()
    }

    __insertAtSelection(text){
        if(!this.__impl) return
        let start = this.__impl.selectionStart
        let end = this.__impl.selectionEnd
        if(!Number.isFinite(start)) start = this.text.length
        if(!Number.isFinite(end)) end = start
        this.__replaceRange(start, end, text)
    }

    __writeClipboardText(text){
        if(typeof navigator !== 'undefined' && navigator.clipboard && navigator.clipboard.writeText){
            navigator.clipboard.writeText(text).catch(()=>{})
            return
        }

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

    __acceptInput(){
        if(this.validator){
            if(!this.validator.validate(this.text)){
                this.acceptableInput = false
                return
            }
            this.acceptableInput = true
        } else {
            this.acceptableInput = true
        }

        this.accepted()
        this.editingFinished()
    }

    __colorCss(name){
        let rgba = Color.getRGBA(this.__proxy, name, this.__self.constructor.meta[name])
        let alpha = this.__proxy[name] === 'transparent' ? 0 : rgba.a
        return `rgba(${rgba.r},${rgba.g},${rgba.b},${alpha})`
    }

    SLOT_readOnlyChanged(oldValue, newValue){
        this.__applyReadOnly()
    }

    SLOT_enabledChanged(oldValue, newValue){
        super.SLOT_enabledChanged(oldValue, newValue)
        this.__applyEnabled()
    }

    SLOT_validatorChanged(oldValue, newValue){
        this.__checkValidator()
    }

    SLOT_maximumLengthChanged(oldValue, newValue){
        this.__applyMaximumLength()
        let maximumLength = this.__getMaximumLength()
        if(this.text.length > maximumLength){
            this.text = this.text.slice(0, maximumLength)
            this.select(maximumLength, maximumLength)
        }
    }

    SLOT_echoModeChanged(oldValue, newValue){
        this.__applyEchoMode()
    }

    SLOT_focusChanged(oldValue, newValue){
        super.SLOT_focusChanged(oldValue, newValue)
    }

    SLOT_horizontalAlignmentChanged(oldValue, newValue){
        this.__applyHorizontalAlignment()
    }

    SLOT_verticalAlignmentChanged(oldValue, newValue){
        this.__applyVerticalAlignment()
    }

    SLOT_selectionColorChanged(oldValue, newValue){
        this.__applySelectionColors()
    }

    SLOT_selectedTextColorChanged(oldValue, newValue){
        this.__applySelectionColors()
    }

    SLOT_cursorPositionChanged(oldValue, newValue){
        if(this.__syncingSelection || !this.__impl) return
        let pos = Math.max(0, Math.min(Number(newValue) || 0, this.text.length))
        this.select(pos, pos)
    }

    SLOT_heightChanged(oldValue, newValue){
        super.SLOT_heightChanged(oldValue, newValue)
        this.__applyVerticalAlignment()
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

    SLOT_textChanged(oldValue, newValue){
        let maximumLength = this.__getMaximumLength()
        if(this.text.length > maximumLength){
            this.text = this.text.slice(0, maximumLength)
            return
        }

        if(this.__impl && this.__impl.value !== this.text){
            this.__setImplValue(this.text, true)
            if(!this.__fromDom){
                this.canUndo = false
                this.canRedo = false
            }
        }

        this.__checkValidator()
        this.__updateGeometry()
    }

    SLOT_colorChanged(oldValue, newValue){
        this.__caretColor = this.__colorCss('color')
        this.__setDOMStyle({
            color: this.__caretColor
        })
        if(this.echoMode === TextInput.NoEcho && this.__impl){
            this.__impl.style.caretColor = this.__caretColor
        }
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

        this.__applyVerticalAlignment()
        this.__updateGeometry()
    }

    clear(){
        this.text = ''
    }
    copy(){
        if(this.echoMode !== TextInput.Normal) return

        this.__syncSelectionFromDOM()
        let text = this.text.slice(this.selectionStart, this.selectionEnd)
        if(text) this.__writeClipboardText(text)
    }
    cut(){
        if(this.readOnly || this.echoMode !== TextInput.Normal) return

        this.__syncSelectionFromDOM()
        let start = this.selectionStart
        let end = this.selectionEnd
        if(start === end) return

        this.__writeClipboardText(this.text.slice(start, end))
        let previousText = this.text
        this.__replaceRange(start, end, '')
        if(this.text !== previousText) this.textEdited()
    }
    deselect(){
        let pos = this.__impl && Number.isFinite(this.__impl.selectionEnd) ? this.__impl.selectionEnd : this.cursorPosition
        this.select(pos, pos)
    }
    ensureVisible(position){

    }
    getText(start, end){
        return this.text.slice(start, end)
    }
    insert(position, text){
        this.__replaceRange(position, position, text)
    }
    isRightToLeft(start, end){

    }
    moveCursorSelection(position, mode){

    }
    paste(){
        if(this.readOnly) return
        if(typeof navigator === 'undefined' || !navigator.clipboard || !navigator.clipboard.readText) return

        let previousText = this.text
        navigator.clipboard.readText().then((text) => {
            this.__insertAtSelection(text)
            if(this.text !== previousText) this.textEdited()
        }).catch(()=>{})
    }
    positionAt(x, y, position){

    }
    positionToRectangle(pos){

    }
    redo(){
        if(this.readOnly || !this.__impl) return
        if(typeof document === 'undefined' || typeof document.execCommand !== 'function') return

        if(document.activeElement !== this.__impl) this.__impl.focus()
        let changed = false
        try {
            changed = document.execCommand('redo')
        } catch (e) {}
        if(!changed) this.canRedo = false
    }
    remove(start, end){
        this.__replaceRange(start, end, '')
    }
    select(start, end){
        let length = this.text.length
        start = Math.max(0, Math.min(Math.floor(Number(start)) || 0, length))
        end = Math.max(start, Math.min(Math.floor(Number(end)) || 0, length))

        if(this.__impl){
            this.__programmaticSelect = true
            try {
                this.__impl.setSelectionRange(start, end)
            } catch (e) {
            }
            this.__programmaticSelect = false
        }

        this.__syncingSelection = true
        try {
            this.selectionStart = start
            this.selectionEnd = end
            this.selectedText = this.text.substring(start, end)
            this.cursorPosition = end
        } finally {
            this.__syncingSelection = false
        }
    }
    selectAll(){
        if(this.__impl && typeof this.__impl.select === 'function'){
            this.__programmaticSelect = true
            try {
                this.__impl.select()
            } catch (e) {}
            this.__programmaticSelect = false
            this.__syncSelectionFromDOM()
            if(this.selectionStart !== 0 || this.selectionEnd !== this.text.length){
                this.select(0, this.text.length)
            }
            return
        }
        this.select(0, this.text.length)
    }
    selectWord(){

    }
    undo(){
        if(this.readOnly || !this.__impl) return
        if(typeof document === 'undefined' || typeof document.execCommand !== 'function') return

        if(document.activeElement !== this.__impl) this.__impl.focus()
        let changed = false
        try {
            changed = document.execCommand('undo')
        } catch (e) {}
        if(!changed) this.canUndo = false
    }

    __destroy(){
        if(this.__onSelectionChange && typeof document !== 'undefined'){
            document.removeEventListener('selectionchange', this.__onSelectionChange)
        }
        JQApplication.MouseController.remove(this)
        super.__destroy()
    }
}



module.exports = TextInput
