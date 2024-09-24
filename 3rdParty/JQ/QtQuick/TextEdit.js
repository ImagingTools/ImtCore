const Item = require('./Item')
const Bool = require('../QtQml/Bool')
const Int = require('../QtQml/Int')
const Var = require('../QtQml/Var')
const String = require('../QtQml/String')
const Color = require('../QtQml/Color')
const Real = require('../QtQml/Real')
const Font = require('../QtQml/Font')
const Signal = require('../QtQml/Signal')
const QtEnums = require('../Qt/enums')
const JQApplication = require("../core/JQApplication")

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

    static meta = Object.assign({}, Item.meta, {
        text: { type: String, value: '', signalName: 'textChanged' },
        color: { type: Color, value: 'black', signalName: 'colorChanged' },
        selectionColor: { type: Color, value: '#000080', signalName: 'selectionColorChanged' },
        selectedTextColor: { type: Color, value: '#ffffff', signalName: 'selectedTextColorChanged' },
        horizontalAlignment: { type: Real, value: TextEdit.AlignLeft, signalName: 'horizontalAlignmentChanged' },
        verticalAlignment: { type: Real, value: TextEdit.AlignTop, signalName: 'verticalAlignmentChanged' },
        wrapMode: { type: Real, value: TextEdit.NoWrap, signalName: 'wrapModeChanged' },
        readOnly: { type: Bool, value: false, signalName: 'readOnlyChanged' },
        font: { type: Font, signalName: 'fontChanged' },
        contentHeight: { type: Real, value: 0, signalName: 'contentHeightChanged' },
        contentWidth: { type: Real, value: 0, signalName: 'contentWidthChanged' },
        paintedHeight: { type: Real, value: 0, signalName: 'paintedHeightChanged' },
        paintedWidth: { type: Real, value: 0, signalName: 'paintedWidthChanged' },
        cursorRectangle: { type: Var, value: undefined, signalName: 'cursorRectangleChanged' },
        selectionStart: { type: Int, value: 0, signalName: 'selectionStartChanged' },
        selectionEnd: { type: Int, value: 0, signalName: 'selectionEndChanged' },
        padding: { type: Real, value: 0, signalName: 'paddingChanged' },
        leftPadding: { type: Real, value: 0, signalName: 'leftPaddingChanged' },
        topPadding: { type: Real, value: 0, signalName: 'topPaddingChanged' },
        rightPadding: { type: Real, value: 0, signalName: 'rightPaddingChanged' },
        bottomPadding: { type: Real, value: 0, signalName: 'bottomPaddingChanged' },
        selectedText: { type: String, value: '', signalName: 'selectedTextChanged' },

        textChanged: {type:Signal, slotName:'onTextChanged', args:[]},
        colorChanged: {type:Signal, slotName:'onColorChanged', args:[]},
        selectionColorChanged: {type:Signal, slotName:'onSelectionColorChanged', args:[]},
        selectedTextColorChanged: {type:Signal, slotName:'onSelectedTextColorChanged', args:[]},
        horizontalAlignmentChanged: {type:Signal, slotName:'onHorizontalAlignmentChanged', args:[]},
        verticalAlignmentChanged: {type:Signal, slotName:'onVerticalAlignmentChanged', args:[]},
        wrapModeChanged: {type:Signal, slotName:'onWrapModeChanged', args:[]},
        readOnlyChanged: {type:Signal, slotName:'onReadOnlyChanged', args:[]},
        fontChanged: {type:Signal, slotName:'onFontChanged', args:[]},
        contentHeightChanged: {type:Signal, slotName:'onContentHeightChanged', args:[]},
        contentWidthChanged: {type:Signal, slotName:'onContentWidthChanged', args:[]},
        paintedHeightChanged: {type:Signal, slotName:'onPaintedHeightChanged', args:[]},
        paintedWidthChanged: {type:Signal, slotName:'onPaintedWidthChanged', args:[]},
        cursorRectangleChanged: {type:Signal, slotName:'onCursorRectangleChanged', args:[]},
        selectionStartChanged: {type:Signal, slotName:'onSelectionStartChanged', args:[]},
        selectionEndChanged: {type:Signal, slotName:'onSelectionEndChanged', args:[]},
        paddingChanged: {type:Signal, slotName:'onPaddingChanged', args:[]},
        leftPaddingChanged: {type:Signal, slotName:'onLeftPaddingChanged', args:[]},
        topPaddingChanged: {type:Signal, slotName:'onTopPaddingChanged', args:[]},
        rightPaddingChanged: {type:Signal, slotName:'onRightPaddingChanged', args:[]},
        bottomPaddingChanged: {type:Signal, slotName:'onBottomPaddingChanged', args:[]},
        selectedTextChanged: {type:Signal, slotName:'onSelectedTextChanged', args:[]},

        editingFinished: {type:Signal, slotName:'onEditingFinished', args:[]},
    })

    static create(parent, ...args){
        let proxy = super.create(parent, ...args)
        proxy.__DOM.classList.add('TextEdit')
        proxy.__createImpl()
        JQApplication.MouseController.add(proxy)
        return proxy
    }

    __createImpl(){
        let dom = this.__getDOM()
        let impl = document.createElement('div')
        impl.classList.add('impl')
        dom.appendChild(impl)
        this.__impl = impl

        impl.setAttribute('contenteditable', true)

        impl.onfocus = ()=>{
            this.forceActiveFocus()
        }

        impl.onblur = ()=>{
            this.activeFocus = false
            this.focus = false
        }

        impl.onkeydown = (e)=>{
            let selection = document.getSelection()

            if(selection.rangeCount){
                let range = selection.getRangeAt(0)

                this.selectionStart = range.startOffset
                this.selectionEnd = range.endOffset
            } else {
                this.selectionStart = 0
                this.selectionEnd = 0
            }

            if(e.keyCode === QtEnums.Key_C && e.ctrlKey){
                e.preventDefault()
                this.copy()
            } else if(e.keyCode === QtEnums.Key_V && e.ctrlKey){
                e.preventDefault()
                this.paste()
            } else if(e.keyCode === QtEnums.Key_X && e.ctrlKey){
                e.preventDefault()
                this.cut()
            } else if(e.code === QtEnums.Key_Enter){
                // e.preventDefault()
                // this.text += '\n\r'
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
                    this.select(this.selectionEnd+1, this.selectionEnd+1)
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

    onHorizontalAlignmentChanged(){
        switch(this.horizontalAlignment){
            case TextEdit.AlignLeft: {
                this.__setImplStyle({
                    alignItems: 'flex-start'
                })
                break
            }
            case TextEdit.AlignRight: {
                this.__setImplStyle({
                    alignItems: 'flex-end'
                })
                break
            }
            case TextEdit.AlignHCenter: {
                this.__setImplStyle({
                    alignItems: 'center'
                })
                break
            }
            case TextEdit.AlignJustify: {
                this.__setImplStyle({
                    alignItems: 'normal'
                })
                break
            }
        }
    }

    onVerticalAlignmentChanged(){
        switch(this.verticalAlignment){
            case TextEdit.AlignTop: {
                this.__setImplStyle({
                    justifyContent: 'flex-start'
                })
                break
            }
            case TextEdit.AlignBottom: {
                this.__setImplStyle({
                    justifyContent: 'flex-end'
                })
                break
            }
            case TextEdit.AlignVCenter: {
                this.__setImplStyle({
                    justifyContent: 'center'
                })
                break
            }
            case TextEdit.AlignJustify: {
                this.__setImplStyle({
                    justifyContent: 'normal'
                })
                break
            }
        }
    }

    onFocusChanged(){
        if(this.focus){
            this.__impl.focus()
        }
    }

    onActiveFocusChanged(){
        if(this.activeFocus){
            this.__impl.focus()
            this.focus = true
        }
    }

    onTextChanged(){
        this.__impl.innerHTML = this.text

        let rect = this.__impl.getBoundingClientRect()

        this.__getObject('width').__setAuto(rect.width)
        this.__getObject('height').__setAuto(rect.height)

        this.contentWidth = rect.width
        this.contentHeight = rect.height
        this.paintedWidth = rect.width
        this.paintedHeight = rect.height
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

module.exports = TextEdit