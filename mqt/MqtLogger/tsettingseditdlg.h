#ifndef TSETTINGSEDITDLG_H
#define TSETTINGSEDITDLG_H

#include <QDialog>
#include <QModelIndex>

namespace Ui {
class TSettingsEditDlg;
}

class SettingsBundle;
class TSettingsEditDlg : public QDialog
{
    Q_OBJECT

    Ui::TSettingsEditDlg *ui;

    SettingsBundle *bundle;
    bool currSectionOnly;
    QString initialSection;
    void showSections();
    void showSection();
    void showDetails();
    void getDetails();
public:
    explicit TSettingsEditDlg(QWidget *parent , SettingsBundle *bundle);
    void ShowCurrentSectionOnly();
    ~TSettingsEditDlg();

    int exec() override;

private slots:
    void on_NewSectionButton_clicked();

    void on_CopyButton_clicked();

    void on_DeleteButton_clicked();

    void on_CancelButton_clicked();

    void on_OKButton_clicked();

    void on_SectionsList_clicked(const QModelIndex &index);
};
#endif // TSETTINGSEDITDLG_H
