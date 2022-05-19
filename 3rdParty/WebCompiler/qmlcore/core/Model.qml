Object {
	signal reset;			///< model reset signal
	signal rowsInserted;	///< rows inserted signal
	signal rowsChanged;		///< rows changed signal
	signal rowsRemoved;		///< rows removed signal
	
	property int count;		///< model rows count. Please note that you can't directly/indirectly modify model from onChanged handler. Use view.onCountChanged insteadzz

	function discard() {
		this.removeAllOnChanged()

		var attached = this.__attachedObjects
		this.__attachedObjects = []
		attached.forEach(function(child) { child.discard() })

		var parent = this.parent
		if (parent) {
			var discardIdx = parent.__attachedObjects.indexOf(this)
			if (discardIdx >= 0)
				parent.__attachedObjects.splice(discardIdx, 1)
		}

		this.children = []
		this._local = {}

		var properties = this.__properties
		for(var name in this.__properties) //fixme: it was added once, then removed, is it needed at all? it double-deletes callbacks
			if(!(name === 'count' && !this._rows.length)) this.__properties[name].discard()

		if(!(name === 'count' && !this._rows.length)) this.__properties = {'count': this.__properties['count']}; else this.__properties = {}

		var properties = this._properties
		for(var name in this._properties) //fixme: it was added once, then removed, is it needed at all? it double-deletes callbacks
			this._properties[name].discard()
		this._properties = {}

		$core.EventEmitter.prototype.discard.apply(this)

	}
}
