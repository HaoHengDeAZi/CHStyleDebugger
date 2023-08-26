/**
  * 基础对话框类
  * @author   	ch
  *
*/

#pragma once

#include <QDialog>

namespace Ui { class CHBaseDialog; }

class CHBaseDialog : public QDialog
{
    Q_OBJECT

public:
    CHBaseDialog(QWidget* parent = Q_NULLPTR);
    ~CHBaseDialog();

    //设置主窗口对象名称
    void setMainWidgetObjectName(const QString& strName);
    //设置主背景对象名称
    void setMainBackgroundObjectName(const QString& strName);
    //设置标题栏对象名称
    void setTitleBarObjectName(const QString& strName);
    //设置标题标签对象名称
    void setTitleLabelObjectName(const QString& strName);
    //设置功能区对象名称
    void setFunctionAreaObjectName(const QString& strName);
    //设置最小化按钮对象名称
    void setMinButtonObjectName(const QString& strName);
    //设置最大化按钮对象名称
    void setMaxButtonObjectName(const QString& strName);
    //设置关闭按钮对象名称
    void setCloseButtonObjectName(const QString& strName);

    //设置最大化样式表
    void setMaxStyleSheet(const QString& strStyleSheet);
    //设置还原样式表
    void setRestoreStyleSheet(const QString& strStyleSheet);

    //设置标题
    void setTitle(const QString& strTitle);
    //获取标题
    QString getTitle() const;

    //设置是否可见
    void setVisible(bool bVisible) Q_DECL_OVERRIDE;

    //设置是否可最小化
    void setMinimizable(bool bMinimizable);
    //获取是否可最小化
    bool isMinimizable() const;

    //设置是否可最大化
    void setMaximizable(bool bMaximizable);
    //获取是否可最大化
    bool isMaximizable() const;

    //设置是否可关闭
    void setClosable(bool bClosable);
    //获取是否可关闭
    bool isClosable() const;

    //设置是否可移动
    void setMovable(bool bMovable);
    //获取是否可移动
    bool isMovable() const;

    //设置是否可调整大小
    void setResizable(bool bResizable);
    //获取是否可调整大小
    bool isResizable() const;

    //设置调整大小宽度
    void setResizeWidth(int nWidth);
    //获取调整大小宽度
    int getResizeWidth() const;

    //设置是否显示阴影效果
    void setShadowEffectVisible(bool bVisible);
    //获取是否显示阴影效果
    bool isShadowEffectVisible() const;
    //获取阴影效果宽度
    int getShadowEffectWidth() const;

    //获取功能窗口
    QWidget* getFunctionWidget() const;
    //设置功能窗口大小
    void setFunctionWidgetSize(int nWidth, int nHeight);

    //视觉大小转为真实大小
    int realSizeFromVisualSize(int nSize);
    //设置固定的视觉大小
    void setFixedVisualSize(int nWidth, int nHeight);
    //设置最小的视觉大小
    void setMinimumVisualSize(int nWidth, int nHeight);

    //调整视觉大小
    void adjustVisualSize(int nWidth, int nHeight = -1);
    //调整小尺寸视觉大小
    void adjustMinVisualSize(int nHeight = -1);
    //调整中尺寸视觉大小
    void adjustMidVisualSize(int nHeight = -1);
    //调整大尺寸视觉大小
    void adjustMaxVisualSize(int nHeight = -1);

public Q_SLOTS:
    //最小化对话框
    virtual void minimizeDialog();
    //最大化对话框
    virtual void maximizeDialog();
    //关闭对话框
    virtual void closeDialog();

Q_SIGNALS:
    //开始
    void started();
    //隐藏
    void hidden();

protected:
    //调整位置
    void adjustPosition(QWidget* parent);
    //显示事件
    void showEvent(QShowEvent* event) Q_DECL_OVERRIDE;
    //关闭事件
    void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE;
    //按键事件
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
#ifdef Q_OS_WIN
    //本地事件
    bool nativeEvent(const QByteArray& eventType, void* message, long* result) Q_DECL_OVERRIDE;
#else
    //事件过滤器
    bool eventFilter(QObject* pWatched, QEvent* pEvent) Q_DECL_OVERRIDE;
#endif

    //改变事件
    void changeEvent(QEvent* event) Q_DECL_OVERRIDE;

private:
    //初始化标题栏
    void initTitleBar();

private:
    Ui::CHBaseDialog* ui;

    bool m_bMinimizable;                //是否可最小化
    bool m_bMaximizable;                //是否可最大化
    bool m_bClosable;                   //是否可关闭
    bool m_bMovable;                    //是否可移动
    bool m_bResizable;                  //是否可调整大小
    bool m_bShadowEffectVisible;        //是否显示阴影效果
    int m_nResizeWidth;                 //调整大小宽度

    QString m_strMaxStyleSheet;         //最大化样式表
    QString m_strRestoreStyleSheet;     //还原样式表
};
