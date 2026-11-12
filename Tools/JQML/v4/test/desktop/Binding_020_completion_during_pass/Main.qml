import QtQuick 2.12

Item {
	id: root
	width: 792
	height: 192
	property int sourceValue: 1
	property int derivedValue: sourceValue * 2
	Loader {
		id: loader
		anchors.fill: parent
		active: false
		sourceComponent: Component {
			Item {
				id: page
				width: 792
				height: 192
				property bool compl: false
				property bool ready: page.compl && page.height > 0 && page.width > 0
				property int count: page.ready ? 3 : 0
				onReadyChanged: if (page.ready) console.log("fill")
				Component.onCompleted: {
					page.compl = true
					console.log("completed", page.ready, page.count, page.height, page.width)
					root.sourceValue = 7
					console.log("derived", root.derivedValue)
				}
			}
		}
	}
	Component.onCompleted: loader.active = Qt.binding(function() { return true })
	Timer {
		interval: 1
		running: true
		onTriggered: Qt.quit()
	}
}
