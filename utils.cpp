#include "utils.h"

#include <QSettings>
#include <QWidget>
#include <QTreeView>
#include <QHeaderView>
#include <QSplitter>
#include <QTabWidget>

namespace Utils {

void saveElementsState(QWidget *parent, QSettings &settings)
{
  for (QTreeView *tree : parent->findChildren<QTreeView*>()) {
    settings.setValue(tree->objectName() + "-column-widths", tree->header()->saveState());
  }

  for (QSplitter *splitter : parent->findChildren<QSplitter*>()) {
    settings.setValue(splitter->objectName() + "-splitter-state", splitter->saveState());
  }

  for (QTabWidget *tab : parent->findChildren<QTabWidget*>()) {
    QWidget *curent_widget = tab->currentWidget();
    settings.setValue(tab->objectName() + "-current-tab", curent_widget ? curent_widget->objectName() : QString());
  }
}

void restoreElementsState(QWidget *parent, QSettings &settings)
{
  for (QTreeView *tree : parent->findChildren<QTreeView*>()) {
    tree->header()->restoreState(settings.value(tree->objectName() + "-column-widths").toByteArray());
  }

  for (QSplitter *splitter : parent->findChildren<QSplitter*>()) {
    splitter->restoreState(settings.value(splitter->objectName() + "-splitter-state").toByteArray());
  }

  for (QTabWidget *tab : parent->findChildren<QTabWidget*>()) {
    QString curent_tab_name = settings.value(tab->objectName() + "-current-tab").toString();
    if (curent_tab_name.isEmpty()) {
      continue;
    }
    QWidget *curent_tab{parent->findChild<QWidget*>(curent_tab_name)};
    if (nullptr != curent_tab) {
      tab->setCurrentWidget( curent_tab );
    }
  }
}

} // namespace Utils
