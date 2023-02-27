import {Item} from './Item'
import {Font} from '../utils/Font'
import {Signal} from '../utils/Signal'

export class TextInput extends Item {

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

    $contentWidthAuto = true
	$contentHeightAuto = true

    $widthAuto = true
	$heightAuto = true

    constructor(args) {
        super(args)

        this.$cP('text', '').connect(this.$textChanged.bind(this))
        this.$cP('color', 'black').connect(this.$colorChanged.bind(this))
        this.$cP('contentHeight', 0)//.connect(this.$contentHeightChanged.bind(this))
        this.$cP('contentWidth', 0)//.connect(this.$contentWidthChanged.bind(this))
        this.$cP('horizontalAlignment', TextInput.AlignLeft).connect(this.$horizontalAlignmentChanged.bind(this))
        this.$cP('verticalAlignment', TextInput.AlignTop).connect(this.$verticalAlignmentChanged.bind(this))
        this.$cP('wrapMode', TextInput.NoWrap).connect(this.$wrapModeChanged.bind(this))
        this.$cPC('font', Font()).connect(this.$fontChanged.bind(this))
        this.$cP('selectionColor', '#000080').connect(this.$selectionColorChanged.bind(this))
        this.$cP('selectedTextColor', '#fff').connect(this.$selectedTextColorChanged.bind(this))
        this.$cP('echoMode', TextInput.Normal).connect(this.$echoModeChanged.bind(this))
        this.$cP('validator', undefined).connect(this.$validatorChanged.bind(this))
        this.$cP('readOnly', false).connect(this.$readOnlyChanged.bind(this))
        this.$cP('acceptableInput', true)


        this.$cS('accepted')
        this.$cS('editingFinished')
        this.$cS('textEdited')
  

        this.$updateTimer = null
    }
    $validatorChanged(){
        // if(this.$validator) this.$validator.$destroy()

        // this.validator(this)
        // this.$validator = this.children.splice(this.children.length-1, 1)[0]
    }

    $domCreate(){
        super.$domCreate()

        this.impl = document.createElement("input")
        this.dom.style.display = 'flex'
        this.dom.appendChild(this.impl);
        
        this.impl.style.textAlign = "inherit"
        this.impl.style.width = "100%";

		this.impl.style.borderWidth = "0"
		this.impl.style.background = "none"
		this.impl.style.outline = "none"


        this.impl.addEventListener("input", function(e) {
            if(this.$p.validator.val){
                if(this.$p.validator.val.validate(this.impl.value)){
                    this.text = this.impl.value
                    this.acceptableInput = true
                } else {
                    this.impl.value = this.$p.text.val
                    this.acceptableInput = false
                }
            } else {
                this.text = this.impl.value
                this.acceptableInput = true
            }
			
		}.bind(this))

        this.$fontChanged()
    }
    $updateGeometry(){
        clearTimeout(this.$updateTimer)
        
        this.$updateTimer = setTimeout(()=>{
            if(this.impl){
                let rect = this.impl.getBoundingClientRect()
                if(this.$contentHeightAuto){
                    if(this.$heightAuto && this.$p.height.val !== rect.height) {
                        this.$p.height.val = rect.height
                        this.$p.height.signal()
                        this.$heightAuto = true
                        // this.dom.style.height = `${rect.height}px`
                    }
                    this.contentHeight = rect.height
                }
                if(this.$contentWidthAuto){
                    if(this.$widthAuto && this.$p.width.val !== rect.width) {
                        this.$p.width.val = rect.width
                        this.$p.width.signal()
                        this.$widthAuto = true
                        // this.dom.style.width = `${rect.width}px`
                    }
                    this.contentWidth = rect.width
                }
            }
            
        }, 1000 / Core.FPS)
        

    }
    $show(){
        this.dom.style.display = 'flex'
    }
    $echoModeChanged(){
        switch(this.echoMode){
			case TextInput.Normal:
				this.impl.type = "text";
				break;
			case TextInput.Password:
				this.impl.type = "password";
				break;
		}
    }
    $readOnlyChanged(){
        if(this.readOnly){
            this.impl.readOnly = true
        } else {
            this.impl.readOnly = false
        }
    }
    $focusChanged(){
        super.$focusChanged()
        if(this.$p.focus.val) {
            this.impl.focus()
        } else {
            this.$s.editingFinished()
        }
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
		this.$style.innerHTML = `#el-${this.UID} input::selection{color: ${this.$p.selectedTextColor.val}; background: ${this.$p.selectionColor.val}} #el-${this.UID} input::-moz-selection{color: ${this.$p.selectedTextColor.val}; background: ${this.$p.selectionColor.val}}`
	}

    
    $widthChanged(){
        super.$widthChanged()
        this.$widthAuto = false
        // this.$contentWAuto = false
        // this.$calcWH()
        this.$updateGeometry()
        this.$horizontalAlignmentChanged()
    }
    $heightChanged(){
        super.$heightChanged()
        this.$heightAuto = false
        // this.$contentHAuto = false
        // this.$calcWH()
        this.$updateGeometry()
        this.$verticalAlignmentChanged()
    }

