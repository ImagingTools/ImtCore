/// provides shadow properties adjustment
Item {
	property color color: "black";	
	property real spread: 0;			
	property var source: null;
	property var _oldSource: null;
	property real horizontalOffset: 0;
	property int samples: 9;
	property int radius: 4;
	property real verticalOffset: 0;

	onSourceChanged: {
		this._deleteOldShadow()
		this._oldSource = this.source
		this._updateShadow()
	}

	onColorChanged,
	onSpreadChanged,
	onRadiusChanged,
	onSamplesChanged,
	onVerticalOffsetChanged,
	onHorizontalOffsetChanged: {
		this._updateShadow()
	}

	function _deleteOldShadow(){
		if(this._oldSource)
		this._oldSource.style({
			'boxShadow': `unset`,
		})
	}

	function _updateShadow(){
		if(this.source)
		this.source.style({
			'boxShadow': `${this.horizontalOffset}px ${this.verticalOffset}px ${this.radius}px ${this.spread}px ${this.color}`,
		})
	}

	onCompleted: {
		this.style({
			'display': 'none',
		})
	}
}
