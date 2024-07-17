const Item = require('./Item')
const Property = require('../QtQml/Property')
const String = require('../QtQml/String')
const Color = require('../QtQml/Color')
const Real = require('../QtQml/Real')
const Font = require('../QtQml/Font')
const Signal = require('../QtQml/Signal')

class Text extends Item {
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
    static WrapAtWordBoundaryOrAnywhere = 4

    static AutoText = 0
    static PlainText = 1
    static StyledText = 2
    static RichText = 3
    static MarkdownText = 4

    static ElideNone = 0
    static ElideLeft = 1
    static ElideMiddle = 2
    static ElideRight = 3

    static html = []

    static meta = Object.assign({}, Item.meta, {
        text: { type: String, value: '', signalName: 'textChanged' },
        color: { type: Color, value: 'black', signalName: 'colorChanged' },
        font: { type: Font, signalName: 'fontChanged' },
        contentWidth: { type: Real, value: 0, signalName: 'contentWidthChanged' },
        contentHeight: { type: Real, value: 0, signalName: 'contentHeightChanged' },
        horizontalAlignment: { type: Property, value: Text.AlignLeft, signalName: 'horizontalAlignmentChanged' },
        verticalAlignment: { type: Property, value: Text.AlignTop, signalName: 'verticalAlignmentChanged' },
        wrapMode: { type: Property, value: Text.NoWrap, signalName: 'wrapModeChanged' },
        textFormat: { type: Property, value: Text.AutoText, signalName: 'textFormatChanged' },
        elide: { type: Property, value: Text.ElideNone, signalName: 'elideChanged' },
        lineHeight: { type: Real, value: 0, signalName: 'lineHeightChanged' },

        textChanged: {type:Signal, slotName:'onTextChanged', args:[]},
        colorChanged: {type:Signal, slotName:'onColorChanged', args:[]},
        fontChanged: {type:Signal, slotName:'onFontChanged', args:[]},
        contentWidthChanged: {type:Signal, slotName:'onContentWidthChanged', args:[]},
        contentHeightChanged: {type:Signal, slotName:'onContentHeightChanged', args:[]},
        horizontalAlignmentChanged: {type:Signal, slotName:'onHorizontalAlignmentChanged', args:[]},
        verticalAlignmentChanged: {type:Signal, slotName:'onVerticalAlignmentChanged', args:[]},
        wrapModeChanged: {type:Signal, slotName:'onWrapModeChanged', args:[]},
        textFormatChanged: {type:Signal, slotName:'onTextFormatChanged', args:[]},
        elideChanged: {type:Signal, slotName:'onElideChanged', args:[]},
        lineHeightChanged: {type:Signal, slotName:'onLineHeightChanged', args:[]},

        lineLaidOut: {type:Signal, slotName:'onLineLaidOut', args:['line']},
        linkActivated: {type:Signal, slotName:'onLinkActivated', args:['link']},
        LinkHovered: {type:Signal, slotName:'onLinkHovered', args:['link']},
    })


    static create(parent, ...args){
        let proxy = super.create(parent, ...args)
        let self = proxy.__self 

        self.__setDOMStyle({
            textAlign: 'inherit',
            whiteSpace: 'pre',
            lineHeight: 'normal',
            fontSize: '14px',
            fontFamily: 'Segoe UI',
        })

        return proxy
    }

    __updateGeometry(){
        this.width = this.__DOM.scrollWidth
        this.height = this.__DOM.scrollHeight
    }

    onTextChanged(){
        this.__DOM.innerHTML = this.text

        this.__updateGeometry()
    }

    onColorChanged(){
        this.__setDOMStyle({
            color: this.color
        })
    }

    onFontChanged(){
        this.__setDOMStyle({
            fontWeight: this.font.bold == true ? 'bold' : 'normal',
            fontSize: this.font.pixelSize+'px',
            fontFamily: `'${this.font.family}'`,
            textDecoration: this.font.underline == true ? 'underline' : 'unset',
        })

        this.__updateGeometry()
    }
}

module.exports = Text