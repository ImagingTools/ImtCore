///HTML text input item
BaseInput {
	property string text;		///< input text string property
	property enum echoMode { Normal, Password };
	property bool readOnly: false;
	property Color cursorColor: "#fff";	///< cursor color
	property Object validator;

	onTextChanged: { this._updateValue(value) }

	constructor: {
		this.element.on("input", function() {
			if(this.validator){

				let text = this._getValue()
				if(!this.validator.regExp || this.validator.regExp.test(text)){
					this.text = text
				} else {
					this._updateValue(this.text)
				}
			} else {
				this.text = this._getValue()
			}
			
			
		}.bind(this))
		this.element.on("keyup", function(e) { 
			if(e.key === "Enter" || e.key === "Return"){
				if(this.validator){
					let text = this._getValue()
					if(!this.validator.regExp || this.validator.regExp.test(text)){
						this.accepted()
					} else {
						// not accepted
					}
				} else {
					this.accepted()
				}
			}
		}.bind(this))
		this.element.dom.style.background = "none"
	}

	onValidatorChanged:{
		
	}

	onCompleted: {

	}

	onReadOnlyChanged: {
		if(this.readOnly){
			this.element.dom.readOnly = true
		} else {
			this.element.dom.readOnly = false
		}
	}

	onEchoModeChanged:{
		if(this.echoMode === TextInput.Password) {
			this.element.dom.type = 'password'
		} else {
			this.element.dom.type = 'text';
		}
	}

	onCursorColorChanged: { this.style('caret-color', value) }

	
}
