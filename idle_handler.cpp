#include "idle_handler.h"

#include "idle_handler_p.h"

// ///////////////////////////////////////////////////////////////////////////
IdleHandlerPrivate::IdleHandlerPrivate(IdleHandler* owner) :
  owner_{owner},
  object_{owner->parent()},
  timer_{this}
{
  if (nullptr == object_) {
    qDebug("GIdleHandler: no QObject specified.");
    return;
  }

  object_->installEventFilter(this);

  connect(&timer_, SIGNAL(timeout()), SLOT(onTimer()), Qt::QueuedConnection);

  timer_.setSingleShot(true);
  timer_.start(0);
}

IdleHandlerPrivate::~IdleHandlerPrivate()
{
}

void IdleHandlerPrivate::onTimer()
{
  emit
    owner_->idle();
}

bool IdleHandlerPrivate::eventFilter(QObject *object, QEvent*)
{
  if (object != object_) {
    return false;
  }

  if (!timer_.isActive()) {
    timer_.start(0);
  }

  return false; // We are just watching, not catching!
}

// ///////////////////////////////////////////////////////////////////////////
IdleHandler::IdleHandler(QObject *parent) :
  QObject{parent},
  d{new IdleHandlerPrivate{this}}
{
}

IdleHandler::~IdleHandler()
{
  delete d;
}
