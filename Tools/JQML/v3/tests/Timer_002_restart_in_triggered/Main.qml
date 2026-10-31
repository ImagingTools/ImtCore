import QtQuick

// A single-shot timer started again in its own onTriggered runs again, as in Qt
Item {
	id: root

	property int count: 0

	Timer {
		id: timer

		interval: 10
		repeat: false
		onTriggered: {
			root.count++
			console.log("triggered " + root.count + " running " + timer.running)
			if (root.count < 3){
				timer.start()
			}
			else{
				Qt.quit()
			}
		}
	}

	Component.onCompleted: {
		timer.start()
	}
}
