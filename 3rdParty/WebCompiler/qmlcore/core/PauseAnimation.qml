TickAnimation {
    property int duration: 250;

    constructor: {
        this._duration = this.duration
    }

    onCompleted: {
        this._duration = this.duration
    }

    function _tick(){
        if(this.running && !this.paused && this._duration > 0){
            this._duration -= 1000/60

            if(this._duration < 0) {
                this.running = false
                this.finished()

                this._context._processActions()
            }
        }
    }

	function restart() { 
        this._duration = this.duration
        this.pause = false
        this.running = true
    }
}