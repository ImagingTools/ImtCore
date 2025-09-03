import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0


SearchComp{
	id: searchContainer;

	width: Style.sizeHintM;
	height: Style.controlHeightM;
	itemHeight: Style.controlHeightL;
	shownItemsCount: 6

	textSize: Style.fontSizeS;
	fontColor: Style.textColor;
	radius: height/2

	placeHolderText: qsTr("Address")
	closeOnFinished: false
	keepFilterText: true

	property string parentIds: ""
	property string previousText: ""

	onFinished: {
		setAddressCurrentText(index)
	}

	onFilterTextChanged: {
		textChangeFunc(filterText)
	}

	onClearSignal:{
		searchContainer.parentIds = ""
		searchContainer.previousText = ""
	}

	/**
		Handles the close action of the search container.
		If the search container is not open and the current text is empty, it resets the excludeFilterPart.
		If the current text is not empty, it trims any leading or trailing commas and updates the filterText and currentText.
		It also adjusts the parentIds based on the number of elements in the currentText.
	*/
	onCloseAction: {
		if(searchContainer.isOpen){
			return
		}

		if(searchContainer.currentText == "" ){
			searchContainer.excludeFilterPart = "";

			return
		}

		let trimmedText = searchContainer.filterText.trim();
		if (trimmedText.startsWith(',') || trimmedText.endsWith(',')){
			trimmedText = trimmedText.replace(/^,|,$/g, '');
		}

		searchContainer.filterText = searchContainer.currentText = trimmedText

		let textArraySize		= searchContainer.arraySize(searchContainer.currentText);
		let parentIdsArraySize	= searchContainer.arraySize(searchContainer.parentIds);
		searchContainer.excludeFilterPart = searchContainer.removeLastElement(searchContainer.currentText);
		if(textArraySize === parentIdsArraySize){
			searchContainer.parentIds = searchContainer.removeLastElement(searchContainer.parentIds);
		}
	}

	function setCustomFilters(inputParams){
		inputParams.InsertField("parentIds", searchContainer.parentIds);
	}

	/**
		Sets the current text to the address at the specified index in the model, optionally appending additional text.
		Also updates the parentIds property based on the selected address's parent IDs.
		If the search container is open, it updates the excludeFilterPart and filterText to match current text .
		\param index - The index of the address in the model to set as current text.
		\param appendCurrText - Optional text to append to the current text (default is an empty string).
	*/
	function setAddressCurrentText(index, appendCurrText){
		let _appendCurrText = appendCurrText || "";

		let fullAdress = model.getData(displayId, index) || ""
		if(fullAdress === ""){
			return
		}

		let textChange = currentText !== fullAdress;
		searchContainer.currentText = fullAdress + _appendCurrText;

		let _parentIds = model.getData("parentIds", index) || "";
		if(_parentIds !== ""){
			_parentIds = _parentIds + ',';
		}

		let addressId = model.getData("id", index);
		searchContainer.parentIds = _parentIds + addressId;
		if(!searchContainer.isOpen){
			return
		}

		searchContainer.filterText = searchContainer.excludeFilterPart = currentText;
		if(textChange){
			searchContainer.textChangeFunc(currentText);
		}
	}

	/**
		Handles changes in the filter text.
		If the text is increased and ends with a comma, it checks if the new text matches the the first address in the list.
		If it matches, it appends a comma to the current text and sets it as a parent for further search of child elements.
		If the text is decreased, it either keeps N elements based on comma count or resets the excludeFilterPart and parentIds.
	*/
	function textChangeFunc(filterText){
		if(!searchContainer.isOpen || !filterText){
			return;
		}

		let isTextIncrease	= searchContainer.previousText.length < filterText.length;
		let elementCountPrev	= searchContainer.arraySize(searchContainer.previousText);
		let elementCountNew	= searchContainer.arraySize(filterText);
		let elementDeletedDiff = elementCountPrev - elementCountNew ;
		let elementDeleted		= elementDeletedDiff > 0;
		searchContainer.previousText = filterText;

		if(isTextIncrease){
			let modelCount = model.getItemsCount();
			if(!modelCount){
				return
			}

			let trimmedText = searchContainer.filterText.trim();
			if(trimmedText.endsWith(',')){
				let str = trimmedText.substring(0, trimmedText.length - 1);
				let strArrCount = searchContainer.arraySize(str);

				let newAddress = model.getData(searchContainer.displayId);
				str = str.replace(/ +/g, '');
				newAddress = newAddress.replace(/ +/g, '');
				if(str.toLowerCase() === newAddress.toLowerCase()){
					searchContainer.setAddressCurrentText(0, ',');
				}
			}
		}
		else if(elementCountNew <= elementCountPrev){
			if(elementCountPrev === elementCountNew && elementCountNew === 1){
				searchContainer.excludeFilterPart = "";
				searchContainer.parentIds		  = "";

				return;
			}

			let keepCount = elementCountNew;
			if(elementDeleted){
				keepCount = keepCount - elementDeletedDiff;
			}
			if(keepCount < 0){
				keepCount = 0;
			}

			if(elementDeleted){
				searchContainer.excludeFilterPart = searchContainer.keepNElements(filterText, keepCount);
				searchContainer.parentIds		  = searchContainer.keepNElements(searchContainer.parentIds, keepCount);

				let str = filterText.replace(excludeFilterPart, "");
				searchContainer.setTextFilter(str);
			}
		}
	}

	/**
		Sets an external address and its associated parent IDs.
		Updates the currentText, parentIds, and excludeFilterPart properties accordingly.
		\param address - The external address to set as current text.
		\param parentIds - The comma-separated string of parent IDs associated with the address.
	*/
	function setExternalAddress(address, parentIds){
		searchContainer.parentIds = parentIds;
		searchContainer.currentText = address
		searchContainer.excludeFilterPart = searchContainer.removeLastElement(currentText);
	}

	// helper methods

	/**
		Returns the number of elements in a comma-separated string.
		If the string is empty or null, returns 0.
	*/
	function arraySize(str){
		if(!str){
			return 0;
		}

		return str.split(',').length;
	}

	/**
		Removes the last element from a comma-separated string.
		If the string is empty or null, returns an empty string.
	*/
	function removeLastElement(str){
		let arr = str.split(',');
		arr.pop()

		return arr.join();
	}

	/**
		Returns a new string containing only the first \param 'elementCount' elements from a comma-separated string.
		If \param 'elementCount' is greater than the number of elements in the string, returns the original string.
		If the string is empty or null, or if \param 'elementCount' is less than or equal to 0, returns an empty string.
	*/
	function keepNElements(str, elementCount){
		if (!str || !elementCount || elementCount < 0){
			return "";
		}

		let parts = str.split(',');

		return parts.slice(0, elementCount).join(',');
	}
}

