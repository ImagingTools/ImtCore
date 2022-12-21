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

    $contentWAuto = true
    $contentHAuto = true

    constructor(args) {
        super(args)

        this.$cP('text', '').connect(this.$textChanged.bind(this))
        this.$cP('color', 'black').connect(this.$colorChanged.bind(this))
        this.$cP('contentHeight', 0).connect(this.$contentHeightChanged.bind(this))
        this.$cP('contentWidth', 0).connect(this.$contentWidthChanged.bind(this))
        this.$cP('horizontalAlignment', TextInput.AlignLeft).connect(this.$horizontalAlignmentChanged.bind(this))
        this.$cP('verticalAlignment', TextInput.AlignTop).connect(this.$verticalAlignmentChanged.bind(this))
        this.$cP('wrapMode', TextInput.NoWrap).connect(this.$wrapModeChanged.bind(this))
        this.$cPC('font', Font()).connect(this.$fontChanged.bind(this))
        this.$cP('selectionColor', '#000080').connect(this.$selectionColorChanged.bind(this))
        this.$cP('selectedTextColor', '#fff').connect(this.$selectedTextColorChanged.bind(this))
        this.$cP('echoMode', TextInput.Normal).connect(this.$echoModeChanged.bind(this))
        this.$cP('validator', undefined).connect(this.$validatorChanged.bind(this))
        this.$cP('readOnly', false).connect(this.$readOnlyChanged.bind(this))


        this.$s.accepted = Signal()
        this.$s.editingFinished = Signal()
        this.$s.textEdited = Signal()
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
                } else {
                    this.impl.value = this.$p.text.val
                }
            } else {
                this.text = this.impl.value
            }
			
		}.bind(this))

        this.$fontChanged()
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
		this.$style.innerHTML = `#el-${this.UID} input::selection{color: ${this.$p.selectedTextColor.val}; background: ${this.$p.selectionColor.val}} #el-${this.UID} input::-moz-selection{color: ${this.$p.selectedTextColor.val}; background: ${this.$p.selectionColor.val}}`
	}

    $contentWidthChanged(){
        if(this.$contentWAuto || this.$p.width.val < this.$p.contentWidth.val){
            if(this.$contentWAuto){
                this.width = this.$p.contentWidth.val
                this.$contentWAuto = true
            } else {
                this.width = this.$p.contentWidth.val
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
        // this.impl.style.width = `${this.contentWidth}px`
    }
    $contentHeightChanged(){
        if(this.$contentHAuto){
            this.height = this.$p.contentHeight.val
            this.$contentHAuto = true
            this.$calcWH()
            // this.$p.height.val = this.contentHeight
            // this.dom.style.height = `${this.height}px`
            // for(let i = 0; i < this.$p.height.depends.length; i++){
            //     this.$p.height.depends[i]()
            // }
            // for(let i = 0; i < this.$p.y.depends.length; i++){
            //     this.$p.y.depends[i]()
            // }
        }
        // this.impl.style.height = `${this.contentHeight}px`
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

    $textChanged(){
		this.impl.value = this.$p.text.val

        // let tempText = document.createElement("span")
        // tempText.innerHTML = this.text
        // document.body.appendChild(tempText);

        // let rect = tempText.getBoundingClientRect()
        // this.contentWidth = rect.width
        // this.contentHeight = rect.height
        // tempText.remove()
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
        this.$calcWH()
    }

    $fontChanged(){
        this.impl.style.fontSize = `${this.$p['font.pixelSize'].val}px`
        this.impl.style.fontStyle = this.$p['font.italic'].val ? 'italic' : 'normal';
        this.impl.style.fontWeight = this.$p['font.bold'].val ? 'bold' : 'normal';
        this.impl.style.textDecoration = this.$p['font.underline'].val ? 'underline' : 'unset';
        this.impl.style.height = `${this.$p['font.pixelSize'].val}px`
        this.$calcWH()

        if(this.$p['font.family'].val && this.$p['font.pixelSize'].val){
            document.fonts.load(`${this.$p['font.pixelSize'].val}px ${this.$p['font.family'].val}`).then((fonts)=>{
                if(fonts.length) this.$calcWH()
            })
        }
    }

    $keydown(e, state){
        if(e.key === 'Enter' || e.key === 'Return'){
            e.preventDefault()
            this.$s.accepted()
        }
    }

    $destroy(){
        // if(this.$validator) this.$validator.$destroy()
        this.impl.remove()
        super.$destroy()
    }

    $mousedown(e, state) {
        e.preventDefault()
		if(this.$p.enabled.val){
            state.blocked(this)
		}
	}
	$mouseup(e, state) {
		e.preventDefault()
		if(this.$p.enabled.val){
            this.focus = true
            state.release()
		}
	}
    
}

QML.TextInput = TextInput