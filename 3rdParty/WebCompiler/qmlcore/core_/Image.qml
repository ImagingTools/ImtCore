//item for image displaying
Item {
	property int paintedWidth;								///< actually painted image width
	property int paintedHeight;								///< actually painted image height
	property int sourceWidth; 								///< actual width of loaded image
	property int sourceHeight; 								///< actual height of loaded image
	property string source;									///< image URL
	property enum status { Null, Ready, Loading, Error };	///< image status
	property enum fillMode { Stretch, PreserveAspectFit, PreserveAspectCrop, Tile, TileVertically, TileHorizontally, Pad };	///< setup mode how image must fill it's content
	property enum verticalAlignment { AlignVCenter, AlignTop, AlignBottom };
	property enum horizontalAlignment { AlignHCenter, AlignLeft, AlignRight };
	property bool smooth: true;								///< if false, image will be pixelated
	property bool preload: false;							///< image will be loaded even if it's not visible

	width: sourceWidth;
	height: sourceHeight;

	///@private
	constructor: {
		this._context.backend.initImage(this)
		this._scheduleLoad()
	}

	function getClass() { return 'core-image' }

	///@private
	function _scheduleLoad() {
		if (this.preload || this.recursiveVisible)
			this._context.delayedAction('image.load', this, this._load)
	}

	///@private
	function _onError() {
		this.status = this.Error;
	}

	///@private
	function _load() {
		if (this.status === this.Ready) {
			this._updatePaintedSize()
			return
		}

		if (!this.preload && !this.recursiveVisible)
			return

		if (!this.source) {
			this._resetImage()
			return
		}

		this.status = this.Loading
		var ctx = this._context
		var callback = this._imageLoaded.bind(this)
		ctx.backend.loadImage(this, ctx.wrapNativeCallback(callback))
	}

	onPreloadChanged,
	onRecursiveVisibleChanged,
	onWidthChanged,
	onHeightChanged,
	onFillModeChanged: {
		this._scheduleLoad()
	}

	onSourceChanged: {
		this.status = this.Null


		let pathArray = this.getComponentPath().split(' → ')
		let path = []
		for(let pa of pathArray){
			let cn = pa.split('.')
			cn = cn.slice(1, cn.length-1)
			if(cn.length) path.push(...cn)
		}

		let find = true
		let pos = 0
		let temp = _globals.src

		while(find && pos < path.length){
			if(temp[path[pos]]){
				temp = temp[path[pos]]
			} else {
				find = false
			}
			pos++
		}
		
		let findKey = function(_obj, _key, _path = []){
			if(_key in _obj) {
				let currentPath = _path.slice()
				currentPath.push(_key)
				path = currentPath
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
		findKey(_globals.src, path[path.length-1])


		let source = (this._context.location.pathname.replaceAll(/\w+[.]html[/]*/g, '') + '/' + path.join('/') + '/' + this.__properties.source.value).replaceAll(/[/]{2,}/g, '/')
		let repl = true
		while(repl){
			let temp = source.replaceAll(/[/]\w*[/][.][.]/g, '')
			if(temp === source) repl = false
			source = temp
		}

		this.__properties.source.value = source
		

		this._scheduleLoad()
	}

	///@private
	function _resetImage() {
		this.style('background-image', '')
	}

	function _updatePaintedSize() {
		var natW = this.sourceWidth, natH = this.sourceHeight
		var w = this.width, h = this.height

		if (natW <= 0 || natH <= 0 || w <= 0 || h <= 0) {
			this.paintedWidth = 0
			this.paintedHeight = 0
			return
		}

		var crop
		switch(this.fillMode) {
			case ImageComponent.PreserveAspectFit:
				crop = false
				break
			case ImageComponent.PreserveAspectCrop:
				crop = true
				break
			default:
				this.paintedWidth = w
				this.paintedHeight = h
				return
		}

		var targetRatio = w / h, srcRatio = natW / natH

		var useWidth = crop? srcRatio < targetRatio: srcRatio > targetRatio
		if (useWidth) { // img width aligned with target width
			this.paintedWidth = w;
			this.paintedHeight = w / srcRatio;
		} else {
			this.paintedHeight = h;
			this.paintedWidth = h * srcRatio;
		}
	}

	///@private
	function _imageLoaded(metrics) {
		if (!metrics) {
			this.status = ImageComponent.Error
			return
		}

		var style = { 'background-image': 'url("' + this.source + '")' }

		var natW = metrics.width, natH = metrics.height
		this.sourceWidth = this.sourceWidth <= 0 ? natW : this.sourceWidth
		this.sourceHeight = this.sourceHeight <= 0 ? natH : this.sourceHeight

		switch(this.fillMode) {
			case ImageComponent.Stretch:
				style['background-repeat'] = 'no-repeat'
				style['background-size'] = `${this.sourceWidth}px ${this.sourceHeight}px`
				style['background-position'] = 'center'
				break;
			case ImageComponent.TileVertically:
				style['background-repeat'] = 'repeat-y'
				style['background-size'] = `${this.sourceWidth}px ${this.sourceHeight}px`
				style['background-position'] = 'auto'
				break;
			case ImageComponent.TileHorizontally:
				style['background-repeat'] = 'repeat-x'
				style['background-size'] = `${this.sourceWidth}px ${this.sourceHeight}px`
				style['background-position'] = 'auto'
				break;
			case ImageComponent.Tile:
				style['background-repeat'] = 'repeat'
				style['background-size'] = `${this.sourceWidth}px ${this.sourceHeight}px`
				style['background-position'] = 'top left'
				break;
			case ImageComponent.PreserveAspectCrop:
				style['background-repeat'] = 'no-repeat'
				style['background-size'] = 'cover'
				style['background-position'] = 'center'
				break;
			case ImageComponent.Pad:
				style['background-repeat'] = 'no-repeat'
				style['background-position'] = 'top left'
				style['background-size'] = 'auto'
				break;
			case ImageComponent.PreserveAspectFit:
				style['background-repeat'] = 'no-repeat'
				style['background-size'] = 'contain'
				style['background-position'] = 'center'
				break;
		}
		//style['image-rendering'] = this.smooth? 'auto': 'pixelated'
		this.style(style)

		this.status = ImageComponent.Ready
		this._updatePaintedSize()
	}
}
