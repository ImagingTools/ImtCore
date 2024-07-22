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

        accepted: {type:Signal, slotName:'onAccepted', args:[]},
        editingFinished: {type:Signal, slotName:'onEditingFinished', args:[]},
        textEdited: {type:Signal, slotName:'onTextEdited', args:[]},
    })

    static create(parent, ...args){
        let proxy = super.create(parent, ...args)
        proxy.__DOM.classList.add('TextInput')
        proxy.__DOM.setAttribute('contenteditable', true)

        proxy.__DOM.onmousemove = (e)=>{
            let selection = document.getSelection()

            if(selection.rangeCount){
                let range = selection.getRangeAt(0)

                proxy.selectionStart = range.startOffset
                proxy.selectionEnd = range.endOffset
            } else {
                proxy.selectionStart = 0
                proxy.selectionEnd = 0
            }
            
        }
        proxy.__DOM.onkeydown = (e)=>{
            let selection = document.getSelection()

            if(selection.rangeCount){
                let range = selection.getRangeAt(0)

                proxy.selectionStart = range.startOffset
                proxy.selectionEnd = range.endOffset
            } else {
                proxy.selectionStart = 0
                proxy.selectionEnd = 0
            }

            if(e.keyCode === QtEnums.Key_C && e.ctrlKey){
                e.preventDefault()
                proxy.copy()
            } else if(e.keyCode === QtEnums.Key_V && e.ctrlKey){
                e.preventDefault()
                proxy.paste()
            } else if(e.keyCode === QtEnums.Key_X && e.ctrlKey){
                e.preventDefault()
                proxy.cut()
            } else if(e.code === QtEnums.Key_Enter){
                e.preventDefault()
                proxy.accepted()
            }
        }
        proxy.__DOM.oninput = (e)=>{
            let selection = document.getSelection()
            selection.removeAllRanges()

            let buff = proxy.text.split('') 
            switch(e.inputType){
                case 'insertText': {
                    buff.splice(proxy.selectionStart, proxy.selectionEnd-proxy.selectionStart, e.data)
                    proxy.text = buff.join('')
                    proxy.select(proxy.selectionEnd+1, proxy.selectionEnd+1)
                    break
                }
                case 'insertFromPaste': {
                    proxy.paste()
                    break
                }
                case 'deleteByCut': {
                    proxy.cut()
                    break
                }
                case 'deleteContentBackward': {
                    let data = []
                    if(proxy.selectionStart === proxy.selectionEnd){
                        data = buff.splice(proxy.selectionStart-1, proxy.selectionEnd-(proxy.selectionStart-1))
                    } else {
                        data = buff.splice(proxy.selectionStart, proxy.selectionEnd-proxy.selectionStart)
                    }
                    proxy.text = buff.join('')
                    proxy.select(proxy.selectionEnd-data.length, proxy.selectionEnd-data.length)
                    break
                }
                case 'deleteContentForward': {
                    let data = []
                    if(proxy.selectionStart === proxy.selectionEnd){
                        data = buff.splice(proxy.selectionStart, proxy.selectionEnd+1-proxy.selectionStart)
                    } else {
                        data = buff.splice(proxy.selectionStart, proxy.selectionEnd-proxy.selectionStart)
                    }
                    proxy.text = buff.join('')
                    proxy.select(proxy.selectionEnd, proxy.selectionEnd)
                    break
                }
            }

        }
        return proxy
    }

    onEchoModeChanged(){
        if(this.echoMode === TextInput.Password){
            this.__DOM.innerText = this.text.replaceAll(/./g, '*')
        } else {
            this.__DOM.innerText = this.text
        }
    }

    onTextChanged(){
        if(this.echoMode === TextInput.Password){
            this.__DOM.innerText = this.text.replaceAll(/./g, '*')
        } else {
            this.__DOM.innerText = this.text
        }
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
        if(this.__DOM.childNodes.length){
            range.setStart(this.__DOM.childNodes[0], start)
            range.setEnd(this.__DOM.childNodes[0], end)
        } else {
            range.setStart(this.__DOM, 0)
            range.setEnd(this.__DOM, 0)
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
}

module.exports = TextInput