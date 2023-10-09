import {Item} from './Item'
import {Font} from '../utils/Font'
import {Signal} from '../utils/Signal'


export class Text extends Item {
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

    $contentWidthAuto = true
	$contentHeightAuto = true

    $widthAuto = true
	$heightAuto = true

    constructor(args) {
        super(args)

        this.$cP('text', '', this.$textChanged)
        this.$cP('color', 'black', this.$colorChanged)
        this.$cP('contentHeight', 0, this.$contentHeightChanged)
        this.$cP('contentWidth', 0, this.$contentWidthChanged)
        this.$cP('horizontalAlignment', Text.AlignLeft, this.$horizontalAlignmentChanged)
        this.$cP('verticalAlignment', Text.AlignTop, this.$verticalAlignmentChanged)
        this.$cP('wrapMode', Text.NoWrap, this.$wrapModeChanged)
        this.$cP('textFormat', Text.AutoText, this.$textFormatChanged)
        this.$cP('elide', Text.ElideNone, this.$elideChanged)
        this.$cPC('font', Font(), this.$fontChanged)

        this.$cS('linkActivated')
    
        this.$updateTimer = null
    }

    $domCreate(){
        super.$domCreate()
        // this.dom.style.position = "relative";
        // this.dom.style.display = "flex"
        // this.dom.style.width = `0px`
        // this.dom.style.height = `0px`
        // this.dom.style.pointerEvents = 'none'
        // this.dom.style.touchAction = 'none'
        this.impl = document.createElement("span")
        this.dom.appendChild(this.impl);
        
        this.dom.style.display = "flex"
        this.impl.style.textAlign = "inherit"
        this.impl.style.whiteSpace = "pre"
        this.impl.style.lineHeight = "normal";
        
        // this.impl.style.position = "absolute";
        // this.impl.style.width = "fit-content";
        this.impl.innerHTML = ``

        // this.$updateGeometry()
        //this.$observer = setInterval(this.$updateContentWH.bind(this), 100)
    }
    $show(){
        this.dom.style.display = 'flex'
    }
    $elideChanged(){
        this.$preChanged()
        if(this.elide === Text.ElideRight){
            this.dom.style.textOverflow = 'ellipsis' 
            this.dom.style.overflow = 'auto' 
        } else {
            this.dom.style.overflow = 'unset' 
            this.dom.style.textOverflow = 'unset' 
        }
        this.$postChanged()
        
        // this.$updateGeometry()
    }
    $textFormatChanged(){
        if(this.textFormat === Text.AutoText){
            let regexp = /<[^<>]+>/g
            if(regexp.test(this.text)){
                this.$isHTML = true
            } else {
                this.$isHTML = false
            }
        } else if(this.textFormat === Text.PlainText){
            this.$isHTML = false
        } else {
            this.$isHTML = true
        }

        this.$preChanged()
        this.impl.innerHTML = `${this.text}`
        this.$postChanged()
    }
    $textChanged(){
        if(this.textFormat === Text.AutoText){
            let regexp = /<[^<>]+>/g
            if(regexp.test(this.text) || this.text.indexOf('\n')){
                this.$isHTML = true
            } else {
                this.$isHTML = false
            }
        } else if(this.textFormat === Text.PlainText){
            this.$isHTML = false
        } else {
            this.$isHTML = true
        }

        this.$preChanged()
        this.impl.innerHTML = `${this.text}`
        this.$postChanged()
        // this.$updateGeometry()
        // this.$calcWH()
    }
    $visibleChanged(){
        super.$visibleChanged()
        // this.$updateGeometry()
    }
    $updateGeometry(){
	
    }
    // $calcWH(){
    //     // let tempText = document.createElement("span")
    //     // let tempDom = document.createElement("div")
    //     // tempText.innerHTML = this.$p.text.val

    //     // tempText.style.fontFamily = this.impl.style.fontFamily
    //     // tempText.style.fontSize = this.impl.style.fontSize 
    //     // tempText.style.fontStyle = this.impl.style.fontStyle
    //     // tempText.style.fontWeight = this.impl.style.fontWeight
    //     // tempText.style.textDecoration = this.impl.style.textDecoration
    //     // tempText.style.whiteSpace = this.impl.style.whiteSpace
    //     // tempText.style.wordBreak = this.impl.style.wordBreak
    //     if(!this.$contentWAuto){
    //         this.impl.style.width = this.dom.style.width
    //         // tempDom.style.maxWidth = this.dom.style.width
    //     }
    //     if(!this.$contentHAuto){
    //         this.impl.style.height = this.dom.style.height
    //         // tempDom.style.maxHeight = this.dom.style.height
    //     }

