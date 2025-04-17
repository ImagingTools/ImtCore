import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

// GuiComponentBase
ViewBase {
	id: root;

	property string documentId;
	property string documentTypeId;
	property string viewTypeId;
	property DocumentManager documentManager;
	property Component viewComp: null;
	property ViewBase view: contentLoader.item;

	signal viewRegistered();

	onModelChanged: {
		if (view){
			view.model = model;
		}
	}

	onViewRegistered: {
		console.debug("onViewRegistered", documentId, documentTypeId, viewTypeId, root);
	}

	function updateGui(){
		if (view){
			view.doUpdateGui();
		}
	}

	function updateModel(){
		if (view){
			view.doUpdateModel();
		}
	}

	Loader {
		id: contentLoader;
		sourceComponent: root.viewComp;
		onLoaded: {
			console.debug("View for document successfully created");
			item.model = root.model;
		}
	}
}
