import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

QtObject{
	id: fontObj;

	property string family: Style.fontFamily;
	property int pixelSize: Style.fontSizeM;
	property bool bold: false;
	property string color: "";
}


