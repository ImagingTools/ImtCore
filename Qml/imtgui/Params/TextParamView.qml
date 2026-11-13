import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

TextParamEditor {
	readOnly: true
	// read-only values like "1.0.0.11563 - Developer Version (03.11.2026 10:01)" are longer than an input
	controlWidth: Style.sizeHintM
}
