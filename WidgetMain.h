#ifndef WIDGETMAIN_H
#define WIDGETMAIN_H

#include "Headers.h"
#include "SerialPortInfo.h"
#include "DataInfo.h"
#include "ParameterInfo.h"

QT_BEGIN_NAMESPACE
namespace Ui { class WidgetMain; }
QT_END_NAMESPACE

class WidgetMain : public QWidget
{
    Q_OBJECT

public:
    WidgetMain(QWidget *parent = nullptr);
    ~WidgetMain();

private:
    Ui::WidgetMain *ui;
    QSharedPointer<SerialPortInfo> serialPortInfo;
    QSharedPointer<DataInfo> dataInfo;
    QSharedPointer<ParameterInfo> parameterInfo;
};
#endif // WIDGETMAIN_H
