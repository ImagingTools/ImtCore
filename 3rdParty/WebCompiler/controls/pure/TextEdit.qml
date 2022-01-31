///HTML text input item
Item {
	property enum horizontalAlignment { AlignLeft, AlignRight, AlignHCenter, AlignJustify };
	property enum verticalAlignment { AlignTop, AlignBottom, AlignVCenter };
	property enum wrapMode { NoWrap, WordWrap, WrapAnywhere, Wrap, WrapAtWordBoundaryOrAnywhere };
	property string text;
	property Font font: Font {}
	property Color color: "#000";
	property Color selectionColor: '#000080';
	property Color selectedTextColor: '#fff';
	property bool selectByMouse: false;

	onTextChanged: { 
		this._updateValue(value) 
		this._updateCursorPos()
	}

	function _updateCursorPos(){
		if(this.text === '') {
			switch(this.horizontalAlignment){
			case TextEdit.AlignLeft:
				this.element.dom.style.paddingLeft = "0";
				break;
			case TextEdit.AlignRight:
				this.element.dom.style.paddingLeft = `${this.width - this.font.pixelSize*0.25}px`;
				break;
			case TextEdit.AlignHCenter:
				this.element.dom.style.paddingLeft = `${this.width*0.5 - this.font.pixelSize*0.25}px`;
				break;
			}
			switch(this.verticalAlignment){
			case TextEdit.AlignTop:
				this.element.dom.style.paddingTop = "0";
				break;
			case TextEdit.AlignBottom:
				this.element.dom.style.paddingTop = `${this.height - this.font.pixelSize*1.25}px`;
				break;
			case TextEdit.AlignVCenter:
				this.element.dom.style.paddingTop = `${this.height*0.5 - this.font.pixelSize*0.5}px`;
				break;
			}
		} else {
			this.element.dom.style.paddingLeft = "0"
			this.element.dom.style.paddingTop = "0"
		}
	}

	function _getValue(){
		return this.element.dom.innerHTML
	}

	function _updateValue(value){
		let pos = this.text.length ? this._getCursorPosition() : value.length
		this.element.dom.innerHTML = this.text
		this._setCursorPosition(pos)
	}

	function _getCursorPosition() {
		let _selection = this._context.backend.document.getSelection()
		if(!_selection.anchorNode || !_selection.anchorOffset) return 0
		let range = new Range
		range.setStart(this.element.dom, 0)
		range.setEnd(_selection.anchorNode, _selection.anchorOffset)
		return range.toString().length
	}

	function _setCursorPosition(position) {
		let child = this.element.dom.firstChild
		while(position > 0) {
			let length = child.textContent.length
			if(position > length) {
			position -= length
			child = child.nextSibling
			}
			else {
			if(child.nodeType == 3) return this._context.backend.document.getSelection().collapse(child, position)
			child = child.firstChild
			}
		}
	}

	constructor: {
		this.element.on("input", function() {
			this.text = this._getValue()
			
		}.bind(this))
		this.element.dom.style.display = "flex"
		this.element.dom.style.pointerEvents = "all"
		this.element.dom.style.boxSizing = "border-box"
		this.element.dom.style.borderWidth = "0"
		this.element.dom.style.background = "none"
		this.element.dom.style.outline = "none"
		this.element.dom.style.overflow = "auto"
		this.element.dom.style.flexWrap = "wrap"
		this.element.dom.contentEditable = true

	}

	onSelectedTextColor,
	onSelectionColorChanged: {
		this._updateSelection()
	}

	onCompleted: {
		this._updateSelection()
	}

	function _updateSelection(){
		if(this._style) this._style.remove() 
		this._style = this._context.backend.document.head.appendChild(this._context.backend.document.createElement("style"));
		this._style.innerHTML = `#el-${this._uid}::selection{color: ${this.selectedTextColor}; background: ${this.selectionColor}} #el-${this._uid}::-moz-selection{color: ${this.selectedTextColor}; background: ${this.selectionColor}}`
	}

	onHorizontalAlignmentChanged:{
		switch(this.horizontalAlignment){
		case TextEdit.AlignLeft:
			this.element.dom.style.justifyContent = 'flex-start';
			break;
		case TextEdit.AlignRight:
			this.element.dom.style.justifyContent = 'flex-end';
			break;
		case TextEdit.AlignHCenter:
			this.element.dom.style.justifyContent = 'center';
			break;
		}
		this._updateCursorPos()
	}
	onVerticalAlignmentChanged:{
		switch(this.verticalAlignment){
		case TextEdit.AlignTop:
			this.element.dom.style.alignItems = 'flex-start';
			break;
		case TextEdit.AlignBottom:
			this.element.dom.style.alignItems = 'flex-end';
			break;
		case TextEdit.AlignVCenter:
			this.element.dom.style.alignItems = 'center';
			break;
		}
		this._updateCursorPos()
	}

	onWrapModeChanged:{
		switch(this.wrapMode){
		case TextEdit.NoWrap:
			this.element.dom.style.wordBreak = 'normal'
			this.element.dom.style.whiteSpace = 'nowrap'
			break;
		case TextEdit.WordWrap:
			this.element.dom.style.wordBreak = 'break-word'
			this.element.dom.style.whiteSpace = 'break-spaces'
			break;
		case TextEdit.WrapAnywhere:
			this.element.dom.style.wordBreak = 'break-all'
			this.element.dom.style.whiteSpace = 'break-spaces'
			break;
		case TextEdit.WrapAtWordBoundaryOrAnywhere:
			this.element.dom.style.wordBreak = 'break-all'
			this.element.dom.style.whiteSpace = 'break-spaces'
			break;
		case TextEdit.Wrap:
			this.element.dom.style.wordBreak = 'break-all'
			this.element.dom.style.whiteSpace = 'break-spaces'
			break;
		}
	}

	onWidthChanged,
	onHeightChanged: {
		this._updateCursorPos()
	}

}
