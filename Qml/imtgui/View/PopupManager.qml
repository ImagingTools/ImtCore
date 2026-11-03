pragma Singleton

import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0

QtObject {
	id: root;

	property PopupContainer popupContainer: null;

	// A message added with options.clickable was clicked
	signal messageClicked(string id);

	property Connections __containerConnections: Connections {
		target: root.popupContainer;

		function onMessageClicked(id){
			root.messageClicked(id);
		}
	}

	function registerPopupContainer(popup){
		popupContainer = popup;
	}

	// options (optional): {"messageDetails": second line, "messageIcon": icon instead of the one of the type,
	// "plainText": show the texts as they are, "clickable": the message emits messageClicked}
	function addErrorMessage(text, autoClose, id, options){
		if (popupContainer){
			popupContainer.addMessage("error", text, autoClose, id, options)
		}
	}

	function addWarningMessage(text, autoClose, id, options){
		if (popupContainer){
			popupContainer.addMessage("warning", text, autoClose, id, options)
		}
	}

	function addSuccessMessage(text, autoClose, id, options){
		if (popupContainer){
			popupContainer.addMessage("success", text, autoClose, id, options)
		}
	}

	function addInfoMessage(text, autoClose, id, options){
		if (popupContainer){
			popupContainer.addMessage("info", text, autoClose, id, options)
		}
	}

	function addCustomMessage(id, customComponent, properties){
		if (popupContainer){
			popupContainer.addCustomMessage(id, customComponent, properties)
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

	function replaceInfoMessage(id, text, autoClose){
		if (popupContainer){
			popupContainer.replaceMessage(id, "info", text, autoClose)
		}
	}

	function closeMessage(id){
		if (!popupContainer){
			return
		}

		popupContainer.removeMessageById(id)
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

	function clear(){
		if (popupContainer){
			popupContainer.clear()
		}
	}
}
