#ifndef IDLEHANDLER_H
#define IDLEHANDLER_H

#include <QObject>

class IdleHandlerPrivate;
class IdleHandler final : public QObject
{
  Q_OBJECT
public:
  explicit IdleHandler(QObject *parent);
  ~IdleHandler() override;

signals:
  /** @brief Parents goes into idle state.
   *
   * Signal is emitted when message queue of parent becomes empty.
   *
   * \warning Do not connect this signal to slot of parent object (i.e. object,
   * passed as a parent to constructor of \ref GIdleHandler) using
   * \ref Qt::QueuedConnection or Qt::BlockingQueuedConnection, because this
   * may result of continuous emission of signal. Signal already is emitted
   * asynchronously.
   */
  void idle();

public slots:

private:
  IdleHandlerPrivate *d;
};

#endif // IDLEHANDLER_H
