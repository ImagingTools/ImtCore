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
        this.$cP('horizontalAlignment', TextEdit.AlignLeft, this.$horizontalAlignmentChanged)
        this.$cP('verticalAlignment', TextEdit.AlignTop, this.$verticalAlignmentChanged)
        this.$cP('wrapMode', TextEdit.NoWrap, this.$wrapModeChanged)
        this.$cPC('font', Font(), this.$fontChanged)
        this.$cP('selectionColor', '#000080', this.$selectionColorChanged)
        this.$cP('selectedTextColor', '#fff', this.$selectedTextColorChanged)
        this.$cP('readOnly', false, this.$readOnlyChanged)
        // this.$sP('height', ()=>{return this.contentHeight})
        this.$cS('editingFinished')
        this.$sP('width', ()=>{return this.contentWidth})
        this.$sP('height', ()=>{return this.contentHeight})

        this.$updateTimer = null
    }

    $domCreate(){
        super.$domCreate()

        this.impl = document.createElement("textarea")
        // this.dom.style.display = 'flex'
        this.dom.appendChild(this.impl);
        
        this.impl.style.textAlign = "inherit"
        this.impl.style.width = "100%";
        this.impl.style.height = '100%'
        this.impl.style.resize = 'none'

		this.impl.style.borderWidth = "0"
		this.impl.style.outline = "none"
        this.impl.style.whiteSpace = "pre"; 
		this.impl.cols = 1
		this.impl.rows = 1


        this.impl.addEventListener("input", function(e) {
            //e.preventDefault()
			this.text = this.impl.value
            this.$updateGeometry()
		}.bind(this))

        this.$hiddenElement = document.createElement("div")
        this.dom.appendChild(this.$hiddenElement)
        this.$hiddenElement.style.textAlign = "inherit"
        // this.$hiddenElement.style.width = "100%";
        // this.$hiddenElement.style.height = '100%'
        this.$hiddenElement.style.visibility = 'hidden'
        this.$hiddenElement.style.zIndex = -1
        this.$hiddenElement.style.display = 'flex'
        this.$hiddenElement.style.position = 'absolute'
        

        this.$hiddenElementContent = document.createElement("span")
        this.$hiddenElement.appendChild(this.$hiddenElementContent)
        this.$hiddenElement.style.maxWidth = '100%'
        this.$hiddenElementContent.style.whiteSpace = "pre";
        this.$hiddenElementContent.style.lineHeight = "normal";


        this.$fontChanged()
    }
    $show(){
        this.dom.style.display = 'flex'
    }
    $readOnlyChanged(){
        // if(this.$p.readOnly.val){
        //     this.impl.contentEditable = true
        // } else {
        //     this.impl.contentEditable = false
        // }
    }
    $focusChanged(){
        super.$focusChanged()
        if(this.$p.focus.val) {
            this.$updateGeometry()
            this.impl.focus()
            Core.setFocus(this)
        } else {
            this.impl.blur()
            this.editingFinished()
        }
        
    }
    $opacityChanged(){
        this.dom.style.opacity = this.opacity
    }
    $updateGeometry(){
        if(this.impl.value.length){
            this.$hiddenElementContent.innerHTML = this.impl.value[this.impl.value.length-1] === '\n' ? this.impl.value + '!' : this.impl.value
            this.contentWidth = this.$hiddenElement.scrollWidth
        } else {
            this.$hiddenElementContent.innerHTML = '!'
            this.contentWidth = 0
        }

        // this.contentWidth = this.$hiddenElement.scrollWidth
        this.contentHeight = this.$hiddenElement.scrollHeight

        let width = this.$hiddenElement.scrollWidth - this.$hiddenElementContent.scrollWidth
        let height = this.text === '' ? this.$hiddenElement.scrollHeight - this.font.pixelSize : this.$hiddenElement.scrollHeight - this.$hiddenElementContent.scrollHeight
        this.impl.style.padding = `0`
        if(this.verticalAlignment === TextEdit.AlignVCenter){
            this.impl.style.paddingTop = `${height/2}px`
            this.impl.style.paddingBottom = `${height/2}px`
        }
        if(this.horizontalAlignment === TextEdit.AlignHCenter){
            this.impl.style.paddingLeft = `${width/2}px`
            this.impl.style.paddingRight = `${width/2}px`
        }
        // console.log(width, height)
        // clearTimeout(this.$updateTimer)
        
        // this.$updateTimer = setTimeout(()=>{
        //     if(this.impl){
        //         let rect = this.impl.getBoundingClientRect()
        //         if(this.$contentHeightAuto){
        //             if(this.$heightAuto && this.$p.height.val !== rect.height) {
        //                 this.$p.height.val = rect.height
        //                 this.$p.height.signal()
        //                 this.$heightAuto = true
        //                 // this.dom.style.height = `${rect.height}px`
        //             }
        //             this.contentHeight = rect.height
        //         }
        //         if(this.$contentWidthAuto){
        //             if(this.$widthAuto && this.$p.width.val !== rect.width) {
        //                 this.$p.width.val = rect.width
        //                 this.$p.width.signal()
        //                 this.$widthAuto = true
        //                 // this.dom.style.width = `${rect.width}px`
        //             }
        //             this.contentWidth = rect.width
        //         }
        //     }
            
        // }, 1000 / Core.FPS)
        

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

    }
    $contentHeightChanged(){

    }
    $widthChanged(){
        super.$widthChanged()
        this.$widthAuto = false
    }
    $heightChanged(){
        super.$heightChanged()
        this.$heightAuto = false
    }

    $textChanged(){
		this.impl.value = this.text
        // this.$setCursorPosition(this.$cursorPos + (this.$p.text.val.length - this.$cursorOffset))
        // this.$updateGeometry()
    }
    
    $colorChanged(){
        this.impl.style.color = `${this.$p.color.val}`
    }

    $horizontalAlignmentChanged(){
        switch(this.$p.horizontalAlignment.val){
			case TextEdit.AlignLeft:
				this.impl.style.textAlign = "start";
				this.$hiddenElement.style.justifyContent = "flex-start";
				break;
			case TextEdit.AlignRight:
				this.impl.style.textAlign = "end";
				this.$hiddenElement.style.justifyContent = "flex-end";
				break;
			case TextEdit.AlignHCenter:
				this.impl.style.textAlign = "center";
				this.$hiddenElement.style.justifyContent = "center";
				break;
		}
        this.$updateGeometry()
    }
    $verticalAlignmentChanged(){
        switch(this.$p.verticalAlignment.val){
			case TextEdit.AlignTop:
				this.$hiddenElement.style.alignItems = "flex-start";
				break;
			case TextEdit.AlignBottom:
				this.$hiddenElement.style.alignItems = "flex-end";
				break;
			case TextEdit.AlignVCenter:
				this.$hiddenElement.style.alignItems = "center";
				break;
		}
        this.$updateGeometry()
    }
    $wrapModeChanged(){
        switch(this.$p.wrapMode.val){
            case TextEdit.NoWrap: this.impl.style.whiteSpace = "pre"; this.$hiddenElementContent.style.whiteSpace = "pre"; break;
            case TextEdit.WordWrap: this.impl.style.whiteSpace = "pre-wrap"; this.impl.style.wordWrap = "normal"; this.$hiddenElementContent.style.whiteSpace = "pre-wrap"; this.$hiddenElementContent.style.wordWrap = "normal"; break;
            case TextEdit.WrapAnywhere: this.impl.style.whiteSpace = "pre-wrap"; this.impl.style.wordBreak = "break-all"; this.$hiddenElementContent.style.whiteSpace = "pre-wrap"; this.$hiddenElementContent.style.wordBreak = "break-all"; break;
            case TextEdit.Wrap: this.impl.style.whiteSpace = "pre-wrap"; this.impl.style.wordWrap = "break-word"; this.$hiddenElementContent.style.whiteSpace = "pre-wrap"; this.$hiddenElementContent.style.wordWrap = "break-word"; break;
        }
        this.$updateGeometry()
    }

    $fontChanged(){
        this.impl.style.fontFamily = this.$p['font.family'].val ? this.$p['font.family'].val : 'unset';
        this.impl.style.fontSize = `${this.$p['font.pixelSize'].val}px`
        this.impl.style.fontStyle = this.$p['font.italic'].val ? 'italic' : 'normal';
        this.impl.style.fontWeight = this.$p['font.bold'].val ? 'bold' : 'normal';
        this.impl.style.textDecoration = this.$p['font.underline'].val ? 'underline' : 'unset';

        this.$hiddenElementContent.style.fontFamily = this.$p['font.family'].val ? this.$p['font.family'].val : 'unset';
        this.$hiddenElementContent.style.fontSize = `${this.$p['font.pixelSize'].val}px`
        this.$hiddenElementContent.style.fontStyle = this.$p['font.italic'].val ? 'italic' : 'normal';
        this.$hiddenElementContent.style.fontWeight = this.$p['font.bold'].val ? 'bold' : 'normal';
        this.$hiddenElementContent.style.textDecoration = this.$p['font.underline'].val ? 'underline' : 'unset';

        this.$updateGeometry()
    }

    $keydown(e, state){
        // this.$cursorPos = this.$p.text.val.length ? this.$getCursorPosition() : 0
        // this.$cursorOffset = this.$p.text.val.length
    }

    $destroy(){
        // if(this.$validator) this.$validator.$destroy()
        clearTimeout(this.$updateTimer)
        this.impl.remove()
        this.$hiddenElement.remove()
        super.$destroy()
    }

    $mousedown(e, state) {
        // e.preventDefault()
		if(this.$p.enabled.val){

            state.blocked(this)
		}
	}
	$mouseup(e, state) {
		// e.preventDefault()
		if(this.$p.enabled.val){
            if(this.focus) {
                this.impl.focus()
                Core.setFocus(this)
            } else {
                this.focus = true
            }

            state.release()
		}
	}
    $mousemove(e, state) {
		// e.preventDefault()
		if(this.$p.enabled.val){

		}
		
	}

    $touchstart(e, state) {
        // e.preventDefault()
		if(this.$p.enabled.val){

            state.blocked(this)
		}
	}
	$touchend(e, state) {
		// e.preventDefault()
		if(this.$p.enabled.val){
            if(this.focus) {
                this.impl.focus()
                Core.setFocus(this)
            } else {
                this.focus = true
            }
            
            state.release()
		}
	}
    $touchmove(e, state) {
		// e.preventDefault()
		if(this.$p.enabled.val){

		}
		
	}
}

QML.TextEdit = TextEdit