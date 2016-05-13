/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#include "../StreetlightdArgumentParser.h"
#include "CommandLineParser_Mock.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <sstream>
#include <set>
#include <string>

class StreetlightdArgumentParser_Test:
    public testing::Test
{
public:
  CommandLineParserMock parser;
  StreetlightdArgumentParser testee{parser};

  void fillDefaultEnums()
  {
    ON_CALL(parser, contains("application")).WillByDefault(testing::Return(true));
    ON_CALL(parser, value("application")).WillByDefault(testing::Return("x"));
    ON_CALL(parser, contains("presentation")).WillByDefault(testing::Return(true));
    ON_CALL(parser, value("presentation")).WillByDefault(testing::Return("x"));
    ON_CALL(parser, contains("session")).WillByDefault(testing::Return(true));
    ON_CALL(parser, value("session")).WillByDefault(testing::Return("x"));
    testee.addApplications({"x"});
    testee.addPresentations({"x"});
    testee.addSessions({"x"});
  }
};

TEST_F(StreetlightdArgumentParser_Test, output_is_not_valid_when_not_provided_all_arguments)
{
  const auto result = testee.parse({});

  ASSERT_FALSE(result);
}

TEST_F(StreetlightdArgumentParser_Test, show_help_when_requested)
{
  ON_CALL(parser, isValid()).WillByDefault(testing::Return(true));
  ON_CALL(parser, contains("help")).WillByDefault(testing::Return(true));

  EXPECT_CALL(parser, printHelp());

  testee.parse({});
}

TEST_F(StreetlightdArgumentParser_Test, show_help_when_missing_required_arguments)
{
  EXPECT_CALL(parser, printHelp());
  ON_CALL(parser, isValid()).WillByDefault(testing::Return(true));
  ON_CALL(parser, contains("application")).WillByDefault(testing::Return(false));

  testee.parse({});
}

TEST_F(StreetlightdArgumentParser_Test, output_is_valid_when_provided_with_valid_arguments)
{
  ON_CALL(parser, isValid()).WillByDefault(testing::Return(true));
  ON_CALL(parser, contains("application")).WillByDefault(testing::Return(true));
  ON_CALL(parser, value("application")).WillByDefault(testing::Return("A"));
  ON_CALL(parser, contains("presentation")).WillByDefault(testing::Return(true));
  ON_CALL(parser, value("presentation")).WillByDefault(testing::Return("P"));
  ON_CALL(parser, contains("session")).WillByDefault(testing::Return(true));
  ON_CALL(parser, value("session")).WillByDefault(testing::Return("S"));
  testee.addApplications({"A"});
  testee.addPresentations({"P"});
  testee.addSessions({"S"});

  const auto result = testee.parse({});

  ASSERT_TRUE(result);
}

TEST_F(StreetlightdArgumentParser_Test, output_is_not_valid_when_provided_invalid_arguments)
{
  ON_CALL(parser, isValid()).WillByDefault(testing::Return(true));
  ON_CALL(parser, contains("application")).WillByDefault(testing::Return(true));
  ON_CALL(parser, value("application")).WillByDefault(testing::Return("X"));
  ON_CALL(parser, contains("presentation")).WillByDefault(testing::Return(true));
  ON_CALL(parser, value("presentation")).WillByDefault(testing::Return("P"));
  ON_CALL(parser, contains("session")).WillByDefault(testing::Return(true));
  ON_CALL(parser, value("session")).WillByDefault(testing::Return("S"));
  testee.addApplications({"A"});
  testee.addPresentations({"P"});
  testee.addSessions({"S"});

  const auto result = testee.parse({});

  ASSERT_FALSE(result);
}

TEST_F(StreetlightdArgumentParser_Test, return_the_specified_enum_values)
{
  ON_CALL(parser, isValid()).WillByDefault(testing::Return(true));
  ON_CALL(parser, contains("application")).WillByDefault(testing::Return(true));
  ON_CALL(parser, value("application")).WillByDefault(testing::Return("b"));
  ON_CALL(parser, contains("presentation")).WillByDefault(testing::Return(true));
  ON_CALL(parser, value("presentation")).WillByDefault(testing::Return("d"));
  ON_CALL(parser, contains("session")).WillByDefault(testing::Return(true));
  ON_CALL(parser, value("session")).WillByDefault(testing::Return("i"));
  testee.addApplications({"a", "b", "c"});
  testee.addPresentations({"d", "e", "f"});
  testee.addSessions({"g", "h", "i"});

  const auto result = testee.parse({});

  ASSERT_EQ("b", result.application);
  ASSERT_EQ("d", result.presentation);
  ASSERT_EQ("i", result.session);
}

TEST_F(StreetlightdArgumentParser_Test, can_specify_the_host)
{
  fillDefaultEnums();
  ON_CALL(parser, isValid()).WillByDefault(testing::Return(true));
  ON_CALL(parser, value("host", testing::_)).WillByDefault(testing::Return("someAddress"));

  const auto result = testee.parse({});

  ASSERT_EQ("someAddress", result.host);
}

TEST_F(StreetlightdArgumentParser_Test, can_specify_the_user)
{
  fillDefaultEnums();
  ON_CALL(parser, isValid()).WillByDefault(testing::Return(true));
  ON_CALL(parser, value("user", testing::_)).WillByDefault(testing::Return("someUser"));

  const auto result = testee.parse({});

  ASSERT_EQ("someUser", result.user);
}

TEST_F(StreetlightdArgumentParser_Test, can_specify_password)
{
  fillDefaultEnums();
  ON_CALL(parser, isValid()).WillByDefault(testing::Return(true));
  ON_CALL(parser, value("password", testing::_)).WillByDefault(testing::Return("mySecret"));

  const auto result = testee.parse({});

  ASSERT_EQ("mySecret", result.password);
}

TEST_F(StreetlightdArgumentParser_Test, do_not_use_external_timer_by_default)
{
  fillDefaultEnums();

  const auto result = testee.parse({});

  ASSERT_FALSE(result.externalTimer);
}

TEST_F(StreetlightdArgumentParser_Test, can_specify_to_use_external_timer)
{
  fillDefaultEnums();
  ON_CALL(parser, isValid()).WillByDefault(testing::Return(true));
  ON_CALL(parser, contains("external-timer")).WillByDefault(testing::Return(true));

  const auto result = testee.parse({});

  ASSERT_TRUE(result.externalTimer);
}
