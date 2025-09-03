import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0


ComboBoxGql{
	id: searchContainer;

	width: Style.sizeHintM;
	height: Style.controlHeightM;
	itemHeight: Style.controlHeightL;
	shownItemsCount: 6

	textSize: Style.fontSizeS;
	fontColor: Style.textColor;
	radius: height/2

	placeHolderText: ""
	visibleIcon: !clearButton.visible
	keepFilterText: true

	onClearSignal:{
		reset()
	}

	function reset(){
		searchContainer.filterText = ""
		searchContainer.setTextFilter("")
		searchContainer.currentText = "";
		searchContainer.currentIndex = -1
	}

	Button{
		id: clearButton;

		anchors.verticalCenter: parent.verticalCenter;
		anchors.right: parent.right;
		anchors.rightMargin: Style.marginXS;

		width: height;
		height: !searchContainer.decorator_ ? 12 : searchContainer.decorator_.icon.height;
		z: parent.z + 1;

		decorator: Component{IconButtonDecorator{}}
		iconSource: "../../../" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);

		visible: searchContainer.currentText !== "" && !searchContainer.isOpen;
		enabled: visible;

		onClicked: {
			searchContainer.clearSignal();
		}
	}
}

