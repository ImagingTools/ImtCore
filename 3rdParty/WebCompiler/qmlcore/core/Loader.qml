/// object that helps loading components dynamically
Item {
	signal loaded;				///< when requested component it loaded event signal
	signal itemCompleted;		///< fires after item onCompleted has been fired and item is fully constructed
	property string source;		///< component's URL
	property Object item: Item {}		///< item for storing requested component
	property bool trace;		///< log loading objects
	property bool asynchronous; //by Artur
	
	constructor: {
		this.__ready = false
	}

	///@private
	function discardItem() {
		//var item = this.item
		if (this.__properties.item.value) {
			this.item.discard()
			this.__properties.item.value = null
		}
	}

	///@private
	function discard() {
		
		this.discardItem()
		$core.Item.prototype.discard.call(this)
	}

	///@internal

	onSourceChanged: {
		if(this.__ready){
			this.discardItem()
			this._load()
		}
	}

	///@internal
	function _load() {
		var source = this.source.replaceAll(/[.]qml/g, '') //by Artur, remove .qml

		if (!source){
			//this.item = null
			this.__properties.item.value = null

			return
		}

		let pathArray = this.getComponentPath().split(' → ')
		let lpath = []
		for(let pa of pathArray){
			let cn = pa.split('.')
			cn = cn.slice(1, cn.length-1)
			if(cn.length) lpath.push(...cn)
		}
		
		let find = true
		let pos = 0
		let temp = _globals.src

		while(find && pos < lpath.length){
			if(temp[lpath[pos]]){
				temp = temp[lpath[pos]]
			} else {
				find = false
			}
			pos++
		}
		
		let findKey = function(_obj, _key, _path = []){
			if(_key in _obj) {
				let currentPath = _path.slice()
				currentPath.push(_key)
				lpath = currentPath
				find = true
			} else {
				for(let key in _obj){
					if(!find){
						let currentPath = _path.slice()
						currentPath.push(key)
						findKey(_obj[key], _key, currentPath)
					}	
				}
			}
		}

		if(!find)
		findKey(_globals.src, lpath[lpath.length-1])

		source = ('src/' + lpath.join('/') + '/' + source).replaceAll(/[/]{2,}/g, '/')
		let repl = true
		while(repl){
			let temp = source.replaceAll(/[/]\w*[/][.][.]/g, '')
			if(temp === source) repl = false
			source = temp
		}

		if(this._oldSource === source) return
		this._oldSource = source
		

		if (this.trace)
			log('loading ' + source + '…')
		var path = source.split('/') //by Artur, change . for /
		var ctor = _globals
		while(path.length) {
			var ns = path.shift()
			ctor = ctor[ns]
			if (ctor === undefined)
				throw new Error('unknown component used: ' + source)
		}

		var item = new ctor(this)
		var self = this
		var overrideComplete = oldComplete !== $core.CoreObject.prototype.__complete
		item.__finished = false
		if (overrideComplete) {
			var oldComplete = item.__complete
			var itemCompleted = this.itemCompleted.bind(this, item)
			item.__complete = function() {
				try {
					oldComplete.call(this)
				} catch(ex) {
					log("onComplete failed:", ex)
				}
				itemCompleted()
				this.__finished = true
			}
		}

		$core.core.createObject(item)
		this.loaded(item)

		if (!overrideComplete)
			this.itemCompleted()
			this.__finished = true

		this.__properties.item.value = item
		
		this.item._context._processActions()

		
	}

	onItemCompleted: {
		let tempTimer = setInterval(()=>{
			if(this.__properties.item && this.__properties.item.value && this.__properties.item.value.__finished === true){
				this.width = this.__properties.item.value.width
				this.height = this.__properties.item.value.height
				for(let func of this.__properties.item.onChanged){
					func(this.__properties.item.value)
				}
				clearInterval(tempTimer)
			}
		},10)
	}

	onRecursiveVisibleChanged: {
		if (this.item)
			this._updateVisibilityForChild(this.item, value)
	}

	///@internal
	onCompleted: {
		if (this.source === '') {
			this.item.discard()
			this.__properties.item.value = null
		}
		this.__ready = true
		this.discardItem()
		this._load()
	}
}
