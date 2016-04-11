/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#ifndef ACTIVEAPPLICATION_H
#define ACTIVEAPPLICATION_H

#include "TimerListener.h"
#include "PresentationListener.h"
#include "Application.h"
#include "../infrastructure/ActiveObject.h"

class ActiveApplication :
    public TimerListener,
    public PresentationListener
{
public:
  ActiveApplication(Application &application);

  void timeout() override;

  void luminosity(double value) override;

private:
  ActiveObject active;
  Application &application;

};

#endif