#ifndef UTILS_H
#define UTILS_H

class QWidget;
class QSettings;

namespace Utils {

void saveElementsState(QWidget *parent, QSettings &settings);
void restoreElementsState(QWidget *parent, QSettings &settings);

} // namespace Utils

#endif // UTILS_H
