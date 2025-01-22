pragma Singleton

import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

QtObject {
	id: root;

	property PopupContainer popupContainer: null;

	function registerPopupContainer(popup){
		popupContainer = popup;
	}

	function addErrorMessage(text, autoClose){
		if (popupContainer){
			popupContainer.addMessage("error", text, autoClose)
		}
	}

	function addWarningMessage(text, autoClose){
		if (popupContainer){
			popupContainer.addMessage("warning", text, autoClose)
		}
	}

	function addSuccessMessage(text, autoClose){
		if (popupContainer){
			popupContainer.addMessage("success", text, autoClose)
		}
	}
}
