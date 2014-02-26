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

#ifndef USERMETRICSSERVICE_SERVICEIMPL_H_
#define USERMETRICSSERVICE_SERVICEIMPL_H_

#include <usermetricsservice/Factory.h>

#include <QFileSystemWatcher>
#include <QDir>
#include <QMap>
#include <QMultiMap>
#include <QString>
#include <QTimer>

namespace UserMetricsService {

class ServiceImpl: public Service {
Q_OBJECT

public:
	ServiceImpl(const QDir &cacheDirectory, FileUtils::Ptr fileUtils,
			Factory &factory);

	virtual ~ServiceImpl();

protected Q_SLOTS:
	void updateInfographicList();

	void updateSourcesList();

	void sourceChanged(const QDir &directory, const QString &fileName);

	void flushQueue();

protected:
	FileUtils::Ptr m_fileUtils;

	Factory &m_factory;

	QTimer m_timer;

	QDir m_infographicDirectory;

	QFileSystemWatcher m_infographicWatcher;

	QDir m_sourcesDirectory;

	QFileSystemWatcher m_sourcesWatcher;

	QMap<QString, InfographicFile::Ptr> m_infographics;

	QMap<QString, SourceDirectory::Ptr> m_sources;

	QMultiMap<QString, QString> m_changedSources;
};

}

#endif /* USERMETRICSSERVICE_SERVICEIMPL_H_ */
