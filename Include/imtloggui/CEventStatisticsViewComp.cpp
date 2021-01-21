#include <imtloggui/CEventStatisticsViewComp.h>


// Acf includes
#include <iwidgets/iwidgets.h>


namespace imtloggui
{


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CEventStatisticsViewComp::UpdateGui(const istd::IChangeable::ChangeSet& changeSet)
{
	IEventStatisticsProvider* objectPtr = GetObservedObject();

	imtbase::ICollectionInfo::Ids ids = objectPtr->GetElementIds();

	// Synchronize groups
	if (changeSet.Contains(IEventStatisticsProvider::CF_GROUPS)){
		GroupList::iterator itGroups = m_groups.begin();
		while (itGroups != m_groups.end()){
			if (ids.contains(itGroups->id)){
				itGroups++;
			}
			else{
				Container->layout()->removeWidget(itGroups->widgetPtr);
				delete itGroups->widgetPtr;
				itGroups = m_groups.erase(itGroups);
			}
		}

		imtbase::ICollectionInfo::Ids::iterator itIds = ids.begin();
		while (itIds != ids.end()){
			bool isFound = false;

			itGroups = m_groups.begin();
			while (itGroups != m_groups.end()){
				if (itGroups->id == *itIds){
					isFound = true;
					break;
				}

				itGroups++;
			}

			if (!isFound){
				GroupItem group;
				group.id = *itIds;
				group.widgetPtr = new GroupStatisticsWidget(Container);
				m_groups.append(group);
			}

			itIds++;
		}

		for (int i = 0; i < ids.count(); i++){
			int sourceIndex = ids.count() - 1 - i;
			int index = GetGroupIndex(ids[sourceIndex]);
			Q_ASSERT(index >= 0);

			if (i != index){
				m_groups.move(index, i);
			}

			m_groups[i].widgetPtr->SetName(objectPtr->GetElementInfo(ids[sourceIndex], IEventStatisticsProvider::EIT_NAME).toString());
			m_groups[i].widgetPtr->setVisible(objectPtr->GetElementInfo(ids[sourceIndex], IEventStatisticsProvider::EIT_ENABLED).toBool());
		}

		QVBoxLayout* layoutPtr = dynamic_cast<QVBoxLayout*>(Container->layout());
		if (layoutPtr != nullptr){
			while (layoutPtr->count() != 0){
				layoutPtr->takeAt(0);
			}

			for (int i = 0; i < m_groups.count(); i++){
				layoutPtr->addWidget(m_groups[i].widgetPtr);
			}

			layoutPtr->addSpacerItem(m_spacerPtr);
		}
	}

	// Set group counters
	if (changeSet.Contains(IEventStatisticsProvider::CF_COUNTERS)){
		for (QByteArray id : ids){
			int index = GetGroupIndex(id);
			Q_ASSERT(index >= 0);

			m_groups[index].widgetPtr->SetCounters(
						objectPtr->GetCategoryCounter(id, istd::IInformationProvider::IC_NONE),
						objectPtr->GetCategoryCounter(id, istd::IInformationProvider::IC_INFO),
						objectPtr->GetCategoryCounter(id, istd::IInformationProvider::IC_WARNING),
						objectPtr->GetCategoryCounter(id, istd::IInformationProvider::IC_ERROR),
						objectPtr->GetCategoryCounter(id, istd::IInformationProvider::IC_CRITICAL));
		}
	}
}


void CEventStatisticsViewComp::OnGuiModelDetached()
{
	BaseClass::OnGuiModelDetached();
}


// reimplemented (iqtgui::CGuiComponentBase)

void CEventStatisticsViewComp::OnGuiCreated()
{
	m_spacerPtr = new QSpacerItem(10, 0, QSizePolicy::Preferred, QSizePolicy::Expanding);
		
	BaseClass::OnGuiCreated();

	if (Container->layout() == nullptr){
		iwidgets::ClearLayout(Container->layout());
		delete Container->layout();
	}

	QVBoxLayout* layoutPtr = new QVBoxLayout();
	layoutPtr->setContentsMargins(0, 0, 0, 0);

	Container->setLayout(layoutPtr);	
}


void CEventStatisticsViewComp::OnGuiDestroyed()
{
	while (Container->layout()->count()){
		Container->layout()->takeAt(0);
	}

	for (int i = 0; i < m_groups.count(); i++){
		delete m_groups[i].widgetPtr;
	}

	delete m_spacerPtr;

	BaseClass::OnGuiDestroyed();
}


void CEventStatisticsViewComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	if (IsModelAttached()){
		UpdateGui(istd::IChangeable::GetAnyChange());
	}
}


