import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtbaseDocumentCollectionFilterSdl 1.0 as Doc

Doc.DocumentCollectionFilter {
	id: root

	readonly property string s_activeState: "ACTIVE"
	readonly property string s_inactiveState: "INACTIVE"
	readonly property string s_disabledState: "DISABLED"

	/*!
		\qmlsignal filterChanged
		Emitted when the filter is changed.
	*/
	signal filterChanged()

	function setDocumentId(documentId){
		m_documentId = documentId
	}

	function clear(beQuiet){
		m_documentId = ""
		m_documentStates = []
	}
	
	function addDocumentState(state){
		m_documentStates.push(state)
	}
	
	function containsState(state){
		return m_documentStates.includes(state)
	}

	function isEmpty(){
		return m_documentId === "" && m_documentStates.length === 0
	}
}


