/*
  This file is part of the ArduinoRS485 library.
  Copyright (c) 2018 Arduino SA. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "RS485.h"

RS485Class::RS485Class(Stream &stream, int txPin, int dePin, int rePin) : _stream(&stream),
                                                                          _txPin(txPin),
                                                                          _dePin(dePin),
                                                                          _rePin(rePin),
                                                                          _transmissionBegun(false)
{
}

void RS485Class::begin(unsigned long _baudrate)
{
  begin(0, 0, RS485_DEFAULT_PRE_DELAY, RS485_DEFAULT_POST_DELAY);
}

void RS485Class::begin(unsigned long _baudrate, int predelay, int postdelay)
{
  begin(0, 0, predelay, postdelay);
}

void RS485Class::begin(unsigned long _baudrate, uint16_t _config)
{
  begin(0, 0, RS485_DEFAULT_PRE_DELAY, RS485_DEFAULT_POST_DELAY);
}

void RS485Class::begin(unsigned long _baudrate, uint16_t _config, int predelay, int postdelay)
{
  // Set only if not already initialized with ::setDelays
  _predelay = _predelay == 0 ? predelay : _predelay;
  _postdelay = _postdelay == 0 ? postdelay : _postdelay;

  if (_dePin > -1)
  {
    pinMode(_dePin, OUTPUT);
    digitalWrite(_dePin, LOW);
  }

  if (_rePin > -1)
  {
    pinMode(_rePin, OUTPUT);
    digitalWrite(_rePin, HIGH);
  }

  _transmissionBegun = false;
}

void RS485Class::end()
{
  if (_rePin > -1)
  {
    digitalWrite(_rePin, LOW);
    pinMode(_dePin, INPUT);
  }

  if (_dePin > -1)
  {
    digitalWrite(_dePin, LOW);
    pinMode(_rePin, INPUT);
  }
}

int RS485Class::available()
{
  return _stream->available();
}

int RS485Class::peek()
{
  return _stream->peek();
}

int RS485Class::read(void)
{
  return _stream->read();
}

void RS485Class::flush()
{
  return _stream->flush();
}

size_t RS485Class::write(uint8_t b)
{
  if (!_transmissionBegun)
  {
    setWriteError();
    return 0;
  }

  return _stream->write(b);
}

RS485Class::operator bool()
{
  return true;
}

void RS485Class::beginTransmission()
{
  if (_dePin > -1)
  {
    digitalWrite(_dePin, HIGH);
    if (_predelay)
      delayMicroseconds(_predelay);
  }

  _transmissionBegun = true;
}

void RS485Class::endTransmission()
{
  _stream->flush();

  if (_dePin > -1)
  {
    if (_postdelay)
      delayMicroseconds(_postdelay);
    digitalWrite(_dePin, LOW);
  }

  _transmissionBegun = false;
}

void RS485Class::receive()
{
  if (_rePin > -1)
  {
    digitalWrite(_rePin, LOW);
  }
}

void RS485Class::noReceive()
{
  if (_rePin > -1)
  {
    digitalWrite(_rePin, HIGH);
  }
}

void RS485Class::sendBreak(unsigned int duration)
{
  _stream->flush();
  pinMode(_txPin, OUTPUT);
  digitalWrite(_txPin, LOW);
  delay(duration);
}

void RS485Class::sendBreakMicroseconds(unsigned int duration)
{
  _stream->flush();
  pinMode(_txPin, OUTPUT);
  digitalWrite(_txPin, LOW);
  delayMicroseconds(duration);
}

void RS485Class::setPins(int txPin, int dePin, int rePin)
{
  _txPin = txPin;
  _dePin = dePin;
  _rePin = rePin;
}

void RS485Class::setDelays(int predelay, int postdelay)
{
  _predelay = predelay;
  _postdelay = postdelay;
}
