const Item = require('./Item')
const Bool = require('../QtQml/Bool')
const Int = require('../QtQml/Int')
const Var = require('../QtQml/Var')
const String = require('../QtQml/String')
const Color = require('../QtQml/Color')
const Real = require('../QtQml/Real')
const Geometry = require('../QtQml/Geometry')
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
        selectByMouse: { type: Bool, value: true, signalName: 'selectByMouseChanged' },

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
        selectByMouseChanged: {type:Signal, slotName:'onSelectByMouseChanged', args:[]},

        editingFinished: {type:Signal, slotName:'onEditingFinished', args:[]},
    })

    static create(parent, ...args){
        let obj = super.create(parent, ...args)
        obj.__DOM.classList.add('TextEdit')
        obj.__createImpl()
        JQApplication.MouseController.add(obj)
        return obj
    }

    __createImpl(){
        let dom = this.__getDOM()
        let impl = document.createElement('textarea')
        impl.classList.add('impl')
        this.__impl = impl

        this.__setImplStyle({
            outline: 'none',
            border: 'none',
            background: 'none',
            width: '100%',
            maxHeight: '100%',
            height: '100%',
            font: 'inherit',
            color: 'inherit',
            zIndex: 'inherit',
            resize: 'none',
            cursor: 'unset',
        })
        
        impl.setAttribute('spellcheck', "false")
        impl.setAttribute('rows', "1")
        dom.appendChild(impl)

        impl.onfocus = ()=>{
            this.forceActiveFocus()
        }

        impl.onblur = ()=>{
            this.activeFocus = false
        }

        impl.onkeydown = (e)=>{
            if(e.code === QtEnums.Key_C && e.ctrlKey){
                e.stopPropagation()
            } else if(e.code === QtEnums.Key_V && e.ctrlKey){
                e.stopPropagation()
            } else if(e.code === QtEnums.Key_X && e.ctrlKey){
                e.stopPropagation()
            } else if(e.code === QtEnums.Key_A && e.ctrlKey){
                e.stopPropagation()
                this.selectAll()
            } else if(e.key === QtEnums.Key_Enter){
                e.stopPropagation()
            } else if(e.key === QtEnums.Key_Tab){
                e.preventDefault()
                e.stopPropagation()
            }
        }

        impl.oninput = (e)=>{
            this.text = impl.value
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

    __complete(){
        super.__complete()
        this.__updateGeometry()
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

    SLOT_verticalAlignmentChanged(oldValue, newValue){
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

    SLOT_activeFocusChanged(oldValue, newValue){
        if(!this.activeFocus){
            this.__impl.blur()
        }
    }

    __updateGeometry(){
        let text = this.text
        if(text){
            if(text[text.length-1] === '\n') text += '.'
        } else {
            text = '.'
        }

        let textMetrics = JQApplication.TextController.measureText(text, this.font, !this.width__prevent ? 0 : this.width, this.wrapMode, false, false)

        this.contentWidth = textMetrics.width
        this.contentHeight = textMetrics.height

        // Geometry.setAuto(this.__self, 'height', textMetrics.height, this.__self.constructor.meta.height)
        this.height = textMetrics.height
        Geometry.setAuto(this.__self, 'width', textMetrics.width, this.__self.constructor.meta.width)

        
        // this.width = textMetrics.width
        

        // let textMetrics = JQApplication.TextController.measureText(text, this.font, this.__getDataQml('width').__auto ? 0 : this.width, this.wrapMode, 0)

        // this.__getDataQml('width').__setAuto(this.__impl.scrollWidth)
        // this.__getDataQml('height').__setAuto(this.__impl.scrollHeight)

        // this.contentWidth = textMetrics.width
        // this.contentHeight = textMetrics.height
        // this.paintedWidth = textMetrics.width
        // this.paintedHeight = textMetrics.height
        // this.__setDOMStyle({
        //     minHeight: textMetrics.height+'px'
        // })
    }

    SLOT_visibleChanged(oldValue, newValue){
        super.SLOT_visibleChanged()
        this.__updateGeometry()
    }

    SLOT_textChanged(oldValue, newValue){
        this.__impl.value = this.text

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



module.exports = TextEdit