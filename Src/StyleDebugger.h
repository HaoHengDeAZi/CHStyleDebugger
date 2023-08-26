/**
  * 工具首页
  * @author   	ch
  *
*/
#ifndef STYLEDEBUGGER_H
#define STYLEDEBUGGER_H

#include <QWidget>
#include "Components/CHBaseDialog.h"

class QSystemTrayIcon;

namespace Ui {
class StyleDebugger;
}

class CQssEditor;
class CPropertyPanel;
class CGlobalSelector;

class StyleDebugger : public CHBaseDialog
{
    Q_OBJECT

public:
    StyleDebugger(QWidget *parent = nullptr);
    ~StyleDebugger();

public Q_SLOTS:
    void toggleQssEditorVisible();
    void togglePropertyPanelVisible();
    void selectObject(bool alwaysSelect = false);

protected:
    void showEvent(QShowEvent *event) override;

private:
    void initWidget();
    void initSettings();

private:
    Ui::StyleDebugger *ui;

    bool m_hasMove;
    bool m_QssEditorHasMove;

    CQssEditor* m_pQssEditor;
    CPropertyPanel* m_pPropertyPanel;
    CGlobalSelector* m_pGlobalSelector;

    QSystemTrayIcon* m_trayIcon;
};

#endif // STYLEDEBUGGER_H
