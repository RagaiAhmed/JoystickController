#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class Controller; }
QT_END_NAMESPACE

class Controller : public QMainWindow
{
    Q_OBJECT

public:
    Controller(QWidget *parent = nullptr);
    ~Controller();

private:
    Ui::Controller *ui;
};
#endif // CONTROLLER_H
