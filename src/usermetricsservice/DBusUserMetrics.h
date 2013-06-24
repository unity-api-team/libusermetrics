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

#ifndef USERMETRICSSERVICE_DBUSUSERMETRICS_H_
#define USERMETRICSSERVICE_DBUSUSERMETRICS_H_

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
#include <QtCore/QScopedPointer>
#include <QtCore/QSharedPointer>
#include <QtCore/QHash>

class UserMetricsAdaptor;

namespace UserMetricsService {

class DBusDataSource;

class DBusUserMetrics: public QObject {
Q_OBJECT

Q_PROPERTY(QList<QDBusObjectPath> dataSources READ dataSources)

public:
	DBusUserMetrics();

	virtual ~DBusUserMetrics();

	QList<QDBusObjectPath> dataSources() const;

	QDBusObjectPath createDataSource(const QString &name,
			const QString &formatString);

public Q_SLOTS:

Q_SIGNALS:
//	void LateEvent(const QString &eventkind);

protected:
	void syncDatabase();

	QScopedPointer<UserMetricsAdaptor> m_adaptor;

	QHash<QString, QSharedPointer<DBusDataSource>> m_dataSources;
};

}

#endif // USERMETRICSSERVICE_DBUSUSERMETRICS_H_
