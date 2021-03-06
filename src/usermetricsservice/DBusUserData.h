/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Pete Woods <pete.woods@canonical.com>
 */

#ifndef USERMETRICSSERVICE_DBUSUSERDATA_H_
#define USERMETRICSSERVICE_DBUSUSERDATA_H_

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QScopedPointer>
#include <QtCore/QSharedPointer>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusContext>
#include <QtDBus/QDBusObjectPath>

class UserDataAdaptor;

namespace UserMetricsCommon {
class DateFactory;
}

namespace UserMetricsService {

class Authentication;
class UserData;
class DBusDataSet;
class DBusUserData;
class DBusUserMetrics;

typedef QSharedPointer<DBusUserData> DBusUserDataPtr;

class DBusUserData: public QObject, protected QDBusContext {
Q_OBJECT

Q_PROPERTY(QString username READ username)

Q_PROPERTY(QList<QDBusObjectPath> dataSets READ dataSets)

public:
	DBusUserData(int id, const QString &username, DBusUserMetrics &userMetrics,
			QDBusConnection &dbusConnection,
			QSharedPointer<UserMetricsCommon::DateFactory> dateFactory,
			QSharedPointer<Authentication> authentication, QObject *parent = 0);

	virtual ~DBusUserData();

	QString path() const;

	QString username() const;

	QList<QDBusObjectPath> dataSets() const;

	QDBusObjectPath createDataSet(const QString &dataSource);

	QSharedPointer<DBusDataSet> dataSet(const QString &dataSource) const;

protected:
	void syncDatabase();

	QDBusConnection m_dbusConnection;

	QScopedPointer<UserDataAdaptor> m_adaptor;

	QSharedPointer<UserMetricsCommon::DateFactory> m_dateFactory;

	QSharedPointer<Authentication> m_authentication;

	DBusUserMetrics &m_userMetrics;

	int m_id;

	QString m_path;

	QString m_username;

	QHash<int, QSharedPointer<DBusDataSet>> m_dataSets;
};

}

#endif // USERMETRICSSERVICE_DBUSUSERDATA_H_
