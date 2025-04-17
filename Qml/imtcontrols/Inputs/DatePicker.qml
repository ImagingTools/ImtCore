import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Item {
	id: datePicker;

	height: 20;
	width: content.width;

	property var monthNames:
		[
		qsTr("January"),
		qsTr("February"),
		qsTr("March"),
		qsTr("April"),
		qsTr("May"),
		qsTr("June"),
		qsTr("July"),
		qsTr("August"),
		qsTr("September"),
		qsTr("October"),
		qsTr("November"),
		qsTr("December")
	];

	property int startYear: 2010;
	property int lastYear: 2040;

	property int todayYear;
	property int todayMonth;

	property int selectedIndexMonth;

	property alias selectedYear: yearField.text;
	property alias selectedMonth: monthField.text;
	property alias selectedDay: dayField.text;

	property alias currentDayButtonVisible: currentDate.visible;
	property alias defaultWidth: mainRow.width;
	property alias moveToEndYear : yearComboObj.moveToEnd;
	property alias moveToIndexYear : yearComboObj.moveToIndex;

	property bool completed: false;

	property bool startWithCurrentDay: true;
	property bool startWithFirstDay: false;
	property bool hasDay: true;

	property bool hasDayCombo: true;
	property bool hasMonthCombo: true;
	property bool hasYearCombo: true;
	property bool hasButtons: true;
	property int shownItemsCountCombo: 12;
	property alias monthCombo: monthComboObj;
	property alias yearCombo: yearComboObj;
	property alias dayCombo: dayComboObj;

	property bool openST: monthComboObj.openST || yearComboObj.openST || dayComboObj.openST;
	property bool endComboOnCurrentMonth: false;
	property bool readOnly: false;
	property bool textInputBan: false;

	property alias contentWidth: content.width;

	// icons
	property int iconWidth: 12;
	property int iconHeight: 10;
	property string iconUpSource: readOnly ? "../../../" + Style.getIconPath("Icons/Up", Icon.State.Off, Icon.Mode.Disabled)
										   : "../../../" + Style.getIconPath("Icons/Up", Icon.State.On, Icon.Mode.Normal);
	property string iconDownSource: readOnly ? "../../../" + Style.getIconPath("Icons/Down", Icon.State.Off, Icon.Mode.Disabled)
											 : "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal);
	// margins
	property int mainMargin: 0;
	// CustomTextField
	property int textFieldHeight: 20;
	property int textFieldWidthYear: 42;
	property int textFieldWidthMonth: 85;
	property int textFieldWidthDay: 25;
	property int textFieldRadius:0;
	property int textSize: Style.fontSizeNormal;
	property string textFieldColor: Style.baseColor;
	property string fontColor: Style.textColor;
	property string textFieldBorderColor: "";//textField.acceptableInput ? Style.iconColorOnSelected : Style.errorTextColor;
	property int textFieldMargin: 5;
	property int horizontalAlignment: TextInput.AlignHCenter;
	// buttons
	property int buttonWidth: 15;
	property int buttonHeight: 10;

	property string buttonColor:"";
	property int buttonHighlighted: -1;

	property int spinBoxSpacing: 0;
	//

	signal dateChanged();

	Component.onCompleted: {
		var date_ = new Date();

		datePicker.selectedIndexMonth = date_.getMonth();
		datePicker.todayMonth = date_.getMonth();
		datePicker.todayYear = date_.getFullYear();

		if (datePicker.startWithCurrentDay){
			let currentDate = new Date();
			yearField.text = currentDate.getFullYear();
			selectedIndexMonth = currentDate.getMonth();
			dayField.text = currentDate.getDate();
		}
		if(datePicker.startWithFirstDay){
			let currentDate = new Date();
			yearField.text = currentDate.getFullYear();
			selectedIndexMonth = currentDate.getMonth();
			dayField.text = "1";

		}

		if(datePicker.hasMonthCombo){
			let monthCount = datePicker.monthNames.length;

			if(datePicker.endComboOnCurrentMonth){
				monthCount = date_.getMonth() + 1;
			}

			for(var i = 0; i < monthCount; i++){
				var index = monthTreeModel.insertNewItem();
				monthTreeModel.setData("id", index, index);
				monthTreeModel.setData("name",qsTr(datePicker.monthNames[i]),index);
			}
		}

		if(datePicker.hasYearCombo && !yearComboObj.model.getItemsCount()){
			if(datePicker.startYear < 1 || datePicker.lastYear < 1){
				datePicker.startYear = 1900;
				datePicker.lastYear = 2100;
			}
			if(datePicker.startYear > datePicker.todayYear || datePicker.lastYear < datePicker.todayYear){
				datePicker.startYear = 1900;
				datePicker.lastYear = 2100;
			}

			if(datePicker.startYear > datePicker.lastYear){
				var temp = datePicker.startYear;
				datePicker.startYear = datePicker.lastYear;
				datePicker.lastYear = temp;
			}
			for(let i = datePicker.startYear; i <= datePicker.lastYear;i++){
				let index = yearTreeModel.insertNewItem();
				yearTreeModel.setData("id", index, index);
				yearTreeModel.setData("name", String(i),index);
			}

		}

		if(datePicker.hasDayCombo && datePicker.selectedYear !== ""){
			datePicker.fillDayModel();

		}

		datePicker.completed = true;
	}

	onSelectedYearChanged: {
		console.log("onSelectedYearChanged");
		if(datePicker.hasMonthCombo && datePicker.endComboOnCurrentMonth && datePicker.completed){
			datePicker.fillMonthModel();
		}
		if(datePicker.hasDayCombo && datePicker.completed){
			datePicker.fillDayModel();
		}
		if (datePicker.completed){
			datePicker.dateChanged();
		}
	}

	onSelectedDayChanged: {
		if (datePicker.completed){
			datePicker.dateChanged();
		}
	}

	onSelectedIndexMonthChanged: {
		if (datePicker.completed){
			datePicker.dateChanged();
		}
		if(datePicker.hasDayCombo && datePicker.completed){
			datePicker.fillDayModel();
		}
	}

	function getDate(){
		let date = new Date(datePicker.selectedYear, datePicker.selectedIndexMonth, Number(datePicker.selectedDay));
		let ok1 = date.getFullYear() == datePicker.selectedYear &&
			date.getMonth() == datePicker.selectedIndexMonth &&
			date.getDate() == Number(datePicker.selectedDay);

		if (!ok1){
			return null;
		}

		return new Date(datePicker.selectedYear, datePicker.selectedIndexMonth, Number(datePicker.selectedDay))
	}

	function getDateAsString(){
		let monthStr = String(datePicker.selectedIndexMonth + 1)
		if (monthStr.length == 1){
			monthStr = "0" + monthStr;
		}

		let dayStr = String(datePicker.selectedDay)
		if (dayStr.length == 1){
			dayStr = "0" + dayStr;
		}

		return datePicker.selectedYear + "-" + monthStr + "-" + dayStr;
	}

	function setCurrentDay(){
		let date = new Date();
		datePicker.setDate(date.getFullYear(), date.getMonth(), date.getDate());
	}

	function checkDate(year, month, day){
		let date = new Date(year, month, day);

		return date.getFullYear() == year &&
				date.getMonth() == month &&
				date.getDate() == day;
	}

	function setDate(year, month, day){
		if (datePicker.checkDate(year, month, day)){
			yearField.text = year;
			datePicker.selectedIndexMonth = month;
			dayField.text = day;

			if(datePicker.hasMonthCombo){
				monthComboObj.currentIndex = month;
			}
			if(datePicker.hasYearCombo){
				for(let i = 0; i <  yearTreeModel.getItemsCount(); i++){
					let id = yearTreeModel.getData("id");
					let name = yearTreeModel.getData("name");
					if(name == year){
						yearComboObj.currentIndex = id;
						break;
					}
				}
			}
			if(datePicker.hasDayCombo){
				for(let i = 0; i <  dayTreeModel.getItemsCount(); i++){
					let id = dayTreeModel.getData("id");
					let name = dayTreeModel.getData("name");
					if(name == day){
						dayComboObj.currentIndex = id;
						break;
					}
				}
			}
		}
	}

	function setDateFromString(stringDate){
		var year = stringDate.slice(0,4);
		var month = stringDate.slice(5,7);
		var day = stringDate.slice(8);
		if(day[0] == "0"){
			day = day.slice(1);
		}
		if(month[0] == "0"){
			month = month.slice(1);
		}

		month = Number(month) - 1;

		setDate(year, month, day);
	}

	function fillDayModel(){
		dayTreeModel.clear();
		var lastDay = datePicker.getLastDayOfMonth(datePicker.selectedIndexMonth + 1);
		for(var i = 1; i <= lastDay; i++){
			var index = dayTreeModel.insertNewItem();
			dayTreeModel.setData("id", i, index);
			dayTreeModel.setData("name",i,index);
		}
	}

	function fillMonthModel(){
		monthTreeModel.clear();

		let monthCount = datePicker.monthNames.length;
		let monthComboIndexPrev = datePicker.selectedIndexMonth;

		if(Number(datePicker.selectedYear) === datePicker.todayYear){
			monthCount = Number(datePicker.todayMonth) + 1;
		}

		for(var i = 0; i < monthCount; i++){
			var index = monthTreeModel.insertNewItem();
			monthTreeModel.setData("id", index, index);
			monthTreeModel.setData("name",qsTr(datePicker.monthNames[i]),index);
		}

		if(monthComboIndexPrev > monthTreeModel.getItemsCount() - 1){
			monthComboIndexPrev = 0;
		}

		monthComboObj.currentIndex = monthComboIndexPrev;
	}

	function getLastDayOfMonth(month){
		if(month < 1 || month > 12){
			console.log("getLastDayOfMonth(): invalid parameters (month < 1 || month > 12)")

			return ("")
		}

		var lastDay;
		switch (month)
		{
		case 2: lastDay = 28
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			lastDay = 30
			break
		default:
			lastDay = 31
			break
		}

		if(month === 2){
			var isLeapYear = datePicker.isLeapYearFun(Number(datePicker.selectedYear));

			if(isLeapYear){
				lastDay = 29;
			}
		}

		return (lastDay);
	}//

	function isLeapYearFun(year){
		var isLeapYear;

		if(!(year % 400)){
			isLeapYear = true;
		}
		else if(!(year % 100)){
			isLeapYear = false;
		}
		else if(!(year % 4)){
			isLeapYear = true;
		}
		else{
			isLeapYear = false;
		}

		return isLeapYear;
	}

	TreeItemModel {
		id: yearTreeModel;
	}

	TreeItemModel {
		id: monthTreeModel;
	}

	TreeItemModel {
		id: dayTreeModel;
	}

	Row {
		id: content;

		height: datePicker.height;

		spacing: Style.sizeMainMargin;

		RegularExpressionValidator {
			id: yearValid;

			regularExpression: /^(19|20)\d{2}$/;
		}

		Row {
			id: mainRow;
			height: parent.height;
			spacing: datePicker.mainMargin;

			Item{
				id: yearItem;

				anchors.verticalCenter: parent.verticalCenter;

				width: yearField.width + yearButtons.width + datePicker.spinBoxSpacing;
				height: datePicker.textFieldHeight;

				MouseArea {
					id: yearMouseArea;

					anchors.fill: yearField;

					enabled: !datePicker.readOnly;

					propagateComposedEvents: true;

					onWheel: {
						if (datePicker.readOnly){
							return;
						}

						if(wheel.angleDelta.y > 0){
							yearUpButton.clicked();
						}
						else {
							yearDownButton.clicked();
						}
					}
				}

				CustomTextField {
					id: yearField;

					anchors.verticalCenter: parent.verticalCenter;

					width: datePicker.textFieldWidthYear;
					height: datePicker.textFieldHeight;
					visible: !yearComboObj.openST;

					radius: datePicker.textFieldRadius;
					textSize: datePicker.textSize;
					fontColor: datePicker.fontColor;
					color: datePicker.textFieldColor;
					//                    borderColorConst: datePicker.textFieldBorderColor !== "" ? datePicker.textFieldBorderColor : acceptableInput ? Style.iconColorOnSelected : Style.errorTextColor;
					margin: datePicker.textFieldMargin;

					leftPadding: horizontalAlignment == TextInput.AlignLeft ? 4 : 0;
					horizontalAlignment: datePicker.horizontalAlignment;

					textInputValidator: yearValid;

					Keys.onUpPressed: {
						yearUpButton.clicked();
					}

					Keys.onDownPressed: {
						yearDownButton.clicked();
					}

					readOnly: true//datePicker.readOnly || datePicker.textInputBan;
				}

				Column {
					id: yearButtons;

					anchors.verticalCenter: parent.verticalCenter;
					anchors.left: yearField.right;
					anchors.leftMargin: visible ? datePicker.spinBoxSpacing : 0;

					width: visible ? datePicker.buttonWidth : 1;

					visible: datePicker.hasButtons;

					Button {
						id: yearUpButton;

						width: datePicker.buttonWidth;
						height: datePicker.buttonHeight;

						iconSource: datePicker.iconUpSource;

						enabled: !datePicker.readOnly;

						decorator: Component { ButtonDecorator {
								color: yearUpButton.hovered ? Style.buttonHoverColor : "transparent";
								border.width: 0;
							}}

						onClicked: {
							if (!enabled){
								return;
							}

							let selectedYear = Number(yearField.text);

							let newYear = String(selectedYear + 1);

							if (datePicker.checkDate(newYear, datePicker.selectedIndexMonth, Number(dayField.text))){
								yearField.text = newYear;
							}
						}
					}

					Button {
						id: yearDownButton;

						width: datePicker.buttonWidth;
						height: datePicker.buttonHeight;

						iconSource: datePicker.iconDownSource;

						enabled: !datePicker.readOnly;

						decorator: Component { ButtonDecorator {
								color: yearDownButton.hovered ? Style.buttonHoverColor : "transparent";
								border.width: 0;
							}}

						onClicked: {
							if (!enabled){
								return;
							}

							if (yearField.text == ""){
								return;
							}

							let selectedYear = Number(yearField.text);

							let newYear = String(selectedYear - 1);

							if (datePicker.checkDate(newYear, datePicker.selectedIndexMonth, Number(dayField.text))){
								yearField.text = newYear;
							}
						}
					}
				}

				ComboBox {
					id: yearComboObj;

					anchors.fill: yearField;

					visible: datePicker.hasYearCombo;
					enabled: visible;
					model: yearTreeModel;

					textSize: datePicker.textSize;
					fontColor: yearField.fontColor;
					borderColor: "transparent";
					backgroundColor: "transparent";

					fontColorTitle: "transparent";
					textCentered: true;
					shownItemsCount: datePicker.shownItemsCountCombo;
					moveToIndex : datePicker.todayYear - datePicker.startYear - datePicker.shownItemsCountCombo +1;
					visibleScrollBar: false;
					isColor: true;

					changeable: !datePicker.readOnly;
					//z: yearField.readOnly ? 1 : 0;

					onCurrentIndexChanged:{
						if(yearComboObj.currentIndex >=0){
							let selectedYear = yearComboObj.currentText;

							if (datePicker.checkDate(selectedYear, datePicker.selectedIndexMonth, Number(dayField.text))){
								yearField.text = selectedYear;
							}
							else{
								yearComboObj.currentIndex = -1;
								monthComboObj.currentIndex = -1;
								dayComboObj.currentIndex = -1;
							}
						}

					}
				}////yearComboObj

			}//yearItem

			Item{
				id: monthItem;
				anchors.verticalCenter: parent.verticalCenter;

				width: monthField.width + monthButtons.width + datePicker.spinBoxSpacing;
				height: datePicker.textFieldHeight;

				MouseArea {
					id: monthMouseArea;

					anchors.fill: monthField;

					enabled: !datePicker.readOnly;

					propagateComposedEvents: true;

					onWheel: {
						if (datePicker.readOnly){
							return;
						}

						if(wheel.angleDelta.y > 0){
							monthUpButton.clicked();
						}
						else {
							monthDownButton.clicked();
						}
					}
				}

				CustomTextField {
					id: monthField;

					anchors.verticalCenter: parent.verticalCenter;

					width: datePicker.textFieldWidthMonth;
					height: parent.height;

					visible: !monthComboObj.openST;

					radius: datePicker.textFieldRadius;
					textSize: datePicker.textSize;
					fontColor: datePicker.fontColor;
					color: datePicker.textFieldColor;
					margin: datePicker.textFieldMargin;

					readOnly: true//datePicker.readOnly || datePicker.textInputBan;

					leftPadding: horizontalAlignment == TextInput.AlignLeft ? 4 : 0;
					horizontalAlignment: datePicker.horizontalAlignment;

					text: datePicker.monthNames[datePicker.selectedIndexMonth];

					Keys.onUpPressed: {
						monthUpButton.clicked();
					}

					Keys.onDownPressed: {
						monthDownButton.clicked();
					}
				}

				Column {
					id: monthButtons;

					anchors.verticalCenter: parent.verticalCenter;
					anchors.left: monthField.right;
					anchors.leftMargin: visible ? datePicker.spinBoxSpacing : 0;

					width: visible ? datePicker.buttonWidth : 1;

					visible: datePicker.hasButtons;

					Button {
						id: monthUpButton;

						width: datePicker.buttonWidth;
						height: datePicker.buttonHeight;

						iconSource: datePicker.iconUpSource;

						enabled: !datePicker.readOnly;

						decorator: Component { ButtonDecorator {
								color: monthUpButton.hovered ? Style.buttonHoverColor : "transparent";
								border.width: 0;
							}}

						onClicked: {
							if (!enabled){
								return;
							}
							let index;
							if (datePicker.selectedIndexMonth < 11){
								index = datePicker.selectedIndexMonth + 1;
							}
							else{
								index = 0;
							}

							if (datePicker.checkDate(Number(yearField.text), index, Number(dayField.text))){
								datePicker.selectedIndexMonth = index;
							}
						}
					}

					Button {
						id: monthDownButton;

						width: datePicker.buttonWidth;
						height: datePicker.buttonHeight;

						iconSource: datePicker.iconDownSource;

						enabled: !datePicker.readOnly;

						decorator: Component { ButtonDecorator {
								color: monthDownButton.hovered ? Style.buttonHoverColor : "transparent";
								border.width: 0;
							}}

						onClicked: {
							if (!enabled){
								return;
							}

							let index;
							if (datePicker.selectedIndexMonth > 0){
								index = datePicker.selectedIndexMonth - 1;
							}
							else{
								index = 11;
							}

							if (datePicker.checkDate(Number(yearField.text), index, Number(dayField.text))){
								datePicker.selectedIndexMonth = index;
							}
						}
					}
				}

				ComboBox {
					id: monthComboObj;

					anchors.fill: monthField;

					visible: datePicker.hasMonthCombo;
					enabled: visible;
					model: monthTreeModel;

					textSize: datePicker.textSize;//Style.fontSizeNormal;
					fontColor: monthField.fontColor;
					borderColor: "transparent";
					backgroundColor: "transparent";

					fontColorTitle: "transparent";
					textCentered: true;
					shownItemsCount: 12;

					visibleScrollBar: false;
					isColor: true;

					changeable: !datePicker.readOnly;

					onCurrentIndexChanged:{
						if(monthComboObj.currentIndex >= 0){
							if (datePicker.checkDate(Number(yearField.text), monthComboObj.currentIndex, Number(dayField.text))){
								datePicker.selectedIndexMonth = monthComboObj.currentIndex;
							}
							else{
								yearComboObj.currentIndex = -1;
								monthComboObj.currentIndex = -1;
								dayComboObj.currentIndex = -1;
							}
						}
					}
				}//monthComboObj

			}//monthItem

			RegularExpressionValidator {
				id: dayValid;

				regularExpression: /\b([1-9]|[12][0-9]|3[01])\b/;
			}

			Item {
				id: dayItem;

				anchors.verticalCenter: parent.verticalCenter;

				visible: datePicker.hasDay;

				width: dayField.width + dayButtons.width + datePicker.spinBoxSpacing;
				height: datePicker.textFieldHeight;

				MouseArea {
					id: daysMouseArea;

					anchors.fill: dayField;

					enabled: !datePicker.readOnly;

					propagateComposedEvents: true;

					onWheel: {
						if(wheel.angleDelta.y > 0){
							dayUpButton.clicked();
						}
						else {
							dayDownButton.clicked();
						}
					}
				}

				CustomTextField {
					id: dayField;

					anchors.verticalCenter: parent.verticalCenter;

					visible: datePicker.hasDay && !dayComboObj.openST;

					width: datePicker.textFieldWidthDay;
					height: parent.height;

					radius: datePicker.textFieldRadius;
					textSize: datePicker.textSize;
					fontColor: datePicker.fontColor;
					color: datePicker.textFieldColor;
					margin: datePicker.textFieldMargin;

					readOnly: true;

					leftPadding: horizontalAlignment == TextInput.AlignLeft ? 4 : 0;
					horizontalAlignment: datePicker.horizontalAlignment;

					textInputValidator: dayValid;

					Keys.onUpPressed: {
						if (datePicker.readOnly){
							return;
						}

						dayUpButton.clicked();
					}

					Keys.onDownPressed: {
						if (datePicker.readOnly){
							return;
						}

						dayDownButton.clicked();
					}
				}

				Column {
					id: dayButtons;

					anchors.verticalCenter: parent.verticalCenter;
					anchors.left: dayField.right;
					anchors.leftMargin: visible ? datePicker.spinBoxSpacing : 0;

					width: visible ? datePicker.buttonWidth : 1;

					visible: datePicker.hasDay && datePicker.hasButtons;

					Button {
						id: dayUpButton;

						width: datePicker.buttonWidth;
						height: datePicker.buttonHeight;

						iconSource: datePicker.iconUpSource;
						enabled: !datePicker.readOnly;

						decorator: Component { ButtonDecorator {
								color: dayUpButton.hovered ? Style.buttonHoverColor : "transparent";
								border.width: 0;
							}}

						onClicked: {
							if (!enabled){
								return;
							}

							let selectedDay = Number(dayField.text);
							if (selectedDay < 31){
								let newDay = String(selectedDay + 1)

								if (datePicker.checkDate(Number(yearField.text), datePicker.selectedIndexMonth, Number(newDay))){
									dayField.text = newDay;
								}
							}
						}
					}

					Button {
						id: dayDownButton;

						width: datePicker.buttonWidth;
						height: datePicker.buttonHeight;

						iconSource: datePicker.iconDownSource;

						enabled: !datePicker.readOnly;

						decorator: Component { ButtonDecorator {
								color: dayDownButton.hovered ? Style.buttonHoverColor : "transparent";
								border.width: 0;
							}}

						onClicked: {
							if (!enabled){
								return;
							}

							let selectedDay = Number(dayField.text);
							if (selectedDay > 1){
								let newDay = String(selectedDay - 1);

								if (datePicker.checkDate(Number(yearField.text), datePicker.selectedIndexMonth, Number(newDay))){
									dayField.text = newDay;
								}
							}
						}
					}
				}

				ComboBox {
					id: dayComboObj;

					anchors.fill: dayField;

					visible: datePicker.hasDayCombo;
					enabled: visible;

					model: dayTreeModel;

					textSize: datePicker.textSize;
					fontColor: dayField.fontColor;
					borderColor: "transparent";
					backgroundColor: "transparent";

					fontColorTitle: "transparent";
					textCentered: true;
					shownItemsCount: datePicker.shownItemsCountCombo;

					visibleScrollBar: false;
					isColor: true;
					changeable: !datePicker.readOnly;

					onCurrentIndexChanged:{
						if(dayComboObj.currentIndex >=0){
							let selectedDay = dayComboObj.currentText;

							if (datePicker.checkDate(datePicker.selectedYear, datePicker.selectedIndexMonth, Number(selectedDay))){
								dayField.text = selectedDay;
							}
							else{
								yearComboObj.currentIndex = -1;
								monthComboObj.currentIndex = -1;
								dayComboObj.currentIndex = -1;
							}
						}
					}
				}//dayComboObj
			}//dayItem
		}

		Rectangle {
			id: currentDate;

			anchors.verticalCenter: parent.verticalCenter;

			width: 7;
			height: width;

			radius: width;

			color: Style.textColor;

			MouseArea {
				anchors.fill: parent;

				cursorShape: Qt.PointingHandCursor;

				onClicked: {
					datePicker.setCurrentDay();
				}
			}
		}
	}
}