    $textChanged(){
		this.impl.value = this.$p.text.val

        // let tempText = document.createElement("span")
        // tempText.innerHTML = this.text
        // document.body.appendChild(tempText);

        // let rect = tempText.getBoundingClientRect()
        // this.contentWidth = rect.width
        // this.contentHeight = rect.height
        // tempText.remove()
        this.$updateGeometry()
    }

    
    selectAll(){
        this.impl.select()
    }
    $colorChanged(){
        this.impl.style.color = `${this.$p.color.val}`
    }

    $horizontalAlignmentChanged(){
        switch(this.$p.horizontalAlignment.val){
			case TextInput.AlignLeft:
				this.impl.style.textAlign = "start";
				this.dom.style.justifyContent = "flex-start";
				break;
			case TextInput.AlignRight:
				this.impl.style.textAlign = "end";
				this.dom.style.justifyContent = "flex-end";
				break;
			case TextInput.AlignHCenter:
				this.impl.style.textAlign = "center";
				this.dom.style.justifyContent = "center";
				break;
		}
    }
    $verticalAlignmentChanged(){
        switch(this.$p.verticalAlignment.val){
			case TextInput.AlignTop:
				this.dom.style.alignItems = "flex-start";
				break;
			case TextInput.AlignBottom:
				this.dom.style.alignItems = "flex-end";
				break;
			case TextInput.AlignVCenter:
				this.dom.style.alignItems = "center";
				break;
		}
    }
    $wrapModeChanged(){
        switch(this.$p.wrapMode.val){
            case TextInput.NoWrap: this.impl.style.whiteSpace = "pre"; break;
            case TextInput.WordWrap: this.impl.style.whiteSpace = "pre-wrap"; this.impl.style.wordWrap = "normal"; break;
            case TextInput.WrapAnywhere: this.impl.style.whiteSpace = "pre-wrap"; this.impl.style.wordBreak = "break-all"; break;
            case TextInput.Wrap: this.impl.style.whiteSpace = "pre-wrap"; this.impl.style.wordWrap = "break-word"; break;
        }
        this.$updateGeometry()
    }

    $fontChanged(){
        this.impl.style.fontSize = `${this.$p['font.pixelSize'].val}px`
        this.impl.style.fontStyle = this.$p['font.italic'].val ? 'italic' : 'normal';
        this.impl.style.fontWeight = this.$p['font.bold'].val ? 'bold' : 'normal';
        this.impl.style.textDecoration = this.$p['font.underline'].val ? 'underline' : 'unset';
        this.impl.style.height = `${this.$p['font.pixelSize'].val}px`
        this.$updateGeometry()

        if(this.$p['font.family'].val && this.$p['font.pixelSize'].val){
            document.fonts.load(`${this.$p['font.pixelSize'].val}px ${this.$p['font.family'].val}`).then((fonts)=>{
                if(fonts.length) this.$updateGeometry()
            })
        }
    }

    $keydown(e, state){
        if(e.key === 'Enter' || e.key === 'Return'){
            e.preventDefault()
            if(this.$p.validator.val){
                if(this.$p.validator.val.validate(this.impl.value)){
                    this.$s.accepted()
                    this.$s.editingFinished()
                    this.acceptableInput = true
                } else {
                    this.acceptableInput = false
                }
            } else {
                this.$s.accepted()
                this.$s.editingFinished()
                this.acceptableInput = true
            }
            
        }
    }

    $destroy(){
        // if(this.$validator) this.$validator.$destroy()
        clearTimeout(this.$updateTimer)
        this.impl.remove()
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
            this.focus = true
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
            this.focus = true
            state.release()
		}
	}
    $touchmove(e, state) {
		// e.preventDefault()
		if(this.$p.enabled.val){

		}
		
	}
    
}

QML.TextInput = TextInput