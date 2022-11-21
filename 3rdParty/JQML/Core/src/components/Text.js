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

    $contentWAuto = true
    $contentHAuto = true

    constructor(parent) {
        super(parent)

        this.$cP('text', '').connect(this.$textChanged.bind(this))
        this.$cP('color', "'black'").connect(this.$colorChanged.bind(this))
        this.$cP('contentHeight', 0).connect(this.$contentHeightChanged.bind(this))
        this.$cP('contentWidth', 0).connect(this.$contentWidthChanged.bind(this))
        this.$cP('horizontalAlignment', Text.AlignLeft).connect(this.$horizontalAlignmentChanged.bind(this))
        this.$cP('verticalAlignment', Text.AlignTop).connect(this.$verticalAlignmentChanged.bind(this))
        this.$cP('wrapMode', Text.NoWrap).connect(this.$wrapModeChanged.bind(this))
        this.$cP('textFormat', Text.AutoText)
        this.$cPC('font', Font()).connect(this.$fontChanged.bind(this))

        this.$s.linkActivated = Signal()
    }

    $domCreate(){
        super.$domCreate()
        this.dom.style.display = "flex"
        this.dom.style.width = `0px`
        this.dom.style.height = `0px`
        this.impl = document.createElement("div")
        this.dom.appendChild(this.impl);
        
        this.impl.style.textAlign = "inherit"
        this.impl.style.whiteSpace = "pre";
        this.impl.style.width = "fit-content";
        this.impl.innerHTML = ``

        //this.$observer = setInterval(this.$updateContentWH.bind(this), 100)
    }
    $show(){
        this.dom.style.display = 'flex'
    }
    $textChanged(){
        this.impl.innerHTML = `${this.$p.text.val}`

        this.$calcWH()
    }
    $calcWH(){
        let tempText = document.createElement("span")
        let tempDom = document.createElement("div")
        tempText.innerHTML = this.$p.text.val

        tempText.style.fontFamily = this.impl.style.fontFamily
        tempText.style.fontSize = this.impl.style.fontSize 
        tempText.style.fontStyle = this.impl.style.fontStyle
        tempText.style.fontWeight = this.impl.style.fontWeight
        tempText.style.textDecoration = this.impl.style.textDecoration
        tempText.style.whiteSpace = this.impl.style.whiteSpace
        tempText.style.wordBreak = this.impl.style.wordBreak
        if(!this.$contentWAuto){
            tempDom.style.width = this.dom.style.width
            // tempDom.style.maxWidth = this.dom.style.width
        }
        if(!this.$contentHAuto){
            tempDom.style.height = this.dom.style.height
            // tempDom.style.maxHeight = this.dom.style.height
        }

        document.body.appendChild(tempDom);
        tempDom.appendChild(tempText);

        let rect = tempText.getBoundingClientRect()
        this.contentWidth = rect.width
        this.contentHeight = rect.height
        tempDom.remove()

        // this.$updateGeometry()
    }
    $colorChanged(){
        this.impl.style.color = `${this.$p.color.val}`
    }
    $contentWidthChanged(){
        if(this.$contentWAuto || this.$p.width.val < this.$p.contentWidth.val){
            if(this.$contentWAuto){
                if(this.$p.width.val !== this.$p.contentWidth.val){
                    this.width = this.$p.contentWidth.val
                    // this.$widthChanged()
                }
                
                this.$contentWAuto = true
            } else {
                this.$p.width.val = this.$p.contentWidth.val
            }
            this.$calcWH()
            // this.$p.width.val = this.contentWidth
            // this.dom.style.width = `${this.width}px`
            // for(let i = 0; i < this.$p.width.depends.length; i++){
            //     this.$p.width.depends[i]()
            // }
            // for(let i = 0; i < this.$p.x.depends.length; i++){
            //     this.$p.x.depends[i]()
            // }
        }
    }
    $contentHeightChanged(){
        if(this.$contentHAuto){
            // if(this.$contentHAuto){
                if(this.$p.height.val !== this.$p.contentHeight.val){
                    this.height = this.$p.contentHeight.val
                    // this.$heightChanged()
                }
                
                this.$contentHAuto = true
                this.$calcWH()
            // } else {
            //     this.height = this.contentHeight
            // }
            // this.$p.height.val = this.contentHeight
            // this.dom.style.height = `${this.height}px`
            // for(let i = 0; i < this.$p.height.depends.length; i++){
            //     this.$p.height.depends[i]()
            // }
            // for(let i = 0; i < this.$p.y.depends.length; i++){
            //     this.$p.y.depends[i]()
            // }
        }
    }
    $widthChanged(){
        super.$widthChanged()
        this.$contentWAuto = false
        this.$calcWH()
    }
    $heightChanged(){
        super.$heightChanged()
        this.$contentHAuto = false
        this.$calcWH()
    }
    $horizontalAlignmentChanged(){
        switch(this.$p.horizontalAlignment.val){
            case Text.AlignLeft: this.dom.style.textAlign = 'left'; this.impl.style.margin = "0 auto 0 0"; break;
            case Text.AlignRight: this.dom.style.textAlign = 'right'; this.impl.style.margin = "0 0 0 auto"; break;
            case Text.AlignHCenter: this.dom.style.textAlign = 'center'; this.impl.style.margin = "0 auto 0 auto"; break;
            case Text.AlignJustify: this.dom.style.textAlign = 'justify'; this.impl.style.margin = "0 0 0 0"; break;
        }
    }
    $verticalAlignmentChanged(){
        switch(this.$p.verticalAlignment.val){
            case Text.AlignTop: this.dom.style.alignItems = 'flex-start'; break;
            case Text.AlignBottom: this.dom.style.alignItems = 'flex-end'; break;
            case Text.AlignVCenter: this.dom.style.alignItems = 'center'; break;
        }
    }
    $wrapModeChanged(){
        switch(this.$p.wrapMode.val){
            case Text.NoWrap: this.impl.style.whiteSpace = "pre"; break;
            case Text.WordWrap: this.impl.style.whiteSpace = "pre-wrap"; this.impl.style.wordWrap = "normal"; break;
            case Text.WrapAnywhere: this.impl.style.whiteSpace = "pre-wrap"; this.impl.style.wordBreak = "break-all"; break;
            case Text.Wrap: this.impl.style.whiteSpace = "pre-wrap"; this.impl.style.wordWrap = "break-word"; break;
            case Text.WrapAtWordBoundaryOrAnywhere: this.impl.style.whiteSpace = "pre-wrap"; this.impl.style.wordBreak = "break-all"; break;
        }
        this.$calcWH()
    }

    $fontChanged(){
        // if(this.$pixelSize !== this.font.pixelSize) this.$p.font.$pointSize = this.font.pixelSize / 0.75
        // if(this.$pointSize !== this.font.pointSize) this.$p.font.$pixelSize = this.font.pointSize * 0.75
        // this.$pixelSize = this.font.pixelSize
        // this.$pointSize = this.font.pointSize

        this.impl.style.fontFamily = this.$p['font.family'].val ? this.$p['font.family'].val : 'unset';
        this.impl.style.fontSize = `${this.$p['font.pixelSize'].val}px`
        this.impl.style.fontStyle = this.$p['font.italic'].val ? 'italic' : 'normal';
        this.impl.style.fontWeight = this.$p['font.bold'].val ? 'bold' : 'normal';
        this.impl.style.textDecoration = this.$p['font.underline'].val ? 'underline' : 'unset';
        this.$calcWH()

        if(this.$p['font.family'].val && this.$p['font.pixelSize'].val){
            document.fonts.load(`${this.$p['font.pixelSize'].val}px ${this.$p['font.family'].val}`).then((fonts)=>{
                if(fonts.length && this.$p) this.$calcWH()
            })
        }
    }
    $destroy(){
        //clearInterval(this.$observer)
        this.impl.remove()
        super.$destroy()
    }
}

QML.Text = Text