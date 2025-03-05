import QtQuick 2.12
import Acf 1.0

QtObject {
	id: root;

	property TreeItemModel filterModel: TreeItemModel {}

	signal filterChanged();

	Component.onDestruction: {
		if (filterModel){
			filterModel.destroy();
		}
	}

	function getTextFilter(){
		if (filterModel.containsKey("TextFilter")){
			return filterModel.getData("TextFilter");
		}

		return "";
	}

	function setTextFilter(filter){
		filterModel.setData("TextFilter", filter);

		root.filterChanged();
	}

	function getSortingOrder(){
		if (filterModel.containsKey("Sort")){
			let sortModel = filterModel.getData("Sort");
			if (sortModel.containsKey("SortOrder")){
				return sortModel.getData("SortOrder")
			}
		}

		return "";
	}

	function setSortingOrder(sortingOrder){
		let sortModel = filterModel.getData("Sort");
		if (!sortModel){
			sortModel = filterModel.addTreeModel("Sort");
		}

		sortModel.setData("SortOrder", sortingOrder);

		root.filterChanged();
	}

	function getSortingInfoId(){
		if (filterModel.containsKey("Sort")){
			let sortModel = filterModel.getData("Sort");
			if (sortModel.containsKey("HeaderId")){
				return sortModel.getData("HeaderId")
			}
		}

		return "";
	}

	function setSortingInfoId(sortingInfoId){
		let sortModel = filterModel.getData("Sort");
		if (!sortModel){
			sortModel = filterModel.addTreeModel("Sort");
		}

		sortModel.setData("HeaderId", sortingInfoId);

		root.filterChanged();
	}

	function getFilteringInfoIds(){
		if (filterModel.containsKey("FilterIds")){
			return filterModel.getData("FilterIds");
		}

		return "";
	}

	function setFilteringInfoIds(filteringInfoIds){
		filterModel.setExternTreeModel("FilterIds", filteringInfoIds)

		root.filterChanged();
	}

	function setTimeFilter(timeFilterModel){
		filterModel.setExternTreeModel("TimeFilter", timeFilterModel.copyMe());

		root.filterChanged();
	}

	function addAdditionalFilter(key, value){
		let objectFilter = filterModel.getData("ObjectFilter");
		if (!objectFilter){
			objectFilter = filterModel.addTreeModel("ObjectFilter")
		}

		objectFilter.setData(key, value);

		root.filterChanged();
	}

	function removeFilterById(key){
		let objectFilter = filterModel.getData("ObjectFilter");
		if (objectFilter){
			if (objectFilter.containsKey(key)){
				objectFilter.removeData(key);

				root.filterChanged();
			}
		}
	}

	function clearAllFilters(){
		filterModel.removeData("TextFilter")
		filterModel.removeData("ObjectFilter")
		filterModel.removeData("TimeFilter")

		root.filterChanged();
	}

	function clearObjectFilter(){
		filterModel.removeData("ObjectFilter")

		root.filterChanged();
	}

	function clearTextFilter(){
		filterModel.removeData("TextFilter")

		root.filterChanged();
	}

	function clearTimeFilter(){
		filterModel.removeData("TimeFilter")

		root.filterChanged();
	}
}
