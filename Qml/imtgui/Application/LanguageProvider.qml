import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtbaseImtBaseTypesSdl 1.0

QtObject {
	id: container;
	
	property string typeId: "Language";
	property string currentLanguage: context ? context.language : ""
	
	property SelectionParam selectionParam: SelectionParam {
		onFinished: {
			if (m_selectedIndex < 0){
				return
			}
			
			let languageId = m_constraints.m_options.get(m_selectedIndex).item.m_id
			
			container.setLanguage(languageId)
		}
	}

	function setLanguage(langId, beQuiet){
		if (!beQuiet){
			beQuiet = false
		}

		if (context.language === ""){
			beQuiet = true
		}
		
		if (context.language !== langId){
			context.language = langId;
			
			if (!beQuiet){
				Events.sendEvent("OnLocalizationChanged", langId);
			}
		}
	}
}


