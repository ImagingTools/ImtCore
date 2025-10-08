import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Item{
	id: menuButton;

	width: Style.sizeHintBXS
	height: Style.buttonHeightM

	property string text: "";
	property string iconSource: "";

	property TreeItemModel menuModel: TreeItemModel{}

	signal clicked(string buttonId);



}