// private methods

int CEventStatisticsViewComp::GetGroupIndex(const QByteArray& id)
{
	for (int i = 0; i < m_groups.count(); i++){
		if (m_groups[i].id == id){
			return i;
		}
	}

	return -1;
}


// public methods of the embedded class GroupStatisticsWidget

CEventStatisticsViewComp::GroupStatisticsWidget::GroupStatisticsWidget(QWidget *parentPtr)
	:QWidget(parentPtr)
{
	setLayout(new QVBoxLayout(this));
	QVBoxLayout* layoutPtr = dynamic_cast<QVBoxLayout*>(layout());

	QFont groupNameFont;
	groupNameFont.setPixelSize(12);
	groupNameFont.setBold(true);

	m_name = new QLabel(this);
	m_name->setStyleSheet("color: #88b8e3");
	m_name->setFont(groupNameFont);

	layoutPtr->addWidget(m_name);

	for (int i = 0; i < 5; i++){
		QHBoxLayout* categoryLayoutPtr = new QHBoxLayout();

		QIcon icon;

		switch (i){
		case istd::IInformationProvider::IC_NONE:
			icon = QIcon(":/Icons/StateUnknown");
			break;
		case istd::IInformationProvider::IC_INFO:
			icon = QIcon(":/Icons/StateOk");
			break;
		case istd::IInformationProvider::IC_WARNING:
			icon = QIcon(":/Icons/StateWarning");
			break;
		case istd::IInformationProvider::IC_ERROR:
			icon = QIcon(":/Icons/Error");
			break;
		case istd::IInformationProvider::IC_CRITICAL:
			icon = QIcon(":/Icons/StateInvalid");
			break;
		}

		QPixmap pixmap = icon.pixmap(16);
		QLabel* iconPtr = new QLabel(this);
		iconPtr->setPixmap(pixmap);
		categoryLayoutPtr->addWidget(iconPtr, 1);

		m_counters[i] = new QLabel("0 ", this);

		QFont countLabelFont;
		countLabelFont.setPixelSize(9);

		m_counters[i]->setStyleSheet("color: gray");
		m_counters[i]->setFont(countLabelFont);
		m_counters[i]->setText(tr("%1 Events").arg(0));
		categoryLayoutPtr->addWidget(m_counters[i], 10000);

		layoutPtr->addLayout(categoryLayoutPtr);
	}
}


void CEventStatisticsViewComp::GroupStatisticsWidget::SetName(const QString& name)
{
	m_name->setText(name);
}


void CEventStatisticsViewComp::GroupStatisticsWidget::SetCounters(
			qint64 noneCouner,
			qint64 infoCouner,
			qint64 warningCouner,
			qint64 errorCouner,
			qint64 criticalCouner)
{
	m_counters[istd::IInformationProvider::IC_NONE]->setText(tr("%1 Events").arg(noneCouner));
	m_counters[istd::IInformationProvider::IC_INFO]->setText(tr("%1 Events").arg(infoCouner));
	m_counters[istd::IInformationProvider::IC_WARNING]->setText(tr("%1 Events").arg(warningCouner));
	m_counters[istd::IInformationProvider::IC_ERROR]->setText(tr("%1 Events").arg(errorCouner));
	m_counters[istd::IInformationProvider::IC_CRITICAL]->setText(tr("%1 Events").arg(criticalCouner));
}


} // namespace imtloggui


