#include "controller.h"
#include "ui_controller.h"
#include <QIntValidator>

Controller::Controller(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Controller)
{
    ui->setupUi(this);

    // Validator for text input
    QIntValidator *limiter = new QIntValidator(0,1000);
    ui->HLineEdit->setValidator(limiter);
    ui->VLineEdit->setValidator(limiter);
}

Controller::~Controller()
{
    delete ui;
}