    //     // document.body.appendChild(tempDom);
    //     // tempDom.appendChild(tempText);

    //     let rect = this.impl.getBoundingClientRect()
    //     this.contentWidth = rect.width
    //     this.contentHeight = rect.height
    //     // tempDom.remove()

    //     // this.$updateGeometry()
    // }
    $colorChanged(){
        let rgba = Qt.$colorToRGBA(this.color)
        this.impl.style.color = `rgba(${rgba.r},${rgba.g},${rgba.b},${this.color === 'transparent' ? 0 : rgba.a * this.opacity})`
    }
    $opacityChanged(){
        let rgba = Qt.$colorToRGBA(this.color)
        this.dom.style.opacity = 1
        this.impl.style.color = `rgba(${rgba.r},${rgba.g},${rgba.b},${this.color === 'transparent' ? 0 : rgba.a * this.opacity})`
    }
    $contentWidthChanged(){
        // this.impl.style.width = `${this.contentWidth}px`
        // if(this.$contentWAuto || this.$p.width.val < this.$p.contentWidth.val){
        //     if(this.$contentWAuto){
        //         if(this.$p.width.val !== this.$p.contentWidth.val){
        //             this.width = this.$p.contentWidth.val
        //             // this.$widthChanged()
        //         }
                
        //         this.$contentWAuto = true
        //     } else {
        //         this.$p.width.val = this.$p.contentWidth.val
        //     }
        //     this.$calcWH()
        //     // this.$p.width.val = this.contentWidth
        //     // this.dom.style.width = `${this.width}px`
        //     // for(let i = 0; i < this.$p.width.depends.length; i++){
        //     //     this.$p.width.depends[i]()
        //     // }
        //     // for(let i = 0; i < this.$p.x.depends.length; i++){
        //     //     this.$p.x.depends[i]()
        //     // }
        // }
    }
    $contentHeightChanged(){
        // this.impl.style.height = `${this.contentHeight}px`
        // if(this.$contentHAuto){
        //     // if(this.$contentHAuto){
        //         if(this.$p.height.val !== this.$p.contentHeight.val){
        //             this.height = this.$p.contentHeight.val
        //             // this.$heightChanged()
        //         }
                
        //         this.$contentHAuto = true
        //         this.$calcWH()
        //     // } else {
        //     //     this.height = this.contentHeight
        //     // }
        //     // this.$p.height.val = this.contentHeight
        //     // this.dom.style.height = `${this.height}px`
        //     // for(let i = 0; i < this.$p.height.depends.length; i++){
        //     //     this.$p.height.depends[i]()
        //     // }
        //     // for(let i = 0; i < this.$p.y.depends.length; i++){
        //     //     this.$p.y.depends[i]()
        //     // }
        // }
    }
    $widthChanged(){
        super.$widthChanged()
        this.$widthAuto = false
        this.$preChanged()
        this.$postChanged()
        // this.$contentWAuto = false
        // this.$calcWH()
        // this.$updateGeometry()
        // this.$horizontalAlignmentChanged()
    }
    $heightChanged(){
        super.$heightChanged()
        this.$heightAuto = false
        // this.$contentHAuto = false
        // this.$calcWH()
        // this.$updateGeometry()
        this.$preChanged()
        this.$postChanged()
        // this.$verticalAlignmentChanged()
    }
    $preChanged(){
        if(this.$isHTML){
            if(this.$widthAuto) this.impl.style.width = '0px'
            if(this.$heightAuto) this.impl.style.height = '0px'
        }
        
    }
    $postChanged(){
        if(this.$isHTML){
            if(this.$widthAuto){
                this.width = this.impl.scrollWidth
                this.impl.style.width = `${this.impl.scrollWidth}px`
                this.$widthAuto = true
            }
            if(this.$heightAuto){
                this.height = this.impl.scrollHeight
                this.impl.style.height = `${this.impl.scrollHeight}px`
                this.$heightAuto = true
            }

            this.contentWidth = this.dom.scrollWidth
            this.contentHeight = this.dom.scrollHeight
        } else {
            this.$applyMetrics()
        }
    }
    $applyMetrics(){
        let textMetrics = Core.measureText(this.text ? this.text.toString() : '', this.font.pixelSize, this.font.family, this.$widthAuto ? 0 : this.width, this.wrapMode)
        this.impl.innerHTML = textMetrics.text

        if(this.$widthAuto){
            this.width = textMetrics.width
            this.impl.style.width = `${textMetrics.width}px`
            this.$widthAuto = true
        }
        if(this.$heightAuto){
            this.height = textMetrics.height
            this.impl.style.height = `${textMetrics.height}px`
            this.$heightAuto = true
        }

        this.contentWidth = textMetrics.width
        this.contentHeight = textMetrics.height
    }
    $horizontalAlignmentChanged(){
        // switch(this.$p.horizontalAlignment.val){
        //     case Text.AlignLeft: this.dom.style.textAlign = 'left'; this.impl.style.margin = "0 auto 0 0"; break;
        //     case Text.AlignRight: this.dom.style.textAlign = 'right'; this.impl.style.margin = "0 0 0 auto"; break;
        //     case Text.AlignHCenter: this.dom.style.textAlign = 'center'; this.impl.style.margin = "0 auto 0 auto"; break;
        //     case Text.AlignJustify: this.dom.style.textAlign = 'justify'; this.impl.style.margin = "0 0 0 0"; break;
        // }
        // let realWidth = this.width + this.anchors.leftMargin + this.anchors.rightMargin
        switch(this.$p.horizontalAlignment.val){
            case Text.AlignLeft: this.dom.style.justifyContent = 'flex-start'; break;
            case Text.AlignRight: this.dom.style.justifyContent = 'dlex-end'; break;
            case Text.AlignHCenter: this.dom.style.justifyContent = 'center'; break;
            case Text.AlignJustify: this.dom.style.justifyContent = 'normal'; break;
        }
    }
    $verticalAlignmentChanged(){
        // switch(this.$p.verticalAlignment.val){
        //     case Text.AlignTop: this.dom.style.alignItems = 'flex-start'; break;
        //     case Text.AlignBottom: this.dom.style.alignItems = 'flex-end'; break;
        //     case Text.AlignVCenter: this.dom.style.alignItems = 'center'; break;
        // }
        // let realHeight = this.height + this.anchors.topMargin + this.anchors.bottomMargin
        switch(this.$p.verticalAlignment.val){
            case Text.AlignTop: this.dom.style.alignItems = 'flex-start'; break;
            case Text.AlignBottom: this.dom.style.alignItems = 'flex-end'; break;
            case Text.AlignVCenter: this.dom.style.alignItems = 'center'; break;
        }
    }
    $wrapModeChanged(){
        this.$preChanged()
        switch(this.$p.wrapMode.val){
            case Text.NoWrap: this.impl.style.whiteSpace = "pre"; break;
            case Text.WordWrap: this.impl.style.whiteSpace = "pre-wrap"; this.impl.style.wordBreak = "break-word"; break;
            case Text.WrapAnywhere: this.impl.style.whiteSpace = "pre-wrap"; this.impl.style.wordBreak = "break-all"; break;
            case Text.Wrap: this.impl.style.whiteSpace = "pre-wrap"; this.impl.style.wordBreak = "break-word"; break;
            case Text.WrapAtWordBoundaryOrAnywhere: this.impl.style.whiteSpace = "pre-wrap"; this.impl.style.wordBreak = "break-all"; break;
        }
        // this.$calcWH()
        this.$postChanged()
    }

