import QtQuick

Item {
	function fail(text){
		throw new Error("failed " + text)
	}

	Component.onCompleted: {
		try {
			fail("here")
			console.log("not thrown")
		}
		catch (error){
			console.log("caught " + error.message)
		}
		Qt.quit()
	}
}
