/**
controls property animation behavior in declarative way
Animation class works only for integral types, please use ColorAnimation for animating color properties
*/
Object {
	property bool alwaysRunToEnd: false;
	property bool paused: false;
	property bool running: false;
	property int loops: 1;

	signal finished;
	signal started;
	signal stopped;

	constructor: { 
		this._context.backend.pushAnimation(this)
	}

	onAlwaysRunToEndChanged:{

	}
	onPausedChanged:{
		
	}
	onRunningChanged:{
		
	}
	onLoopsChanged:{
		
	}
	
	function complete() { }
	function pause() { 
        this.pause = true
    }
	function start() {
        this.restart()
        this.started()
    }
	function stop() {
		this.running = false
        this.stopped()
    }
	function resume() { 
        this.pause = false
    }
	function restart() { 
		this.pause = false
        this.running = true
    }

	function _tick() { }

}
