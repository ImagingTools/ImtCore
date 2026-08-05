import QtQuick 2.12

QtObject {
	id: generator;

	property var sourceModel;

	// Отфильтрованная и отсортированная модель
	property var filteredModel;
	property var filterableFields: []
	property string textFilter: "";

	property var sourceIndexes: ({})

	signal accepted();

	Component.onDestruction: {
		if (filteredModel){
			filteredModel.destroy();
		}
	}

	onTextFilterChanged: {
		updateModel();
	}

	onSourceModelChanged: {
		if (sourceModel){
			filteredModel = sourceModel.copyMe();
		}
	}

	function updateModel(){
		if (!filteredModel){
			return;
		}

		filteredModel.clear()

		sourceIndexes = {}

		// Фильтрация
		let filtered = [];
		for (let i = 0; i < sourceModel.getItemsCount(); i++){
			let keys = sourceModel.getKeys(i);

			for (let j = 0; j < filterableFields.length; j++){
				let id = filterableFields[j];
				if (keys.includes(id)){
					let itemId = sourceModel.getData("id", i)
					let value = sourceModel.getData(id, i)
					if (value.toLowerCase().indexOf(textFilter.toLowerCase()) >= 0){
						let index = filteredModel.insertNewItem();
						filteredModel.copyItemDataFromModel(index, sourceModel, i);
						sourceIndexes[itemId] = i;

						break;
					}
				}
			}
		}
		filteredModel.refresh();

		accepted();
	}
}
