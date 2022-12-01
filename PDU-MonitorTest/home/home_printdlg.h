#ifndef HOME_PRINTDLG_H
#define HOME_PRINTDLG_H

#include <QDialog>
#include "common.h"
#include "common/printer/printer_bartender.h"


namespace Ui {
class Home_PrintDlg;
}

class Home_PrintDlg : public QDialog
{
    Q_OBJECT

public:
    explicit Home_PrintDlg(QWidget *parent = nullptr);
    ~Home_PrintDlg();

protected slots:
    void initFunSlot();

private slots:
    void on_testPrintBtn_clicked();

    void on_saveBtn_clicked();

    void on_closeBtn_clicked();

private:
    Ui::Home_PrintDlg *ui;
    sCfgItem *mItem;
};

#endif // HOME_PRINTDLG_H
