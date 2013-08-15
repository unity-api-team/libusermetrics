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

#include <stdexcept>

#include <usermetricsservice/DBusUserMetrics.h>
#include <usermetricsservice/DBusDataSource.h>
#include <usermetricsservice/DBusUserData.h>
#include <usermetricsservice/DBusDataSet.h>
#include <libusermetricscommon/DateFactory.h>
#include <libusermetricscommon/DBusPaths.h>

#include <testutils/DBusTest.h>
#include <testutils/QStringPrinter.h>
#include <testutils/QVariantListPrinter.h>

#include <QDjango.h>

#include <QSqlDatabase>
#include <QtCore/QVariantList>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace std;
using namespace testing;
using namespace UserMetricsCommon;
using namespace UserMetricsService;
using namespace UserMetricsTestUtils;

namespace {

class MockDateFactory: public DateFactory {
public:
	MOCK_CONST_METHOD0(currentDate, QDate());
};

class TestUserMetricsService: public DBusTest {
protected:
	TestUserMetricsService() :
			db(
					QSqlDatabase::addDatabase("QSQLITE",
							"test-user-metrics-service")), dateFactory(
					new NiceMock<MockDateFactory>()) {
		db.setDatabaseName(":memory:");
		if (!db.open()) {
			throw logic_error("Could not open memory database");
		}

		ON_CALL(*dateFactory, currentDate()).WillByDefault(
				Return(QDate(2001, 01, 07)));

//		QDjango::setDebugEnabled(true);
		QDjango::setDatabase(db);
	}

	virtual ~TestUserMetricsService() {
		QDjango::dropTables();
		db.close();
		QSqlDatabase::removeDatabase("test-user-metrics-service");
	}

	QSqlDatabase db;

