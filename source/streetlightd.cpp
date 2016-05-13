/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#include "protocolstack/ProtocolStack.h"
#include "protocolstack/StackFactory.h"

#include "dbus/DbusTimer.h"
#include "dbus/Streetlight.h"

#include "infrastructure/Factory.h"
#include "infrastructure/StreetlightdArgumentParser.h"
#include "infrastructure/TimerFactory.h"

#include <dbus-c++/dbus.h>
#include <dbus-c++/api.h>
#include <dbus-c++-1/dbus-c++/dbus.h>

#include <csignal>
#include <memory>

static DBus::BusDispatcher dispatcher;

static void niam(int)
{
  dispatcher.leave();
}

int main(int argc, char **argv)
{
  DBus::_init_threading();
  DBus::default_dispatcher = &dispatcher;

  signal(SIGTERM, niam);
  signal(SIGINT, niam);

  const std::vector<std::string> arg{argv, argv+argc};

  StackFactory factory;
  CommandLineParserImplementation baseParser{std::cout};
  StreetlightdArgumentParser parser{baseParser};
  parser.addApplications(factory.applications());
  parser.addPresentations(factory.presentations());
  parser.addSessions(factory.sessions());
  Configuration configuration = parser.parse(arg);

  if (!configuration) {
    return -1;
  }

  ProtocolStack stack = factory.produce(configuration);
  stack.session->setConfiguration(configuration);

  // DBus creation
  DBus::Connection connection = DBus::Connection::SessionBus();
  connection.request_name("ch.bbv.streetlightd");
  BrightnessSensor brightness{connection};
  MoistureSensor moisture{connection};
  ProximitySensor proximity{connection};
  LuminosityActor luminosity{connection};
  WarningActor warning{connection};

  //  Timer creation
  TimerFactory timerFactory{connection};
  std::unique_ptr<Timer> timer = std::unique_ptr<Timer>{timerFactory.produce(configuration)};

  // connection
  stack.application->setBrightnessSensor([&brightness]{
    //TODO check if dbus-c++ is thread safe; if not, protect it against those calls
    return brightness.scaled();
  });
  stack.application->setMoistureSensor([&moisture]{
    return moisture.scaled();
  });
  stack.application->setProximitySensor([&proximity]{
    return proximity.scaled();
  });
  stack.application->setLuminosityActor([&luminosity](double value){
    luminosity.scaled(value);
  });
  stack.application->setWarningActor([&warning](const std::string &value){
    warning.phrase(value);
  });
  timer->setCallback([&stack]{
    stack.application->timeout();
  });

  stack.session->connect();
  dispatcher.enter();
  stack.session->close();

  return 0;
}
