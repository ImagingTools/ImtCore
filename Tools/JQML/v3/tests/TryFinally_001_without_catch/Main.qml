import QtQuick

Item {
	property string state: "idle"

	function run(fail){
		state = "running"
		try {
			if (fail){
				throw new Error("stopped")
			}
			console.log("body done")
		}
		finally {
			state = "idle"
		}
	}

	Component.onCompleted: {
		run(false)
		console.log("after success " + state)
		try {
			run(true)
		}
		catch (error){
			console.log("caught " + error.message + ", " + state)
		}
		Qt.quit()
	}
}
