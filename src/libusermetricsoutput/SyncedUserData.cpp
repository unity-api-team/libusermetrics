/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of version 3 of the GNU Lesser General Public License as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Pete Woods <pete.woods@canonical.com>
 */

#include <libusermetricsoutput/SyncedDataSet.h>
#include <libusermetricsoutput/SyncedUserData.h>
#include <libusermetricsoutput/UserMetricsStore.h>
#include <libusermetricscommon/DataSetInterface.h>
#include <libusermetricscommon/DBusPaths.h>

using namespace com;
using namespace UserMetricsCommon;
using namespace UserMetricsOutput;

SyncedUserData::SyncedUserData(UserMetricsStore &userMetricsStore,
		QSharedPointer<com::canonical::usermetrics::UserData> interface,
		QObject *parent) :
		UserData(userMetricsStore, parent) {
	attachUserData(interface);
}

SyncedUserData::~SyncedUserData() {
}

void SyncedUserData::attachUserData(
		QSharedPointer<com::canonical::usermetrics::UserData> interface) {
	if (m_userDatas.contains(interface)) {
		return;
	}

	m_userDatas.insert(interface);

	connect(interface.data(),
	SIGNAL(dataSetAdded(const QDBusObjectPath &, const QDBusObjectPath &)),
			this,
			SLOT(addDataSet(const QDBusObjectPath &, const QDBusObjectPath &)));

	connect(interface.data(),
	SIGNAL(dataSetRemoved(const QDBusObjectPath &, const QDBusObjectPath &)),
			this,
			SLOT(removeDataSet(const QDBusObjectPath &, const QDBusObjectPath &)));

	for (const QDBusObjectPath &path : interface->dataSets()) {

		QSharedPointer<canonical::usermetrics::DataSet> dataSet(
				new canonical::usermetrics::DataSet(DBusPaths::serviceName(),
						path.path(), interface->connection()));

		QString dataSourcePath(dataSet->dataSource().path());
		insert(dataSourcePath,
				DataSetPtr(
						new SyncedDataSet(dataSet,
								m_userMetricsStore.dataSource(
										dataSourcePath))));
	}
}

void SyncedUserData::addDataSet(const QDBusObjectPath &dataSourcePath,
		const QDBusObjectPath &path) {
	QSharedPointer<canonical::usermetrics::DataSet> dataSet(
			new canonical::usermetrics::DataSet(DBusPaths::serviceName(),
					path.path(), (*m_userDatas.begin())->connection()));

	insert(dataSourcePath.path(),
			DataSetPtr(
					new SyncedDataSet(dataSet,
							m_userMetricsStore.dataSource(
									dataSourcePath.path()))));
}

void SyncedUserData::removeDataSet(const QDBusObjectPath &dataSourcePath,
		const QDBusObjectPath &path) {
	Q_UNUSED(path);
	m_dataSets.remove(dataSourcePath.path());
}
