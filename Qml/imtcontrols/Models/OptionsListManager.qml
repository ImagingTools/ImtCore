import QtQuick 2.12
import imtbaseImtBaseTypesSdl 1.0

QtObject {
	id: root

	property OptionsList optionsList: null
	property Component optionFactoryComp: Component{Option {}}

	function optionsListIsValid(){
		if (!optionsList || !optionsList.hasOptions()){
			return false
		}

		return true
	}

	function getOptionsCount(){
		if (!optionsListIsValid()){
			return 0
		}

		return optionsList.m_options.count
	}

	function getOptionIndexById(id){
		if (!optionsListIsValid()){
			return -1
		}

		for (let i = 0; i < optionsList.m_options.count; ++i){
			let optionId = optionsList.get(i).item.m_id
			if (optionId === id){
				return i
			}
		}

		return -1
	}

	function getOptionIndexByName(name){
		if (!optionsListIsValid()){
			return -1
		}

		for (let i = 0; i < optionsList.m_options.count; ++i){
			let optionName = optionsList.get(i).item.m_name
			if (optionName === name){
				return i
			}
		}

		return -1
	}

	function resetOptions(){
		if (!optionsListIsValid()){
			return
		}

		optionsList.m_options.clear()
	}

	function getOptionId(index){
		if (!optionsListIsValid()|| index < 0 || index >= optionsList.m_options.count){
			return ""
		}

		return optionsList.get(index).item.m_id
	}

	function getOptionName(index){
		if (!optionsListIsValid() || index < 0 || index >= optionsList.m_options.count){
			return ""
		}

		return optionsList.get(index).item.m_name
	}

	function getOptionDescription(index){
		if (!optionsListIsValid() || index < 0 || index >= optionsList.m_options.count){
			return ""
		}

		return optionsList.get(index).item.m_description
	}

	function isOptionEnabled(index){
		if (!optionsListIsValid() || index < 0 || index >= optionsList.m_options.count){
			return false
		}

		return optionsList.get(index).item.m_enabled
	}

	function insertOption(id, name, description, index){
		if (!optionsListIsValid()){
			return false
		}

		let optionObj = optionFactoryComp.createObject(optionsList)
		optionObj.m_id = id
		optionObj.m_name = name
		optionObj.m_description = description
		optionObj.m_enabled = true

		optionsList.m_options.insertElement(index, optionObj)

		return true
	}

	function removeOption(index){
		if (!optionsListIsValid()){
			return false
		}

		optionsList.m_options.removeElement(index)

		return true
	}

	function appendOption(id, name, description){
		if (!optionsListIsValid()){
			return false
		}

		let optionObj = optionFactoryComp.createObject(optionsList)
		optionObj.m_id = id
		optionObj.m_name = name
		optionObj.m_description = description
		optionObj.m_enabled = true

		optionsList.m_options.addElement(optionObj)

		return true
	}

	function setOptionName(index, name){
		if (!optionsListIsValid()){
			return false
		}

		optionsList.m_options.setProperty(index, "m_name", name)

		return true
	}

	function setOptionDescription(index, description){
		if (!optionsListIsValid()){
			return false
		}

		optionsList.m_options.setProperty(index, "m_description", description)

		return true
	}

	function setOptionEnabled(index, enabled){
		if (!optionsListIsValid()){
			return false
		}

		optionsList.m_options.setProperty(index, "m_enabled", enabled)

		return true
	}
}