    $fontChanged(){
        // if(this.$pixelSize !== this.font.pixelSize) this.$p.font.$pointSize = this.font.pixelSize / 0.75
        // if(this.$pointSize !== this.font.pointSize) this.$p.font.$pixelSize = this.font.pointSize * 0.75
        // this.$pixelSize = this.font.pixelSize
        // this.$pointSize = this.font.pointSize
        this.$preChanged()

        this.impl.style.fontFamily = this.$p['font.family'].val ? this.$p['font.family'].val : 'unset';
        this.impl.style.fontSize = `${this.$p['font.pixelSize'].val}px`
        this.impl.style.fontStyle = this.$p['font.italic'].val ? 'italic' : 'normal';
        this.impl.style.fontWeight = this.$p['font.bold'].val ? 'bold' : 'normal';
        this.impl.style.textDecoration = this.$p['font.underline'].val ? 'underline' : 'unset';
        // this.$calcWH()
        this.$postChanged()

        if(this.$p['font.family'].val && this.$p['font.pixelSize'].val){
            document.fonts.load(`${this.$p['font.pixelSize'].val}px ${this.$p['font.family'].val}`).then((fonts)=>{
                if(fonts.length && this.$p) {
                    this.$preChanged()
                    this.$postChanged()
                }
            })
        }
    }
    $destroy(){
        //clearInterval(this.$observer)
        // clearTimeout(this.$updateTimer)
        this.impl.remove()
        super.$destroy()
    }
}

QML.Text = Text