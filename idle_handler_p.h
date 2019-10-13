#ifndef IDLE_HANDLER_P_H
#define IDLE_HANDLER_P_H

#include <QTimer>

#include "idle_handler.h"

class IdleHandlerPrivate final : public QObject
{
  Q_OBJECT

private:
  explicit IdleHandlerPrivate(IdleHandler* owner);
  ~IdleHandlerPrivate() override;

private:
  bool eventFilter(QObject *object, QEvent *) override final;

private:
  IdleHandler *owner_;
  QObject *object_;
  QTimer timer_;

private slots:
  void onTimer();

  friend class IdleHandler;
};

#endif // IDLE_HANDLER_P_H
