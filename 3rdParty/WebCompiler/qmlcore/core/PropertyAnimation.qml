TickAnimation {
    property int duration: 250;
    property variant from;
    property variant to;
    property string property: '';
    property string properties: '';
    property Object target;

    onPropertyChanged: {
        this.properties = this.property
    }

	function _tick() { 
        if(this.target && this.running){
            let properties = this.properties.split(',')

            for(let prop of properties){
                let increment = (this.to - this.from) / (this.duration / (1000/60))

                this.target[prop] += increment

                if(this.to >= this.from && this.target[prop] >= this.to) {
                    this.target[prop] = this.to
                    this.running = false
                    this.finished()
                }
                if(this.to <= this.from && this.target[prop] <= this.to) {
                    this.target[prop] = this.to
                    this.running = false
                    this.finished()
                }

            }

            this._context._processActions()
        }
        
    }

    function restart() { 
        let properties = this.properties.split(',')
        for(let prop of properties){
            this.target[prop] = this.from
        }
        this._context._processActions()
		this.pause = false
        this.running = true
    }

    
}