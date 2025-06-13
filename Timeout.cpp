#include "Timeout.h"

Timeout::Timeout()
  : _time_ms(0)
  , _reset_time_ms(0)
  , _time_over_forced(true)
  , _paused(false) 
  {}

void Timeout::start(uint32_t time_ms){
  _paused = false;
  _time_ms = time_ms;
  if (time_ms == 0) {
      _time_over_forced = true;
  } else {
      _reset_time_ms = millis();
      _time_over_forced = false;
  }
}

bool Timeout::periodic(uint32_t time_ms) {
  bool result = time_over();
  if (result)
      start(time_ms);
  return result;
}

void Timeout::pause(void) {
  _time_ms = time_left_ms();
  _paused = true;
}

void Timeout::resume(void) {
  if (_paused) {
      start(_time_ms);
  }
}

void Timeout::expire(void) {
  _time_over_forced = true;
  _paused = false;
}

bool Timeout::time_over(void)
{
  if (_paused) {
      return false;
  }
  bool result = (_time_over_forced || (uint32_t)(millis() - _reset_time_ms) >= _time_ms);
  if (result) {
      // make sure to stay expired and not roll over again
      _time_over_forced = true;
  }
  return result;
}

bool Timeout::is_paused(void)
{
  return _paused;
}

uint32_t Timeout::time_left_ms(void)
{
  if (_paused) {
      return _time_ms;
  }
  if (time_over()) {
      return 0;
  }
  return _time_ms - (uint32_t)(millis() - _reset_time_ms);
}