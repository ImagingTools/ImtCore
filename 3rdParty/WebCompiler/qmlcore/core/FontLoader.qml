//item for image displaying
Item {
	property string source;									///< font URL
	property enum status { Null, Ready, Loading, Error };	///< font status
	property string name;	
	///@private
	constructor: {
		this._scheduleLoad()
	}

	///@private
	function _scheduleLoad() {
		this._context.delayedAction('font.load', this, this._load)
	}

	///@private
	function _onError() {
		this.status = this.Error;
	}

	///@private
	function _load() {
		if (!this.source) {
			return
		}

		this.status = this.Loading
		var ctx = this._context
		var callback = this._fontLoaded.bind(this)

        let xhr = new XMLHttpRequest()
        xhr.open('GET', this.source)
        xhr.onload = () => {
            if(xhr.status >= 200 && xhr.status <= 300){
				callback()
				this.status = this.Ready
            } else {
                this.status = this.Error
            }
        }
        xhr.onerror = () => {
            this.status = this.Error
        }
        xhr.send()
	}

	onSourceChanged: {
		this.status = this.Null
		this._scheduleLoad()
	}

	///@private
	function _fontLoaded() {
		let temp = this.source.split('/')
		let name = temp[temp.length-1].split('.')[0]
        let domStyle = document.createElement("style")
        domStyle.innerHTML = `@font-face { font-family: ${this.name}; src: url('${this.source}'); }`
        document.head.appendChild(domStyle)
		this.name = name	
	}
}
