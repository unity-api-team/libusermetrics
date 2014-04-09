/*
 * Copyright (C) 2014 Canonical, Ltd.
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

#ifndef INFOGRAPHICSERVICE_SERVICE_H_
#define INFOGRAPHICSERVICE_SERVICE_H_

#include <QCryptographicHash>
#include <QDir>
#include <QDBusContext>
#include <QDBusConnection>
#include <QObject>
#include <QSet>
#include <QSharedPointer>
#include <QTimer>

class InfographicsAdaptor;

namespace InfographicService {

class Service: public QObject, protected QDBusContext {
Q_OBJECT

public:
	Service(const QDir &directory, const QDBusConnection &systemConnection);

	virtual ~Service();

public Q_SLOTS:
	void clear();

	void update(const QString &visualizer, const QStringList &sources,
			const QString &file);

protected Q_SLOTS:
	void timeout();

protected:
	virtual unsigned int uid();

	QDir userDirectory();

	QDir m_directory;

	QFile m_tempFile;

	QDBusConnection m_connection;

	QSharedPointer<InfographicsAdaptor> m_adaptor;

	QTimer m_dirtyTimer;

	QCryptographicHash m_hash;

	QSet<QPair<QString, QString>> m_dirty;
};

}

#endif /* INFOGRAPHICSERVICE_SERVICE_H_ */