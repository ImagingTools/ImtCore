pragma Singleton

import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0

QtObject {
	id: root;

	property PopupContainer popupContainer: null;

	function registerPopupContainer(popup){
		popupContainer = popup;
	}

	function addErrorMessage(text, autoClose, id){
		if (popupContainer){
			popupContainer.addMessage("error", text, autoClose, id)
		}
	}

	function addWarningMessage(text, autoClose, id){
		if (popupContainer){
			popupContainer.addMessage("warning", text, autoClose, id)
		}
	}

	function addSuccessMessage(text, autoClose, id){
		if (popupContainer){
			popupContainer.addMessage("success", text, autoClose, id)
		}
	}

	function replaceErrorMessage(id, text, autoClose){
		if (popupContainer){
			popupContainer.replaceMessage(id, "error", text, autoClose)
		}
	}

	function replaceWarningMessage(id, text, autoClose){
		if (popupContainer){
			popupContainer.replaceMessage(id, "warning", text, autoClose)
		}
	}

	function replaceSuccessMessage(id, text, autoClose){
		if (popupContainer){
			popupContainer.replaceMessage(id, "success", text, autoClose)
		}
	}

	function closeMessage(id){
		if (!popupContainer){
			return
		}

		let index = getIndexByMessageId(id);
		if (index >= 0){
			popupContainer.removeMessage(index);
		}
	}

	function getIndexByMessageId(id){
		if (!id || id === "" || !popupContainer){
			return -1;
		}

		return popupContainer.findMessage(id);
	}

	function messageIsOpened(id){
		return getIndexByMessageId(id) >= 0;
	}
}
