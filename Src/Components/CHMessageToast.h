/**
  * 消息提示框类
  * @author   	ch
  *
*/

#pragma once

#include <QWidget>

namespace Ui { class CHMessageToast; };

QT_BEGIN_NAMESPACE
class QTimer;
class QPropertyAnimation;
QT_END_NAMESPACE

class CHMessageToast : public QWidget
{
    Q_OBJECT

public:
    CHMessageToast(QWidget* parent = Q_NULLPTR);
    ~CHMessageToast();

    //设置显示持续时间
    void setShowDuration(int nMSecs);
    //获取显示持续时间
    int getShowDuration() const;

    //信息消息框
    static void information(QWidget* pParent, const QString& strText, int nShowDuration = 2000);
    //警告消息框
    static void warning(QWidget* pParent, const QString& strText, int nShowDuration = 2000);
    //错误消息框
    static void error(QWidget* pParent, const QString& strText, int nShowDuration = 2000);
    //成功消息框
    static void success(QWidget* pParent, const QString& strText, int nShowDuration = 2000);

protected:
    //窗口显示事件
    void showEvent(QShowEvent* event) Q_DECL_OVERRIDE;

private:
    //初始化窗口
    void initWidget();

    //显示消息提示框
    void showMessageToast(const QString& strIcon, const QString& strText, int nShowDuration = 2000);

    //获取消息框
    static CHMessageToast* getMessageToast(QWidget* pParent);

private Q_SLOTS:
    //开始淡入动画
    void startFadeInAnimation();
    //开始淡出动画
    void startFadeOutAnimation();

private:
    Ui::CHMessageToast* ui;

    int m_nShowDuration;                        //显示持续时间
    QTimer* m_pShowTimer;                       //显示定时器
    QPropertyAnimation* m_pShowAnimation;       //显示动画
};
