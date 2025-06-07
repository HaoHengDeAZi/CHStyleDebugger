/**
  * 工具首页
  * @author   	ch
  *
*/
#ifndef STYLEDEBUGGER_H
#define STYLEDEBUGGER_H

#include <QWidget>
#include <QPointer>
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

    void setFpsValue(int mainFps, int selectFps);

public Q_SLOTS:
    void toggleQssEditorVisible();
    void togglePropertyPanelVisible();
    void selectObject(bool alwaysSelect = false);

protected:
    void showEvent(QShowEvent *event) override;

private:
    void initWidget();
    void initSettings();

    void resetPropertyPanel(QWidget* parentWidget);

private:
    Ui::StyleDebugger *ui;

    bool m_hasMove;
    bool m_QssEditorHasMove;

    CQssEditor* m_pQssEditor;
    QPointer<CPropertyPanel> m_pPropertyPanel;
    CGlobalSelector* m_pGlobalSelector;

    QPointer<QSystemTrayIcon> m_trayIcon;
};

#endif // STYLEDEBUGGER_H
