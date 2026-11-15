import QtQuick

// Loader.source/active is often assigned from a binding while an outer subscriber drain is
// already running. Component.onCompleted of the loaded tree then writes properties that other
// bindings (and onXChanged) depend on. Those updates must flush before the next script line —
// the same pattern TablePage uses: compl=true then tableDecorator=decorModel after setDefault.
Item {
	id: root
	property bool go: false
	property int result: -1

	Loader {
		id: loader
		active: root.go
		sourceComponent: Component {
			Item {
				id: page
				property bool a: false
				property bool b: false
				property bool ready: a && b
				property int filled: 0

				onReadyChanged: {
					if (ready)
						filled = 99
				}

				// Completes second (earlier sibling). After b=true, filled must already be 99.
				Item {
					Component.onCompleted: {
						page.b = true
						root.result = page.filled
					}
				}

				// Completes first (later sibling).
				Item {
					Component.onCompleted: page.a = true
				}
			}
		}
	}

	Component.onCompleted: {
		go = true
		console.log("result", root.result)
		Qt.quit()
	}
}
