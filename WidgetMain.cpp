#include "WidgetMain.h"
#include "ui_WidgetMain.h"

WidgetMain::WidgetMain(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WidgetMain)
{
    ui->setupUi(this);

    serialPortInfo = QSharedPointer<SerialPortInfo>(new SerialPortInfo());
    serialPortInfo->SerialPortInit();
    dataInfo = QSharedPointer<DataInfo>(new DataInfo());
    dataInfo->DataInfoInit(serialPortInfo);
    parameterInfo = QSharedPointer<ParameterInfo>(new ParameterInfo());
    parameterInfo->ParameterInfoInit(dataInfo);
}

WidgetMain::~WidgetMain()
{
    delete ui;
}

