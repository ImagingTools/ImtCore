const { Item } = require('./Item')
const { QString, QColor, QReal, QFont, QProperty } = require('../utils/properties')

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
    
    static defaultProperties = {
        text: { type: QString, value: '', changed: '$textChanged' },
        color: { type: QColor, value: 'black', changed: '$colorChanged' },
        font: { type: QFont, changed: '$fontChanged' },
        contentWidth: { type: QReal, value: 0 },
        contentHeight: { type: QReal, value: 0 },
        horizontalAlignment: { type: QProperty, value: Text.AlignLeft, changed: '$horizontalAlignmentChanged' },
        verticalAlignment: { type: QProperty, value: Text.AlignTop, changed: '$verticalAlignmentChanged' },
        wrapMode: { type: QProperty, value: Text.NoWrap, changed: '$wrapModeChanged' },
        textFormat: { type: QProperty, value: Text.AutoText, changed: '$textFormatChanged' },
        elide: { type: QProperty, value: Text.ElideNone, changed: '$elideChanged' },
    }

    static defaultSignals = {
        linkActivated: { params: [] },
    }

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        
        this.setStyle({
            display: 'flex',
            textAlign: 'inherit',
            whiteSpace: 'pre',
            lineHeight: 'normal',
            fontSize: '14px',
        })

        this.impl = document.createElement('span')
        this.impl.style.textAlign = 'inherit'
        this.getDom().appendChild(this.impl)

        this.getProperty('visible').getNotify().connect(()=>{
            this.updateGeometry()
        })
    }


    updateGeometry(){
        this.setStyle({
            minHeight: 0,
            height: 0,
        })
        this.getProperty('width').setAuto(this.impl.scrollWidth)
        this.getProperty('height').setAuto(this.impl.scrollHeight)

        this.getProperty('contentWidth').reset(this.impl.scrollWidth)
        this.getProperty('contentHeight').reset(this.impl.scrollHeight)

        this.setStyle({
            minHeight: `${this.getPropertyValue('height')}px`,
            height: `${this.getPropertyValue('height')}px`,
        })
    }

    applyMetrics(){
        let isHTML = false
        if(this.getPropertyValue('textFormat') === Text.AutoText){
            let regexp = /<[^<>]+>/g
            if(this.getPropertyValue('text') && (regexp.test(this.getPropertyValue('text')) || this.getPropertyValue('text').indexOf('\n') >= 0)){
                isHTML = true
            } else {
                isHTML = false
            }
        } else if(this.getPropertyValue('textFormat') === Text.PlainText){
            isHTML = false
        } else {
            isHTML = true
        }

        if(isHTML){
            this.impl.innerHTML = this.getPropertyValue('text').replaceAll('<br>', '\r')
            this.updateGeometry()

        } else {
            let textMetrics = TextFontController.measureText(this.getPropertyValue('text'), this.getProperty('font').getPropertyValue('pixelSize'), this.getProperty('font').getPropertyValue('family'), this.getProperty('width').auto ? 0 : this.getProperty('width').get(), this.getPropertyValue('wrapMode'))
            
            this.impl.innerHTML = this.getPropertyValue('text')
            this.getProperty('width').setAuto(textMetrics.width)
            this.getProperty('height').setAuto(textMetrics.height)

            this.getProperty('contentWidth').reset(textMetrics.width)
            this.getProperty('contentHeight').reset(textMetrics.height)
        }
        

        
    }

    $widthChanged(){
        super.$widthChanged()
        if(!this.getProperty('width').auto) this.applyMetrics()
    }

    $heightChanged(){
        super.$heightChanged()
        if(!this.getProperty('height').auto) this.applyMetrics()
    }

    $horizontalAlignmentChanged(){
        switch(this.getPropertyValue('horizontalAlignment')){
            case Text.AlignLeft: this.setStyle({ justifyContent: 'flex-start', textAlign: 'start' }); break;
            case Text.AlignRight: this.setStyle({ justifyContent: 'flex-end', textAlign: 'end' }); break;
            case Text.AlignHCenter: this.setStyle({ justifyContent: 'center', textAlign: 'center' }); break;
            case Text.AlignJustify: this.setStyle({ justifyContent: 'normal', textAlign: 'justify' }); break;
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
        switch(this.getPropertyValue('wrapMode')){
            case Text.NoWrap: this.setStyle({ whiteSpace: 'pre' }); break;
            case Text.WordWrap: this.setStyle({ whiteSpace: 'pre-wrap', wordBreak: 'break-word' }); break;
            case Text.WrapAnywhere: this.setStyle({ whiteSpace: 'pre-wrap', wordBreak: 'break-all' }); break;
            case Text.Wrap: this.setStyle({ whiteSpace: 'pre-wrap', wordBreak: 'break-word' }); break;
            case Text.WrapAtWordBoundaryOrAnywhere: this.setStyle({ whiteSpace: 'pre-wrap', wordBreak: 'break-all' }); break;
        }
        this.applyMetrics()
    }

    $textFormatChanged(){
        this.applyMetrics()
    }

    $elideChanged(){
        if(this.getPropertyValue('elide') === Text.ElideRight){
            this.setStyle({
                textOverflow: 'ellipsis',
                overflow: 'auto',
            })
        } else {
            this.setStyle({
                textOverflow: 'unset',
                overflow: 'unset',
            })
        }
    }

    $textChanged(){
        // this.$dom.innerHTML = this.getPropertyValue('text')
        this.applyMetrics()
    }

    $colorChanged(){
        let rgba = this.getProperty('color').toRGBA()
        this.setStyle({
            opacity: 1,
            color: `rgba(${rgba.r},${rgba.g},${rgba.b},${this.getProperty('color').get() === 'transparent' ? 0 : rgba.a * this.getPropertyValue('opacity')})`
        })
    }

    $fontChanged(){
        this.setStyle({
            fontWeight: this.getProperty('font').getPropertyValue('bold') ? 'bold' : 'normal',
            fontSize: this.getProperty('font').getPropertyValue('pixelSize')+'px',
            fontFamily: `'${this.getProperty('font').getPropertyValue('family')}'`,
            textDecoration: this.getProperty('font').getPropertyValue('underline') ? 'underline' : 'unset',
            
        })
        // for Mozilla Firefox
        // this.impl.style.borderBottom = this.getProperty('font').getPropertyValue('underline') ? `1px solid ${this.getPropertyValue('color')}` : 'unset'
        if(this.getPropertyValue('text')) this.applyMetrics()
    }

    $opacityChanged(){
        let rgba = this.getProperty('color').toRGBA()
        this.setStyle({
            opacity: 1,
            color: `rgba(${rgba.r},${rgba.g},${rgba.b},${this.getProperty('color').get() === 'transparent' ? 0 : rgba.a * this.getPropertyValue('opacity')})`
        })
    }

    $destroy(){
        super.$destroy()
    }
}

module.exports.Text = Text