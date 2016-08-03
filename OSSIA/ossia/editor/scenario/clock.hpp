#pragma once

#include <ossia/editor/scenario/time_value.hpp>
#include <chrono>
#include <functional>
#include <memory>
#include <ossia_export.h>

namespace ossia
{
class OSSIA_EXPORT clock
{
public:
  /*! to get the clock execution back
   \param clock position
   \param clock date
   \param dropped ticks */
  using ExecutionCallback
      = std::function<void(time_value, time_value, unsigned char)>;

  enum ClockExecutionStatus
  {
    RUNNING,
    STOPPED
  };
  using ExecutionStatusCallback = std::function<void(ClockExecutionStatus)>;

  /*! how the time flows for the clock */
  enum class DriveMode
  {
    INTERNAL, // the tick method is called by the clock itself
    EXTERNAL  // the tick method is called from outside the clock
  };

  /*! factory
   \param #ExecutionCallback
   \param duration
   \param granularity
   \param offset
   \param speed
   \param drive mode
   \return std::shared_ptr<#Clock> */
  static std::shared_ptr<clock> create(
      clock::ExecutionCallback, time_value = Infinite, time_value = 10.,
      time_value = 0., float = 1., DriveMode = clock::DriveMode::INTERNAL);

  /*! destructor */
  virtual ~clock();

  /*! start the clock */
  virtual void start() = 0;

  /*! halt the clock */
  virtual void stop() = 0;

  /*! pause the clock progression */
  virtual void pause() = 0;

  /*! true if the constraint is running and paused, else false */
  virtual bool paused() const = 0;

  /*! resume the clock progression */
  virtual void resume() = 0;

  /*! called every time a new step should be processed.
   \details can be use to force step processing in case of external drive but
   the callback will not be called
   \return bool true if the clock ticks */
  virtual bool tick() = 0;

  /*! can be called by hand to advance the clock from a certain number of
   * ticks.
   * \param usec Number of microseconds to move the clock
   * \return bool true if the clock ticks
   */
  virtual bool tick(time_value usec) = 0;

  /*! get the duration of the clock
   \return const #TimeValue duration */
  virtual const time_value& getDuration() const = 0;

  /*! set the duration of the clock execution
   \param const #TimeValue duration
   \return #Clock the clock */
  virtual clock& setDuration(time_value) = 0;

  /*! get the offset of the clock
   \return const #TimeValue offset */
  virtual const time_value& getOffset() const = 0;

  /** set the offset of the clock
   \param const #TimeValue offset
   \return #Clock the clock */
  virtual clock& setOffset(time_value) = 0;

  /*! get the granularity of the clock
   \return const #TimeValue granularity */
  virtual const time_value& getGranularity() const = 0;

  /*! set the granularity of the clock execution
   \param const #TimeValue granularity
   \return #Clock the clock */
  virtual clock& setGranularity(time_value) = 0;

  /*! get the speed of the clock
   \return const #TimeValue speed */
  virtual double getSpeed() const = 0;

  /** set the speed factor attribute
   \param double speed factor
   \return #Clock the clock */
  virtual clock& setSpeed(double) = 0;

  /*! get the clock drive mode
   \return #DriveMode */
  virtual DriveMode getDriveMode() const = 0;

  /** set is the clock drive mode
   \param #DriveMode
   \return #Clock the clock */
  virtual clock& setDriveMode(DriveMode) = 0;

  /*! get the running status of the clock
   \return bool true if is running */
  virtual bool getRunning() const = 0;

  /*! get the position of the clock
   \return const #TimeValue position */
  virtual const time_value& getPosition() const = 0;

  /*! get the date of the clock
   \return const #TimeValue date */
  virtual const time_value& getDate() const = 0;

  // Execution status will be called when the clock starts and stops.
  virtual void setExecutionStatusCallback(ExecutionStatusCallback) = 0;
  virtual ExecutionStatusCallback getExecutionStatusCallback() const = 0;
};
}