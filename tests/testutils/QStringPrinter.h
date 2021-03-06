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

#ifndef USERMETRICS_TESTUTILS_QSTRINGPRINTER_H_
#define USERMETRICS_TESTUTILS_QSTRINGPRINTER_H_

#include <iostream>
#include <QtCore/QString>

void PrintTo(const QString& str, std::ostream* os);

void PrintTo(const QChar& chr, std::ostream* os);

#endif /* USERMETRICS_TESTUTILS_QSTRINGPRINTER_H_ */
