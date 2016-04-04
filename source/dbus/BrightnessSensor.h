/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#ifndef BRIGHTNESSSENSOR_H
#define BRIGHTNESSSENSOR_H

#include "StreetlightProxy.h"

#include <application/Sensor.h>

#include <dbus-c++/dbus.h>

class BrightnessSensor :
    public ch::bbv::brightness_proxy,
    public DBus::IntrospectableProxy,
    public DBus::ObjectProxy,
    public Sensor
{
public:
  BrightnessSensor(DBus::Connection& connection);

  double get() override;

};

#endif
