import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtbaseImtBaseTypesSdl 1.0

QtObject {
	id: container;

	property string typeId: "DesignSchema"
	property string currentDesignId
	
	property SelectionParam selectionParam: SelectionParam {
		onFinished: {
			if (m_selectedIndex < 0){
				return
			}

			let designId = m_constraints.m_options.get(m_selectedIndex).item.m_id
			
			container.setDesignSchema(designId)
		}
	}

	function setDesignSchema(designId){
		if (currentDesignId != designId){
			Style.getDesignScheme(designId);
			
			currentDesignId = designId
			
			Events.sendEvent("DesignSchemeChanged", currentDesignId);
		}
	}
}

