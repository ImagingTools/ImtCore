// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthype/IJobQueueManager.h>


namespace imthype
{


const QByteArray IJobQueueManager::CN_JOB_STATUS_CHANGED = QByteArrayLiteral("imthype:IJobQueueManager::JobStatusChanged");
const QByteArray IJobQueueManager::CN_JOB_PROGRESS_CHANGED = QByteArrayLiteral("imthype:IJobQueueManager::JobProgressChanged");
const QByteArray IJobQueueManager::CN_JOB_RESULT_CHANGED = QByteArrayLiteral("imthype:IJobQueueManager::JobResultChanged");


} // namespace imthype


