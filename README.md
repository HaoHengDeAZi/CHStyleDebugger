# CHStyleDebugger

CHStyleDebugger是一个Qt Widgets实时样式修改和调试的工具，你可以很快的部署在Qt应用中。

## 1. 特性

1. 快速定位Qt对象和对象树关系，辅助代码方便定位对象。

2. 找到对象后可以跟Qt Desginer一样设置对象属性，并且实时查看结果。

3. 可以加载qss文件，通过内置qss编辑器或外部打开修改，实时修改并预览样式。

4. 内置编辑器支持qss的报错提醒。

5. 纯Qt接口，支持跨平台，理论上支持所有Qt支持的平台。

   目前暂未加入系统宏判断，以及编译器判断，可能出现乱码，后续会解决。

   暂未适配Qt6，后续解决。

   

## 2. 使用说明

由于Qt版本众多，本项目不提供编译库，有需要的自己下载源码进行编译。

### 2.1 编译项目

QtCreator打开pro文件，直接进行编译。Demo和插件会生成在Output目录中。

### 2.2 集成项目

在主窗口，或者main函数内增加如下代码

```c++
//1.头文件引入
#include <QtWidgets>
//如果不需要自定义样式转换函数不需要加入该头文件
#include "../Include/CHStyleDebuggerInterface.h"

//2.加载插件
bool loadPlugin()
{
    QDir pluginsDir(qApp->applicationDirPath());
    pluginsDir.cd("ChPlugins");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        //如果没有自定义样式转换就不需要以下的函数
        if (plugin)
        {
            CHStyleDebuggerInterface* styleInterface = qobject_cast<CHStyleDebuggerInterface *>(plugin);
            if (styleInterface != Q_NULLPTR)
            {
                styleInterface->setCustomStyleSheetFunction(test);
            }
        }
    }

    return false;
}

//找个地方加载插件
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    //加载插件
    loadPlugin();

    return a.exec();
}
```



### 2.3 使用说明

打开软件后，加载成功会在右下角出现图标。右上角出现面板。

![image-20230826101559481](.\Screens\1.png)



1. 选择对象

   Qt窗口获取焦点的情况下，通过Crtl+B（或者点击面板的“查找对象”），然后点击想查找的对象，出现菜单即可。

   ![image-20230826102618682](.\Screens\2.png)

2. 属性操作

   ![3](.\Screens\3.gif)

   查找对象->选择对象->实时修改属性并预览。

   支持生成样例代码，方便拷贝到代码中操作。

3.  美化面板

   a) 加载qss文件

   设置->美化面板设置->加入qss文件->**按照项目调整qss加载顺序**->保存

![4](.\Screens\4.png)

![image-20230826104307158](.\Screens\5.png)

b) 使用美化面板

![6](.\Screens\6.gif)



## 规划：

1. 兼容不同系统和不同Qt版本。
2. 增加自定义qss解析器，类似less.js的转换器，支持自定义颜色变量，支持展示自定义颜色。