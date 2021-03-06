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

#ifndef USERMETRICSOUTPUT_USERDATA_H_
#define USERMETRICSOUTPUT_USERDATA_H_

#include <libusermetricsoutput/DataSet.h>

#include <QtCore/QMap>

namespace UserMetricsOutput {

class UserData;
class UserMetricsStore;

typedef QSharedPointer<UserData> UserDataPtr;

class UserData: public QObject {
Q_OBJECT

public:
	typedef QMap<QString, DataSetPtr> DataSetMap;

	typedef DataSetMap::iterator iterator;

	typedef DataSetMap::const_iterator const_iterator;

	explicit UserData(UserMetricsStore &userMetricsStore, QObject *parent = 0);

	virtual ~UserData();

	virtual const_iterator constBegin() const;

	virtual const_iterator constEnd() const;

	virtual iterator insert(const QString &dataSourceName, DataSetPtr dataSet);

Q_SIGNALS:
	void dataSetAdded(const QString &dataSourceName);

protected:
	DataSetMap m_dataSets;

	UserMetricsStore &m_userMetricsStore;
};

}

#endif // USERMETRICSOUTPUT_USERDATA_H_
