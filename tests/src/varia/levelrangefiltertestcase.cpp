/*
 * Copyright 2003,2004 The Apache Software Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <log4cxx/logger.h>
#include <log4cxx/simplelayout.h>
#include <log4cxx/fileappender.h>
#include <log4cxx/level.h>
#include <log4cxx/varia/levelrangefilter.h>

#include "../util/compare.h"

#include <log4cxx/helpers/pool.h>
#include <log4cxx/helpers/stringhelper.h>
#include <log4cxx/helpers/transcoder.h>
#include "../testchar.h"
#include <log4cxx/spi/loggerrepository.h>

using namespace log4cxx;
using namespace log4cxx::helpers;
using namespace log4cxx::varia;


#define ACCEPT_FILE LOG4CXX_FILE("output/LevelRangeFilter_accept")
#define ACCEPT_WITNESS LOG4CXX_FILE("witness/LevelRangeFilter_accept")
#define NEUTRAL_FILE LOG4CXX_FILE("output/LevelRangeFilter_neutral")
#define NEUTRAL_WITNESS LOG4CXX_FILE("witness/LevelMatchFilter_neutral")

class LevelRangeFilterTestCase : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(LevelRangeFilterTestCase);
		CPPUNIT_TEST(accept);
		CPPUNIT_TEST(neutral);
	CPPUNIT_TEST_SUITE_END();

	LoggerPtr root;
	LoggerPtr logger;

public:
	void setUp()
	{
		root = Logger::getRootLogger();
		root->removeAllAppenders();
		logger = Logger::getLogger(LOG4CXX_TEST_STR("test"));
	}

	void tearDown()
	{
		root->getLoggerRepository()->resetConfiguration();
	}

	void accept()
	{
		// set up appender
		LayoutPtr layout = new SimpleLayout();
		AppenderPtr appender = new FileAppender(layout, ACCEPT_FILE, false);

		// create LevelMatchFilter
		LevelRangeFilterPtr rangeFilter = new LevelRangeFilter();

		// set it to accept on a match
		rangeFilter->setAcceptOnMatch(true);

		// attach match filter to appender
		appender->addFilter(rangeFilter);

		// set appender on root and set level to debug
		root->addAppender(appender);
		root->setLevel(Level::DEBUG);

		int passCount = 0;
		LogString sbuf(LOG4CXX_STR("pass "));

        Pool pool;
        sbuf.append(StringHelper::toString(passCount, pool));

        sbuf.append(LOG4CXX_STR("; no min or max set"));
		common(sbuf);
		passCount++;

		// test with a min set
		rangeFilter->setLevelMin(Level::WARN);
		sbuf.assign(LOG4CXX_STR("pass "));
        sbuf.append(StringHelper::toString(passCount, pool));
        sbuf.append(LOG4CXX_STR("; min set to WARN, max not set"));
		common(sbuf);
		passCount++;

		// create a clean filter
		appender->clearFilters();
		rangeFilter = new LevelRangeFilter();
		appender->addFilter(rangeFilter);

		//test with max set
		rangeFilter->setLevelMax(Level::WARN);
		sbuf.assign(LOG4CXX_STR("pass "));
        sbuf.append(StringHelper::toString(passCount, pool));
        sbuf.append(LOG4CXX_STR("; min not set, max set to WARN"));
		common(sbuf);
		passCount++;


		LevelPtr levelArray[] =
			{ Level::getDebug(), Level::getInfo(), Level::getWarn(), 
                 Level::getError(), Level::getFatal() };

		int length = sizeof(levelArray)/sizeof(levelArray[0]);

		for (int x = 0; x < length; x++)
		{
			// set the min level to match
			rangeFilter->setLevelMin(levelArray[x]);

			for (int y = length - 1; y >= 0; y--)
			{
				// set max level to match
				rangeFilter->setLevelMax(levelArray[y]);

				sbuf.assign(LOG4CXX_STR("pass "));
                                sbuf.append(StringHelper::toString(passCount, pool));
				sbuf.append(LOG4CXX_STR("; filter set to accept between "));
				sbuf.append(levelArray[x]->toString());
                                sbuf.append(LOG4CXX_STR(" and "));
				sbuf.append(levelArray[y]->toString());
                                sbuf.append(LOG4CXX_STR(" msgs"));
				common(sbuf);

				// increment passCount
				passCount++;
			}
		}


		CPPUNIT_ASSERT(Compare::compare(ACCEPT_FILE, ACCEPT_WITNESS));
	}

	void neutral()
	{
		// set up appender
		LayoutPtr layout = new SimpleLayout();
		AppenderPtr appender = new FileAppender(layout, ACCEPT_FILE, false);

		// create LevelMatchFilter
		LevelRangeFilterPtr rangeFilter = new LevelRangeFilter();

		// set it to accept on a match
		rangeFilter->setAcceptOnMatch(true);

		// attach match filter to appender
		appender->addFilter(rangeFilter);

		// set appender on root and set level to debug
		root->addAppender(appender);
		root->setLevel(Level::DEBUG);

		int passCount = 0;
		LogString sbuf(LOG4CXX_STR("pass "));

                Pool pool;
                sbuf.append(StringHelper::toString(passCount, pool));

		// test with no min or max set
		sbuf.append(LOG4CXX_STR("; no min or max set"));
		common(sbuf);
		passCount++;

		// test with a min set
		rangeFilter->setLevelMin(Level::WARN);
		sbuf.assign(LOG4CXX_STR("pass "));

                sbuf.append(StringHelper::toString(passCount, pool));
                sbuf.append(LOG4CXX_STR("; min set to WARN, max not set"));
		common(sbuf);
		passCount++;

		// create a clean filter
		appender->clearFilters();
		rangeFilter = new LevelRangeFilter();
		appender->addFilter(rangeFilter);

		//test with max set
		rangeFilter->setLevelMax(Level::WARN);
		sbuf.append(LOG4CXX_STR("pass "));

                sbuf.append(StringHelper::toString(passCount, pool));

		sbuf.append(LOG4CXX_STR("; min not set, max set to WARN"));
		common(sbuf);
		passCount++;

		LevelPtr levelArray[] =
			{ Level::getDebug(), Level::getInfo(), Level::getWarn(), 
                 Level::getError(), Level::getFatal() };

		int length = sizeof(levelArray)/sizeof(levelArray[0]);

		for (int x = 0; x < length; x++)
		{
			// set the min level to match
			rangeFilter->setLevelMin(levelArray[x]);

			for (int y = length - 1; y >= 0; y--)
			{
				// set max level to match
				rangeFilter->setLevelMax(levelArray[y]);

				sbuf.assign(LOG4CXX_STR("pass "));
                                sbuf.append(StringHelper::toString(passCount, pool));
				sbuf.append(LOG4CXX_STR("; filter set to accept between "));
				sbuf.append(levelArray[x]->toString());
                                sbuf.append(LOG4CXX_STR(" and "));
                                sbuf.append(levelArray[y]->toString());
                                sbuf.append(LOG4CXX_STR(" msgs"));
				common(sbuf);

				// increment passCount
				passCount++;
			}
		}

		CPPUNIT_ASSERT(Compare::compare(NEUTRAL_FILE, NEUTRAL_WITNESS));
 	}

	void common(const LogString& msg)
	{
		logger->debug(msg);
		logger->info(msg);
		logger->warn(msg);
		logger->error(msg);
		logger->fatal(msg);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(LevelRangeFilterTestCase);