	QSharedPointer<MockDateFactory> dateFactory;
};

TEST_F(TestUserMetricsService, PersistsDataSourcesBetweenRestart) {
	{
		DBusUserMetrics userMetrics(systemConnection(), dateFactory);

		EXPECT_TRUE(userMetrics.dataSources().empty());

		EXPECT_EQ(QString("/com/canonical/UserMetrics/DataSource/1"),
				userMetrics.createDataSource("facebook", "%1 messages received",
						"no facebook data", "facebook text domain").path());

		DBusDataSourcePtr facebook(userMetrics.dataSource("facebook"));
		EXPECT_EQ(QString("facebook"), facebook->name());
		EXPECT_EQ(QString("%1 messages received"), facebook->formatString());

		QList<QDBusObjectPath> dataSources(userMetrics.dataSources());
		EXPECT_EQ(1, dataSources.size());
		EXPECT_EQ(QString("/com/canonical/UserMetrics/DataSource/1"),
				dataSources.first().path());
	}

	{
		DBusUserMetrics userMetrics(systemConnection(), dateFactory);

		QList<QDBusObjectPath> dataSources(userMetrics.dataSources());
		EXPECT_EQ(1, dataSources.size());
		EXPECT_EQ(QString("/com/canonical/UserMetrics/DataSource/1"),
				dataSources.first().path());

		DBusDataSourcePtr facebook(userMetrics.dataSource("facebook"));
		EXPECT_EQ(QString("facebook"), facebook->name());
		EXPECT_EQ(QString("%1 messages received"), facebook->formatString());
		EXPECT_EQ(QString("no facebook data"), facebook->emptyDataString());
		EXPECT_EQ(QString("facebook text domain"), facebook->textDomain());
	}
}

TEST_F(TestUserMetricsService, UpdatesFormatString) {
	{
		DBusUserMetrics userMetrics(systemConnection(), dateFactory);

		userMetrics.createDataSource("twitter", "%1 tweets received", "", "");

		DBusDataSourcePtr twitter(userMetrics.dataSource("twitter"));
		EXPECT_EQ(QString("%1 tweets received"), twitter->formatString());

		twitter->setFormatString("%1 new format string");
		EXPECT_EQ(QString("%1 new format string"), twitter->formatString());
	}

	{
		DBusUserMetrics userMetrics(systemConnection(), dateFactory);

		DBusDataSourcePtr twitter(userMetrics.dataSource("twitter"));
		EXPECT_EQ(QString("%1 new format string"), twitter->formatString());
	}
}

TEST_F(TestUserMetricsService, UpdatesEmptyDataString) {
	{
		DBusUserMetrics userMetrics(systemConnection(), dateFactory);

		userMetrics.createDataSource("twitter", "%1 tweets received",
				"no tweets today", "");

		DBusDataSourcePtr twitter(userMetrics.dataSource("twitter"));
		EXPECT_EQ(QString("no tweets today"), twitter->emptyDataString());

		twitter->setEmptyDataString("no tweeties today");
		EXPECT_EQ(QString("no tweeties today"), twitter->emptyDataString());
	}

	{
		DBusUserMetrics userMetrics(systemConnection(), dateFactory);

		DBusDataSourcePtr twitter(userMetrics.dataSource("twitter"));
		EXPECT_EQ(QString("no tweeties today"), twitter->emptyDataString());
	}
}

TEST_F(TestUserMetricsService, UpdatesTextDomain) {
	{
		DBusUserMetrics userMetrics(systemConnection(), dateFactory);

		userMetrics.createDataSource("twitter", "%1 tweets received", "",
				"start text domain");

		DBusDataSourcePtr twitter(userMetrics.dataSource("twitter"));
		EXPECT_EQ(QString("start text domain"), twitter->textDomain());

		twitter->setTextDomain("changed text domain");
		EXPECT_EQ(QString("changed text domain"), twitter->textDomain());
	}

	{
		DBusUserMetrics userMetrics(systemConnection(), dateFactory);

		DBusDataSourcePtr twitter(userMetrics.dataSource("twitter"));
		EXPECT_EQ(QString("changed text domain"), twitter->textDomain());
	}
}

TEST_F(TestUserMetricsService, UpdatesFormatStringOnCreate) {
	{
		DBusUserMetrics userMetrics(systemConnection(), dateFactory);

		userMetrics.createDataSource("twitter", "%1 tweets received", "", "");

		DBusDataSourcePtr twitter(userMetrics.dataSource("twitter"));
		EXPECT_EQ(QString("twitter"), twitter->name());
		EXPECT_EQ(QString("%1 tweets received"), twitter->formatString());

		userMetrics.createDataSource("twitter", "%1 new format string", "", "");

		EXPECT_EQ(QString("twitter"), twitter->name());
		EXPECT_EQ(QString("%1 new format string"), twitter->formatString());
	}

	{
		DBusUserMetrics userMetrics(systemConnection(), dateFactory);

		DBusDataSourcePtr twitter(userMetrics.dataSource("twitter"));
		EXPECT_EQ(QString("%1 new format string"), twitter->formatString());
	}
}

TEST_F(TestUserMetricsService, PersistsUserDataBetweenRestart) {
	{
		DBusUserMetrics userMetrics(systemConnection(), dateFactory);

		EXPECT_TRUE(userMetrics.dataSources().empty());

		EXPECT_EQ(QString("/com/canonical/UserMetrics/UserData/1"),
				userMetrics.createUserData("alice").path());

		DBusUserDataPtr alice(userMetrics.userData("alice"));
		EXPECT_EQ(QString("alice"), alice->username());

		QList<QDBusObjectPath> userData(userMetrics.userDatas());
		EXPECT_EQ(1, userData.size());
		EXPECT_EQ(QString("/com/canonical/UserMetrics/UserData/1"),
				userData.first().path());
	}

	{
		DBusUserMetrics userMetrics(systemConnection(), dateFactory);

		QList<QDBusObjectPath> userData(userMetrics.userDatas());
		EXPECT_EQ(1, userData.size());
		EXPECT_EQ(QString("/com/canonical/UserMetrics/UserData/1"),
				userData.first().path());

		DBusUserDataPtr alice(userMetrics.userData("alice"));
		EXPECT_EQ(QString("alice"), alice->username());
	}
}

TEST_F(TestUserMetricsService, PersistsDataSetsBetweenRestart) {
	QVariantList data( { 100.0, 50.0, 0.0, -50.0, -100.0 });

	{
		DBusUserMetrics userMetrics(systemConnection(), dateFactory);

		userMetrics.createDataSource("twitter", "%1 tweets received", "", "");
		userMetrics.createUserData("alice");

		DBusUserDataPtr alice(userMetrics.userData("alice"));

		EXPECT_TRUE(alice->dataSets().empty());

		// should re-use the same data set
		EXPECT_EQ(QString("/com/canonical/UserMetrics/DataSet/1"),
				alice->createDataSet("twitter").path());;
		EXPECT_EQ(QString("/com/canonical/UserMetrics/DataSet/1"),
				alice->createDataSet("twitter").path());;

		DBusDataSetPtr twitter(alice->dataSet("twitter"));
		EXPECT_EQ(1, twitter->id());

		QList<QDBusObjectPath> dataSets(alice->dataSets());
		EXPECT_EQ(1, dataSets.size());
		EXPECT_EQ(QString("/com/canonical/UserMetrics/DataSet/1"),
				dataSets.first().path());

		twitter->update(data);
	}

	{
		DBusUserMetrics userMetrics(systemConnection(), dateFactory);

		DBusUserDataPtr alice(userMetrics.userData("alice"));
		EXPECT_EQ(QString("alice"), alice->username());

		QList<QDBusObjectPath> dataSets(alice->dataSets());
		EXPECT_EQ(1, dataSets.size());
		EXPECT_EQ(QString("/com/canonical/UserMetrics/DataSet/1"),
				dataSets.first().path());

		DBusDataSetPtr twitter(alice->dataSet("twitter"));
		EXPECT_EQ(1, twitter->id());

		EXPECT_EQ(data, twitter->data());
	}
}

TEST_F(TestUserMetricsService, CreateDataSetsWithUnknownSourceFailsGracefully) {
	DBusUserMetrics userMetrics(systemConnection(), dateFactory);

	userMetrics.createUserData("bob");
	DBusUserDataPtr bob(userMetrics.userData("bob"));

	EXPECT_EQ(QString(), bob->createDataSet("twitter").path());
}

TEST_F(TestUserMetricsService, UpdateData) {
	EXPECT_CALL(*dateFactory, currentDate()).Times(2).WillOnce(
			Return(QDate(2001, 01, 5))).WillOnce(Return(QDate(2001, 01, 8)));

	DBusUserMetrics userMetrics(systemConnection(), dateFactory);
	userMetrics.createDataSource("twitter", "foo", "", "");

	userMetrics.createUserData("bob");
	DBusUserDataPtr bob(userMetrics.userData("bob"));

	bob->createDataSet("twitter");
	DBusDataSetPtr twitter(bob->dataSet("twitter"));

	QVariantList first( { 5.0, 4.0, 3.0, 2.0, 1.0 });
	QVariantList second( { 10.0, 9.0, 8.0, 7.0, 6.0 });
	QVariantList expected( { 10.0, 9.0, 8.0, 7.0, 6.0, 3.0, 2.0, 1.0 });

	// first update happens on the 5th of the month
	twitter->update(first);
	EXPECT_EQ(first, twitter->data());
	EXPECT_EQ(QDate(2001, 01, 5), twitter->lastUpdatedDate());

	// second update happens on the 8th of the month
	// -> 3 new data points and 2 overwritten
	twitter->update(second);
	EXPECT_EQ(expected, twitter->data());
	EXPECT_EQ(QDate(2001, 01, 8), twitter->lastUpdatedDate());
}

TEST_F(TestUserMetricsService, UpdateDataWithGap) {
	EXPECT_CALL(*dateFactory, currentDate()).Times(2).WillOnce(
			Return(QDate(2001, 01, 5))).WillOnce(Return(QDate(2001, 01, 15)));

	DBusUserMetrics userMetrics(systemConnection(), dateFactory);
	userMetrics.createDataSource("twitter", "foo", "", "");

	userMetrics.createUserData("bob");
	DBusUserDataPtr bob(userMetrics.userData("bob"));

	bob->createDataSet("twitter");
	DBusDataSetPtr twitter(bob->dataSet("twitter"));

	QVariantList first( { 5.0, 4.0, 3.0, 2.0, 1.0 });
	QVariantList second( { 10.0, 9.0, 8.0, 7.0, 6.0 });
	QVariantList expected;
	expected.append(second);
	for (int i(0); i < 5; ++i) {
		expected.append(QVariant(""));
	}
	expected.append(first);

	// first update happens on the 5th of the month
	twitter->update(first);
	EXPECT_EQ(first, twitter->data());
	EXPECT_EQ(QDate(2001, 01, 5), twitter->lastUpdatedDate());

	// second update happens on the 15th of the month
	// -> 5 new data points, 5 nulls, and none overwritten
	twitter->update(second);
	EXPECT_EQ(expected, twitter->data());
	EXPECT_EQ(QDate(2001, 01, 15), twitter->lastUpdatedDate());
}

TEST_F(TestUserMetricsService, UpdateDataTotallyOverwrite) {
	EXPECT_CALL(*dateFactory, currentDate()).Times(2).WillOnce(
			Return(QDate(2001, 01, 5))).WillOnce(Return(QDate(2001, 01, 7)));

	DBusUserMetrics userMetrics(systemConnection(), dateFactory);
	userMetrics.createDataSource("twitter", "foo", "", "");

	userMetrics.createUserData("bob");
	DBusUserDataPtr bob(userMetrics.userData("bob"));

	bob->createDataSet("twitter");
	DBusDataSetPtr twitter(bob->dataSet("twitter"));

	QVariantList first( { 3.0, 2.0, 1.0 });
	QVariantList second( { 10.0, 9.0, 8.0, 7.0, 6.0, 5.0, 4.0 });
	QVariantList &expected(second);

	// first update happens on the 5th of the month
	twitter->update(first);
	EXPECT_EQ(first, twitter->data());
	EXPECT_EQ(QDate(2001, 01, 5), twitter->lastUpdatedDate());

	// second update happens on the 7th of the month
	// -> 2 new data points, 3 overwrites, and 2 new appends
	twitter->update(second);
	EXPECT_EQ(expected, twitter->data());
	EXPECT_EQ(QDate(2001, 01, 7), twitter->lastUpdatedDate());
}

TEST_F(TestUserMetricsService, MultipleUsers) {
	DBusUserMetrics userMetrics(systemConnection(), dateFactory);
	userMetrics.createDataSource("twitter", "foo", "", "");

	userMetrics.createUserData("alice");
	DBusUserDataPtr alice(userMetrics.userData("alice"));

	userMetrics.createUserData("bob");
	DBusUserDataPtr bob(userMetrics.userData("bob"));

	alice->createDataSet("twitter");
	DBusDataSetPtr aliceTwitter(alice->dataSet("twitter"));

	bob->createDataSet("twitter");
	DBusDataSetPtr bobTwitter(bob->dataSet("twitter"));

	QList<QDBusObjectPath> aliceDataSets(alice->dataSets());
	ASSERT_EQ(1, aliceDataSets.size());
	EXPECT_EQ(DBusPaths::dataSet(1), aliceDataSets.first().path());

	QList<QDBusObjectPath> bobDataSets(bob->dataSets());
	ASSERT_EQ(1, bobDataSets.size());
	EXPECT_EQ(DBusPaths::dataSet(2), bobDataSets.first().path());
}

TEST_F(TestUserMetricsService, IncrementOverSeveralDays) {
	EXPECT_CALL(*dateFactory, currentDate()).WillRepeatedly(
			Return(QDate(2001, 03, 1)));

	DBusUserMetrics userMetrics(systemConnection(), dateFactory);
	userMetrics.createDataSource("twitter", "foo", "", "");

	userMetrics.createUserData("bob");
	DBusUserDataPtr bob(userMetrics.userData("bob"));

	bob->createDataSet("twitter");
	DBusDataSetPtr twitter(bob->dataSet("twitter"));

	QVariantList expected1( { 1.0 });
	QVariantList expected2( { 1.0, 1.0 });
	QVariantList expected3( { 1.0, 1.0, 1.0 });

	twitter->increment(1.0);
	EXPECT_EQ(expected1, twitter->data());
	EXPECT_EQ(QDate(2001, 03, 1), twitter->lastUpdatedDate());

	EXPECT_CALL(*dateFactory, currentDate()).WillRepeatedly(
			Return(QDate(2001, 03, 2)));

	twitter->increment(1.0);
	EXPECT_EQ(expected2, twitter->data());
	EXPECT_EQ(QDate(2001, 03, 2), twitter->lastUpdatedDate());

	EXPECT_CALL(*dateFactory, currentDate()).WillRepeatedly(
			Return(QDate(2001, 03, 3)));

	twitter->increment(1.0);
	EXPECT_EQ(expected3, twitter->data());
	EXPECT_EQ(QDate(2001, 03, 3), twitter->lastUpdatedDate());
}

TEST_F(TestUserMetricsService, StoreMaximumOf62Days) {
	EXPECT_CALL(*dateFactory, currentDate()).WillRepeatedly(
			Return(QDate(2001, 3, 5)));

	DBusUserMetrics userMetrics(systemConnection(), dateFactory);
	userMetrics.createDataSource("twitter", "foo", "", "");

	userMetrics.createUserData("bob");
	DBusUserDataPtr bob(userMetrics.userData("bob"));

	bob->createDataSet("twitter");
	DBusDataSetPtr twitter(bob->dataSet("twitter"));

	// create some data that's way too big
	QVariantList input;
	for (int i(0); i < 100; ++i) {
		input << 1.0;
	}
	twitter->update(input);

	// create some data that's way too big
	QVariantList expected;
	for (int i(0); i < 62; ++i) {
		expected << 1.0;
	}

	EXPECT_EQ(expected, twitter->data());
}

} // namespace
