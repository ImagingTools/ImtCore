import {Item} from './Item'
import {Font} from '../utils/Font'

export class TextEdit extends Item {

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

    $contentWAuto = true
    $contentHAuto = true

    constructor(parent) {
        super(parent)

        this.$cP('text', '').connect(this.$textChanged.bind(this))
        this.$cP('color', 'black').connect(this.$colorChanged.bind(this))
        this.$cP('contentHeight', 0).connect(this.$contentHeightChanged.bind(this))
        this.$cP('contentWidth', 0).connect(this.$contentWidthChanged.bind(this))
        this.$cP('horizontalAlignment', TextEdit.AlignLeft).connect(this.$horizontalAlignmentChanged.bind(this))
        this.$cP('verticalAlignment', TextEdit.AlignTop).connect(this.$verticalAlignmentChanged.bind(this))
        this.$cP('wrapMode', TextEdit.NoWrap).connect(this.$wrapModeChanged.bind(this))
        this.$cPC('font', Font()).connect(this.$fontChanged.bind(this))
        this.$cP('selectionColor', '#000080').connect(this.$selectionColorChanged.bind(this))
        this.$cP('selectedTextColor', '#fff').connect(this.$selectedTextColorChanged.bind(this))
        this.$cP('readOnly', 'bool', false).connect(this.$readOnlyChanged.bind(this))

    }

    $domCreate(){
        super.$domCreate()

        this.impl = document.createElement("div")
        this.dom.style.display = 'flex'
        this.dom.appendChild(this.impl);
        
        this.impl.style.textAlign = "inherit"
        this.impl.style.width = "100%";
        this.impl.style.height = "auto";

		this.impl.style.borderWidth = "0"
		this.impl.style.background = "none"
		this.impl.style.outline = "none"
        this.impl.contentEditable = true


        this.impl.addEventListener("input", function(e) {
            //e.preventDefault()
			this.text = this.impl.innerText
		}.bind(this))

        this.$fontChanged()
    }
    $show(){
        this.dom.style.display = 'flex'
    }
    $readOnlyChanged(){
        if(this.$p.readOnly.val){
            this.impl.contentEditable = true
        } else {
            this.impl.contentEditable = false
        }
    }
    $focusChanged(){
        super.$focusChanged()
        if(this.$p.focus.val) this.impl.focus()
    }
    $selectionColorChanged(){
        this.$updateSelection()
    }
    $selectedTextColorChanged(){
        this.$updateSelection()
    }
    $updateSelection(){
		if(this.$style) this.$style.remove() 
		this.$style = document.head.appendChild(document.createElement("style"))
		this.$style.innerHTML = `#el-${this.UID} div::selection{color: ${this.$p.selectedTextColor.val}; background: ${this.$p.selectionColor.val}} #el-${this.UID} div::-moz-selection{color: ${this.$p.selectedTextColor.val}; background: ${this.$p.selectionColor.val}}`
	}

    $getCursorPosition() {
		let selection = document.getSelection()
		if(!selection.anchorNode || !selection.anchorOffset) return 0
		let range = new Range
		range.setStart(this.impl, 0)
		range.setEnd(selection.anchorNode, selection.anchorOffset)
		return range.toString().length
	}

	$setCursorPosition(position) {
		let child = this.impl.firstChild
		while(position > 0) {
			let length = child.textContent.length
			if(position > length) {
			position -= length
			child = child.nextSibling
			}
			else {
			if(child.nodeType == 3) return document.getSelection().collapse(child, position)
			child = child.firstChild
			}
		}
	}

    $contentWidthChanged(){
        if(this.$contentWAuto || this.$p.width.val < this.$p.contentWidth.val){
            if(this.$contentWAuto) {
                this.width = this.$p.contentWidth.val
                this.$contentWAuto = true
            } else {
                this.width = this.$p.contentWidth.val
            }
        }
    }
    $contentHeightChanged(){
        if(this.$contentHAuto){
            this.height = this.$p.contentHeight.val
            this.$contentHAuto = true
        }
    }
    $widthChanged(){
        super.$widthChanged()
        this.$contentWAuto = false
    }
    $heightChanged(){
        super.$heightChanged()
        this.$contentHAuto = false
    }

    $textChanged(){
		this.impl.innerText = this.$p.text.val
        this.$setCursorPosition(this.$cursorPos + (this.$p.text.val.length - this.$cursorOffset))
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
        }
        if(!this.$contentHAuto){
            tempDom.style.height = this.dom.style.height
        }

        document.body.appendChild(tempDom);
        tempDom.appendChild(tempText);

        let rect = tempText.getBoundingClientRect()
        this.contentWidth = rect.width
        this.contentHeight = rect.height
        tempDom.remove()
    }
    $colorChanged(){
        this.impl.style.color = `${this.$p.color.val}`
    }

    $horizontalAlignmentChanged(){
        switch(this.$p.horizontalAlignment.val){
			case TextEdit.AlignLeft:
				this.impl.style.textAlign = "start";
				this.dom.style.justifyContent = "flex-start";
				break;
			case TextEdit.AlignRight:
				this.impl.style.textAlign = "end";
				this.dom.style.justifyContent = "flex-end";
				break;
			case TextEdit.AlignHCenter:
				this.impl.style.textAlign = "center";
				this.dom.style.justifyContent = "center";
				break;
		}
    }
    $verticalAlignmentChanged(){
        switch(this.$p.verticalAlignment.val){
			case TextEdit.AlignTop:
				this.dom.style.alignItems = "flex-start";
				break;
			case TextEdit.AlignBottom:
				this.dom.style.alignItems = "flex-end";
				break;
			case TextEdit.AlignVCenter:
				this.dom.style.alignItems = "center";
				break;
		}
    }
    $wrapModeChanged(){
        switch(this.$p.wrapMode.val){
            case TextEdit.NoWrap: this.impl.style.whiteSpace = "pre"; break;
            case TextEdit.WordWrap: this.impl.style.whiteSpace = "pre-wrap"; this.impl.style.wordWrap = "normal"; break;
            case TextEdit.WrapAnywhere: this.impl.style.whiteSpace = "pre-wrap"; this.impl.style.wordBreak = "break-all"; break;
            case TextEdit.Wrap: this.impl.style.whiteSpace = "pre-wrap"; this.impl.style.wordWrap = "break-word"; break;
        }
        this.$calcWH()
    }

    $fontChanged(){
        this.impl.style.fontSize = `${this.$p['font.pixelSize'].val}px`
        this.impl.style.fontStyle = this.$p['font.italic'].val ? 'italic' : 'normal';
        this.impl.style.fontWeight = this.$p['font.bold'].val ? 'bold' : 'normal';
        this.impl.style.textDecoration = this.$p['font.underline'].val ? 'underline' : 'unset';
        // this.impl.style.height = `${this.font.pixelSize}px`
        this.$calcWH()
    }

    $keydown(e, state){
        this.$cursorPos = this.$p.text.val.length ? this.$getCursorPosition() : 0
        this.$cursorOffset = this.$p.text.val.length
    }
}

QML.TextEdit = TextEdit