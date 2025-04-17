import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

TextParamEditor {
	onSourceItemChanged: {
		if (sourceItem){
			sourceItem.echoMode = TextInput.Password
		}
	}
}
