const Item = require('./Item')
const Bool = require('../QtQml/Bool')
const Var = require('../QtQml/Var')
const Int = require('../QtQml/Int')
const String = require('../QtQml/String')
const Color = require('../QtQml/Color')
const Real = require('../QtQml/Real')
const Font = require('../QtQml/Font')
const Signal = require('../QtQml/Signal')

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

        let cacheRange = {
            startOffset: 0,
            endOffset: 0,
        }
        proxy.__DOM.onkeydown = (e)=>{
            let range = document.getSelection().getRangeAt(0)
            cacheRange.startOffset = range.startOffset
            cacheRange.endOffset = range.endOffset
        }
        proxy.__DOM.oninput = (e)=>{
            let selection = document.getSelection()
            selection.removeAllRanges()

            let buff = proxy.text.split('') 
            switch(e.inputType){
                case 'insertText': {
                    buff.splice(cacheRange.startOffset, cacheRange.endOffset-cacheRange.startOffset, e.data)
                    proxy.text = buff.join('')

                    let range = new Range()
                        if(proxy.__DOM.childNodes.length){
                            range.setStart(proxy.__DOM.childNodes[0], cacheRange.endOffset+1)
                            range.setEnd(proxy.__DOM.childNodes[0], cacheRange.endOffset+1)
                        } else {
                            range.setStart(proxy.__DOM, 0)
                            range.setEnd(proxy.__DOM, 0)
                        }
                        selection.addRange(range)

                    break
                }
                case 'insertFromPaste': {
                    navigator.clipboard.readText().then((text) => {
                        let data = buff.splice(cacheRange.startOffset, cacheRange.endOffset-cacheRange.startOffset, text)
                        proxy.text = buff.join('')

                        let range = new Range()
                        if(proxy.__DOM.childNodes.length){
                            range.setStart(proxy.__DOM.childNodes[0], cacheRange.endOffset-data.length+text.length)
                            range.setEnd(proxy.__DOM.childNodes[0], cacheRange.endOffset-data.length+text.length)
                        } else {
                            range.setStart(proxy.__DOM, 0)
                            range.setEnd(proxy.__DOM, 0)
                        }
                        selection.addRange(range)
                    })
                    break
                }
                case 'deleteByCut': {
                    let data = buff.splice(cacheRange.startOffset, cacheRange.endOffset-cacheRange.startOffset)
                    navigator.clipboard.writeText(data.join('')).then(() => {
                        proxy.text = buff.join('')

                        let range = new Range()
                        if(proxy.__DOM.childNodes.length){
                            range.setStart(proxy.__DOM.childNodes[0], cacheRange.endOffset)
                            range.setEnd(proxy.__DOM.childNodes[0], cacheRange.endOffset)
                        } else {
                            range.setStart(proxy.__DOM, 0)
                            range.setEnd(proxy.__DOM, 0)
                        }
                        selection.addRange(range)
                    })
                    break
                }
                case 'deleteContentBackward': {
                    let data = []
                    if(cacheRange.startOffset === cacheRange.endOffset){
                        data = buff.splice(cacheRange.startOffset-1, cacheRange.endOffset-(cacheRange.startOffset-1))
                    } else {
                        data = buff.splice(cacheRange.startOffset, cacheRange.endOffset-cacheRange.startOffset)
                    }
                    proxy.text = buff.join('')

                    let range = new Range()
                    if(proxy.__DOM.childNodes.length){
                        range.setStart(proxy.__DOM.childNodes[0], cacheRange.endOffset-data.length)
                        range.setEnd(proxy.__DOM.childNodes[0], cacheRange.endOffset-data.length)
                    } else {
                        range.setStart(proxy.__DOM, 0)
                        range.setEnd(proxy.__DOM, 0)
                    }
                    selection.addRange(range)

                    break
                }
                case 'deleteContentForward': {
                    let data = []
                    if(cacheRange.startOffset === cacheRange.endOffset){
                        data = buff.splice(cacheRange.startOffset, cacheRange.endOffset+1-cacheRange.startOffset)
                    } else {
                        data = buff.splice(cacheRange.startOffset, cacheRange.endOffset-cacheRange.startOffset)
                    }
                    proxy.text = buff.join('')

                    let range = new Range()
                    if(proxy.__DOM.childNodes.length){
                        range.setStart(proxy.__DOM.childNodes[0], cacheRange.endOffset)
                        range.setEnd(proxy.__DOM.childNodes[0], cacheRange.endOffset)
                    } else {
                        range.setStart(proxy.__DOM, 0)
                        range.setEnd(proxy.__DOM, 0)
                    }
                    selection.addRange(range)

                    break
                }
            }

        }
        return proxy
    }

    onTextChanged(){
        this.__DOM.innerText = this.text.replaceAll(/./g, '*')
    }

    cut(){

    }

    copy(){
        
    }

    paste(){
        
    }

    clear(){
        
    }

    redo(){
        
    }

    undo(){
        
    }

    remove(start, end){

    }

    select(start, end){

    }

    selectAll(){

    }

    ensureVisible(position){

    }
}

module.exports = TextInput