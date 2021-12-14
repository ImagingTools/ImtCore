///HTML text input item
BaseInput {
	property string text;		///< input text string property
	property bool passwordMode: false;	///< show password chars instead of input text flag property
	property Color cursorColor: "#fff";	///< cursor color
	height: 20;	///<@private
	width: 173;	///<@private
	type: passwordMode ? "password" : "text";	///<@private
	property Object validator;

	onTextChanged: { this._updateValue(value) }

	constructor: {
		this.element.on("input", function() {
			if(this.validator){
				let text = this._getValue()
				console.log(1)
				if(this.validator.__re.test(text)){
					this.text = text
				} else {
					this._updateValue(this.text)
				}
			} else {
				this.text = this._getValue()
			}
			
			
		}.bind(this))
	}

	onValidatorChanged:{
		
	}

	onCompleted: {

	}

	onCursorColorChanged: { this.style('caret-color', value) }


}
