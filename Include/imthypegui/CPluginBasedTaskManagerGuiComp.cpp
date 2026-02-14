// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthypegui/CPluginBasedTaskManagerGuiComp.h>


// Qt includes
#include <QtCore/QUuid>

// Acula includes
#include <imthypegui/CTaskSettingsDialog.h>


namespace imthypegui
{


// public methods

CPluginBasedTaskManagerGuiComp::CPluginBasedTaskManagerGuiComp()
	:BaseClass(IMT_CREATE_PLUGIN_INSTANCE_FUNCTION_NAME(TaskEditor), IMT_DESTROY_PLUGIN_INSTANCE_FUNCTION_NAME(TaskEditor))
{
}


// protected methods

// reimplemented (TPluginManagerCompBase)

void CPluginBasedTaskManagerGuiComp::OnPluginsCreated()
{
	BaseClass::OnPluginsCreated();

	for (const PluginInfo & pluginInfo : m_plugins){
		if (pluginInfo.pluginPtr != nullptr){
			const istd::TIFactory<iqtgui::IGuiObject>* factoryPtr = pluginInfo.pluginPtr->GetTaskEditorFactory();
			if (factoryPtr != nullptr){
				istd::IFactoryInfo::KeyList keys = factoryPtr->GetFactoryKeys();
				for (const QByteArray& key : keys){
					Q_ASSERT(!key.isEmpty());

					if (!key.isEmpty()){
						m_pluginsMap[key] = pluginInfo.pluginPtr;
					}
				}
			}
		}
	}
}


// reimplemented (iqtgui::CGuiComponentBase)

void CPluginBasedTaskManagerGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	qsizetype lastTaskEditorIndex = m_typeToStackIndexMap.size();

	for (PluginsMap::ConstIterator pluginIter = m_pluginsMap.constBegin(); pluginIter != m_pluginsMap.constEnd(); ++pluginIter){
		const QByteArray& editorTypeId = pluginIter.key();
		if (m_editorsMap.contains(editorTypeId)){
			continue;
		}

		const istd::TIFactory<iqtgui::IGuiObject>* editorFactoryPtr = m_pluginsMap[editorTypeId]->GetTaskEditorFactory();
		Q_ASSERT(editorFactoryPtr != nullptr);

		istd::TUniqueInterfacePtr<iqtgui::IGuiObject> editorPtr(editorFactoryPtr->CreateInstance(editorTypeId));
		if (editorPtr.IsValid()){
			imod::IObserver* observerPtr = dynamic_cast<imod::IObserver*>(editorPtr.GetPtr());
			if (observerPtr != nullptr){
				istd::TDelPtr<QWidget> pageWidgetPtr(new QWidget(TaskEditorStack));
				QVBoxLayout* layoutPtr = new QVBoxLayout(pageWidgetPtr.GetPtr());
				layoutPtr->setContentsMargins(0, 0, 0, 0);

				if (editorPtr->CreateGui(pageWidgetPtr.GetPtr())){
					TaskEditorStack->addWidget(pageWidgetPtr.PopPtr());
					m_editorsMap[editorTypeId].FromUnique(editorPtr);
					m_observersMap[editorTypeId] = observerPtr;
					m_typeToStackIndexMap[editorTypeId] = ++lastTaskEditorIndex;
				}
			}
		}
	}
}


void CPluginBasedTaskManagerGuiComp::OnGuiDestroyed()
{
	BaseClass::OnGuiDestroyed();
}


void CPluginBasedTaskManagerGuiComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();
}


} // namespace imthypegui


