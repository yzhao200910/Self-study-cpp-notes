# Qt 学习

# 1 认识Qt

## 1.1 从一个hello world程序开始

### 1.1.1 Qt Creator的基本使用

![1713053526687](./picture/1713053526687.png)

- Qt Widgets 支持桌面平台的有图形用户界面（GUI）的应用程序
- QT Console Appliaction 类似于终端那种小黑窗
- Qt Quick Apppliaction，创建可部署的QT Quick 2 应用程序，其页面设计采用QML语言，程序架构采用C++y语言，一般用于移动设备或嵌入式设备上无边框的应用程序设计
- Qt Canvas 3D Application 创建Qt Canvas 3D QML项目，也是基于QML语言的页面设计，支持3D画布

#### 1 Qt Creator布局初识

![1713055957517](./picture/1713055957517.png)

一个Qt可运行程序的编写包含以下：

- untitled（项目名称）文件：主要用于存放qmake（和cmake和makefile一样，用于构建项目）
- Headers 存放头文件
- Sources 存放源文件
- Froms存放UI文件，用来设计界面的UI
- other Files存放其他文件如语言的翻译文件

#### 2：UI文件（Design）界面分布与介绍

![11488fd99db3f888026adae804ca278](./picture/11488fd99db3f888026adae804ca278.jpg)

UI界面的设计就是将组件拖到对应的窗口，在设计页面时可以减轻代码的编写

#### 3：project的编译，调试与运行

注意事项,勾选了会产生不同其他编译器的版本（在设置tools勾选编译器）

![1713058440359](./picture/1713058440359.png)

**project基于qmake+编译工具，在当前目录下简历Release和Debug**，**可执行文件和.o文件都会在Debug文件下，就像makefile+bush脚本一样**（不勾选），按F5就可以直接编译+运行软件（make+./可执行文件）

一些图标

| 小电脑 | 列举出编译工具和编译模式如Debug或Release模式     |          |
| ------ | ------------------------------------------------ | -------- |
| run    | 直接运行程序，如果为编译就是编译+运行程序        | ctr + R  |
| Debug  | 类似于GDB，用于调试文件相当于g++ -g+./可执行文件 | F5       |
| 小锤子 | 相当于make命令，编译当前项目                     | ctrl + B |

# 2 GUI程序设计基础

## 2.1 UI文件设计与运行机制

## 2.1.1项目文件组成

一个qt项目包含以下文件（前面说过，再说一次）

- untitled（项目名称）文件：主要用于存放qmake（和cmake和makefile一样，用于构建项目）
- Headers 存放头文件
- Sources 存放源文件
- Froms存放UI文件，用来设计界面的UI
- other Files存放其他文件如语言的翻译文件

![1713062924693](./picture/1713062924693.png)

## 2.1.2项目管理文件

对应上文中的untitled（项目名称）文件

具体代码

```cmake
QT       += core gui#追加类库内容，
#检查版本
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
##类似 a = version >4 ? QT : QT+widgets

CONFIG += c++11 #使用c++11，类似选项-std=c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS #类似于-D

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \                #将依赖的源文件追加
    main.cpp \
    mainwindow.cpp

HEADERS += \  				 #将头文件追加
    mainwindow.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    untitled_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

```

## 2.1.3 界面文件

后缀为.ui的文件时可视化设计的窗体的定义文件。如上图的mainwindow.ui，通过Design来可视化设计页面，Design分布如下：

![11488fd99db3f888026adae804ca278](./picture/11488fd99db3f888026adae804ca278.jpg)

- 对象浏览器（Objects Inspector）也就是右上角的Object，用来记录maninwindow.ui对应mainwindow.cpp/.h中定义的类实例化出来对象的一些属性
- 属性编辑器（Property Editor），位于窗口右下方，用来编辑对象中元素中一些属性。，比如图中的hello world 就是对象的元素Qlab类变量的属性的值
- 最底端为Signals 和 Slots，为信号与信号槽，这是Qt中非常重要的概念，类似于回调函数

![1713224748444](./picture/1713224748444.png)

ui设计界面的布局一定要熟练，一些gui组件的布局属性的一些值可以直接通过ui来设置。

## 2.1.4  从一个简单的hello world代码入门Qt

**首先一个简单的qt程序包含的文件，上面提到过**

### **主函数文件**

```c++
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);//定义并创建应用程序
    MainWindow w; //定义并创建窗口
    w.show();//展示窗口
    return a.exec();//应用程序运行
}

```

- QApplication 是Qt的标准应用程序类，声明并创建程序a
- MainWindow继承Qt库中窗口类，实例化对象为w，使用w.show类函数来展示窗口
- 最后一行用a.exec()启动应用程序的执行，开始应用程序的消息循环和事件处理

### 窗体相关文件

头文件

```c++
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE//编译时来提示是否由最新的函数
namespace Ui { class MainWindow; } //ui界面的窗口类
//我们可以看到下面类中的MainWindow类成员有一个ui命名空间的 MainWindow
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{	//如果要使用信号槽与信号机制类必须添加
    Q_OBJECT//这个宏的展开有一大堆变量还有一些其他的宏

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
```

源文件

```c++
#include "mainwindow.h"
#include "ui_mainwindow.h"
//源文件没啥好说

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);//这个函数实现窗口的生成与各种属性的设置，信号与信号槽的关联
        //可以理解将一个程序的窗口的组件加载
}

MainWindow::~MainWindow()
{
    delete ui;
}

```

### ui_mainwindow.h文件

这个文件是对项目编译完成（是对头文件mainwindow.h文件编译完成）后产生，代码如下

```c++
/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    //组件的类，通过Qt design的右上方也可以看到这些类
    QWidget *centralwidget;
    QLabel *label;
    QPushButton *pushButton;
    QMenuBar *menubar;
    QMenu *menumy_first_qt;
    QStatusBar *statusbar;
	//MainWindow.cpp文件中ui->setupUi(this)this指向实例化的QMainWindow对象
    //我们读完这个段代码，发现这个函数类似于init()函数，将一些类对象的属性设置设置
    //再使用 retranslateUi函数（下文定义）来对窗口组件进行调整，比如窗口的名称等等
    //还有就是信号和槽的关联
    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1021, 775)//窗口的大小;
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(40, 230, 921, 201));
        QFont font;
        font.setPointSize(26);
        font.setUnderline(false);
        label->setFont(font);
        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(240, 430, 221, 91));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1021, 23));
        menumy_first_qt = new QMenu(menubar);
        menumy_first_qt->setObjectName(QString::fromUtf8("menumy_first_qt"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menumy_first_qt->menuAction());
		//
        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi
	//重新设置窗口组件和窗口属性的值
    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Hello world ,my first Qt program", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWindow", "close", nullptr));
#if QT_CONFIG(shortcut)
        pushButton->setShortcut(QString());
#endif // QT_CONFIG(shortcut)
        menumy_first_qt->setTitle(QCoreApplication::translate("MainWindow", "my first qt", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

```

# 2.2 可视化UI设计

前面我们发现通过Qt design操作一些组件是一种方式和ui_MainWindows.h文件相互对应，那么通过修改ui_MainWindows.h文件也可以做到Qt design的效果

## 2.2.1 实例化程序功能

进入Qt design界面

![11488fd99db3f888026adae804ca278](./picture/11488fd99db3f888026adae804ca278.jpg)

将左边的组件拖至中间即可