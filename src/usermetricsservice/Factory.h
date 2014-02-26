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

#ifndef USERMETRICSSERVICE_FACTORY_H_
#define USERMETRICSSERVICE_FACTORY_H_

#include <usermetricsservice/FileUtils.h>
#include <usermetricsservice/Infographic.h>
#include <usermetricsservice/InfographicFile.h>
#include <usermetricsservice/Service.h>
#include <usermetricsservice/SourceDirectory.h>

#include <QFile>
#include <QDir>

namespace UserMetricsService {

class Factory {
public:
	Factory();

	virtual ~Factory();

	virtual Service::Ptr singletonService();

	virtual FileUtils::Ptr singletonFileUtils();

	virtual InfographicFile::Ptr newInfographicFile(const QFile &path,
			const Service &service);

	virtual Infographic::Ptr newInfographic(const QVariant &config);

	virtual SourceDirectory::Ptr newSourceDirectory(const QDir &path);

protected:
	Service::Ptr m_service;

	FileUtils::Ptr m_fileUtils;
};

}

#endif /* USERMETRICSSERVICE_FACTORY_H_ */
