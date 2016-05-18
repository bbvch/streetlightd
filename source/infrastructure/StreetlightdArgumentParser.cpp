/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#include "StreetlightdArgumentParser.h"

#include <map>

static const std::string HOST_ARGUMENT = "host";
static const std::string USER_ARGUMENT = "user";
static const std::string PASSWORD_ARGUMENT = "password";
static const std::string TIMER_ARGUMENT = "external-timer";

StreetlightdArgumentParser::StreetlightdArgumentParser(CommandLineParser &_parser) :
  parser{_parser}
{
  enums[Layer::Application] = EnumEntry{"application", "a", {}};
  enums[Layer::Presentation] = EnumEntry{"presentation", "p", {}};
  enums[Layer::Session] = EnumEntry{"session", "s", {}};
}

void StreetlightdArgumentParser::addApplications(const std::set<std::string> &values)
{
  enums[Layer::Application].values.insert(values.begin(), values.end());
}

void StreetlightdArgumentParser::addPresentations(const std::set<std::string> &values)
{
  enums[Layer::Presentation].values.insert(values.begin(), values.end());
}

void StreetlightdArgumentParser::addSessions(const std::set<std::string> &values)
{
  enums[Layer::Session].values.insert(values.begin(), values.end());
}

Configuration StreetlightdArgumentParser::parse(const std::vector<std::string> &arguments)
{
  parser.parse(arguments, createOptions());
  if (!parser.isValid()) {
    parser.printHelp();
    return {};
  }

  if (parser.contains("help")) {
    parser.printHelp();
    return {};
  }

  auto enumValues = fillEnumValues();
  if (enumValues.empty()) {
    //TODO test this case
    parser.printHelp();
    return {};
  }

  Configuration config{};
  fillStackConfig(config, enumValues);
  fillSessionConfig(config);
  fillTimerConfig(config);
  return config;
}

std::map<StreetlightdArgumentParser::Layer,std::string> StreetlightdArgumentParser::fillEnumValues() const
{
  std::map<Layer,std::string> enumValues{};
  for (const auto layer : {Layer::Application, Layer::Presentation, Layer::Session}) {
    const auto value = valueFor(layer);
    if (value.empty()) {
      return {};
    }
    enumValues[layer] = value;
  }

  return enumValues;
}

void StreetlightdArgumentParser::fillStackConfig(StackConfiguration &config, std::map<Layer, std::string> enumValues) const
{
  config.application = enumValues[Layer::Application];
  config.presentation = enumValues[Layer::Presentation];
  config.session = enumValues[Layer::Session];
}

void StreetlightdArgumentParser::fillSessionConfig(session::Configuration &config) const
{
  config.host = parser.value(HOST_ARGUMENT, "");
  config.user = parser.value(USER_ARGUMENT, "");
  config.password = parser.value(PASSWORD_ARGUMENT, "");
}

void StreetlightdArgumentParser::fillTimerConfig(TimerConfiguration &config) const
{
  config.externalTimer = parser.contains(TIMER_ARGUMENT);
}

Poco::Util::OptionSet StreetlightdArgumentParser::createOptions() const
{
  Poco::Util::OptionSet options{};

  options.addOption(Poco::Util::Option{"help",         "h", "print this help", false});

  for (const auto &opt : enums) {
    options.addOption(opt.second.asOption());
  }

  options.addOption(Poco::Util::Option{HOST_ARGUMENT,     "", "host to connect to", false}.argument("<" + HOST_ARGUMENT + ">"));
  options.addOption(Poco::Util::Option{USER_ARGUMENT,     "", "user of connection", false}.argument("<" + USER_ARGUMENT + ">"));
  options.addOption(Poco::Util::Option{PASSWORD_ARGUMENT, "", "password for connection", false}.argument("<" + PASSWORD_ARGUMENT + ">"));
  options.addOption(Poco::Util::Option{TIMER_ARGUMENT,    "", "trigger updates via DBus", false});
  return options;
}

std::string StreetlightdArgumentParser::keyFor(StreetlightdArgumentParser::Layer entry) const
{
  const auto &pos = enums.find(entry);
  if (pos == enums.end()) {
    throw std::runtime_error("entry not defined: " + std::to_string(int(entry)));
  }
  return pos->second.longName;
}

StreetlightdArgumentParser::EnumEntry StreetlightdArgumentParser::entryFor(StreetlightdArgumentParser::Layer entry) const
{
  const auto &pos = enums.find(entry);
  if (pos == enums.end()) {
    throw std::runtime_error("entry not defined: " + std::to_string(int(entry)));
  }
  return pos->second;
}

std::string StreetlightdArgumentParser::valueFor(StreetlightdArgumentParser::Layer type) const
{
  EnumEntry entry = entryFor(type);
  if (!parser.contains(entry.longName)) {
    return {};
  }

  const auto value = parser.value(entry.longName);
  const auto contains = entry.values.find(value) != entry.values.end();

  if (!contains) {
    return {};
  }

  return value;
}

static std::string join(const std::set<std::string> &list)
{
  std::string result{};
  for (const auto &product : list) {
    result += " " + product;
  }
  return result;
}

Poco::Util::Option StreetlightdArgumentParser::EnumEntry::asOption() const
{
  const std::string argument{"<"+longName+">"};
  const std::string help{argument + ":" + join(values)};
  return Poco::Util::Option{longName,  shortName, help, true}.argument(argument);
}