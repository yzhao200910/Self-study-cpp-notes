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

# 3 qt窗口类和代理类的介绍

Qt 中的几个关键基类：`QWidget`, `QMainWindow`, `QDialog`, 和 `QFrame`

## 3.1 QWidget

**QWidget**：

**特点**：

- `QWidget` 是所有用户界面对象的基类。
- **任何可以被显示的对象**都是 `QWidget` 的子类。
- 它提供了基本的用户界面功能，可以包含其他 `QWidget` 对象。

**功能**：

- 它可以单独使用，也可以作为其他复杂控件的容器。
- 支持事件处理，如鼠标点击、键盘输入等。
- 可以设置大小、位置、可视/隐藏状态等。

**使用场景：**

- 创建自定义的用户界面控件。
- 作为其他控件的容器。

## 3.2 **QMainWindow**：

**特点**：

- `QMainWindow` 是专为主窗口设计的类，提供了一个框架，使得开发者可以添加工具栏、状态栏、以及其他常见的元素。
- **它是 `QWidget` 的子类，但是专为主窗口应用程序设计**。
- 允许使用停靠窗口（dockable windows）和工具栏。

**功能：**

- 自动管理菜单栏、工具栏、状态栏和中心窗口。
- 支持添加停靠窗口（dock widgets）和工具条。
- 可以很容易地通过拖放来组织工具栏和停靠窗口的布局。

**使用场景**：

- 构建主窗口应用程序。
- 需要一个包含常用GUI组件的应用程序窗口。

## 3.3 QDialog

#### **特点：**

- `QDialog` 是专为对话框设计的基类。
- 通常用于执行短期任务或与用户交互。
- 支持模态和非模态对话框。

#### **功能**：

- 支持模态（阻塞）和非模态（非阻塞）对话框。

  - ```c++
    //模态实现
    QDialog dlg(this);
    dlg.exec(); // 阻塞在此
    //第二种实现
    QDialog* dlg = new QDialog(this);
    dlg->setModel(true); // 设置模态
    dlg->show();
    ```
  - ```c++
    //非模态
    QDialog* dlg = new QDialog(this);
    ```

- 可以有返回值，通常用于表示用户的动作（如接受或拒绝）。
- 支持添加标准按钮和自定义的界面元素。
- ```c++
  //窗口置顶
  MainWindow::MainWindow(QWidget *parent) :
      QMainWindow(parent),
      ui(new Ui::MainWindow)
  {
      ui->setupUi(this);
      auto s = new QDialog(this);
      s->setWindowFlag(Qt::WindowStaysOnTopHint);
      s->show();
  }
  ```

#### **使用场景**：

- 需要从用户那里获取决定或额外信息。
- 创建设置选项、文件选择对话框等。

#### **常用的对话框**

对话框的弹出一般作为槽，接受信号，做出一定的应对

- ```c++
  //颜色对话框
  void MainWindow::on_pushButton_clicked()
  {
  //    QColorDialog colorDlg(Qt::blue, this);
  //    colorDlg.setOption(QColorDialog::ShowAlphaChannel);
  //    colorDlg.exec();
  //    QColor color = colorDlg.currentColor();
  //    qDebug() << "color is " << color;
     QColor color = QColorDialog::getColor(Qt::blue, this,tr("选择颜色"), QColorDialog::ShowAlphaChannel );//创建颜色对话框并且返回选择的颜色
      qDebug() << "color is " << color;
  }
  ```

- ```c++
  //文本对话框
  //创建一个选择文件的对话框，选择后，可以根据返回值打印选择的文件的路径信息
  void MainWindow::on_pushButton_2_clicked()
  {
      QString path = QDir::currentPath();//初始化默认当前的路径
      QString title = tr("文件对话框");//选择窗口的额名称
      QString filter = tr("文本文件(*.txt);;图片文件(*.jpg *.gif *.png);;所有文件(*.*)");//选择可匹配的文件类型
      //创建窗口并且返回选择文件的绝对路径名称
      QString aFileName=QFileDialog::getOpenFileName(this,title,path,filter);
  
      qDebug() << aFileName << endl;
  }
  ```

- ```c++
  //输出对话框
  //输入对话框分几种，包括文本输入对话框，整数输入对话框，浮点数输入对话框，条目输入对话框。 先看看文本输入对话框
  void MainWindow::on_pushButton_3_clicked()
  {
      bool ok = false;
      auto text = QInputDialog::getText(this, tr("文字输入对话框"), tr("请输入用户的姓名"), QLineEdit::Normal, tr("admin"), &ok); //可以有返回值，通常用于表示用户的动作（如接受或拒绝）
      //这里只是改变ok的值，来代替返回的用户动作
      if(ok){
          qDebug() << text << endl;
      }
  }
  
  //整形输入的对话框
  void MainWindow::on_pushButton_4_clicked()
  {
       bool ok = false;
      //参数解释，标题，选项，默认值，最小最大值，步长，与选择结果
      auto intdata = QInputDialog::getInt(this,tr("数字输入对话框"),tr("请输入数字"),200,-200,400,10,&ok);
      if(ok){
          qDebug() << intdata << endl;
      }
  }
  
  //浮点输入对话框
  void MainWindow::on_pushButton_5_clicked()
  {
      bool ok = false;
      auto floatdata = QInputDialog::getDouble(this,tr("浮点数输入对话框"),tr("输入浮点数"),0.1,-1,1,2,&ok);
      if(ok){
          qDebug() << floatdata << endl;
      }
  }
  //条目输入对话框，也就是一个item可以被选择，提供多个选择
  
  void MainWindow::on_pushButton_6_clicked()
  {
      QStringList items;
      items << tr("条目1") << tr("条目2");
      bool ok = false;
      auto itemData = QInputDialog::getItem(this,tr("条目输入对话框"),tr("输入或选择条目"),items,0,true, &ok);
      if(ok){
          qDebug() << "item is " << itemData << endl;
      }
  }
  
  //提示对话框
  //返回值作为选择的结果，用于提示的判断
  void MainWindow::on_pushButton_7_clicked()
  {
      auto ret = QMessageBox::question(this,tr("提问对话框"),tr("你是单身吗"),QMessageBox::Yes, QMessageBox::No);
      if(ret == QMessageBox::Yes || ret == QMessageBox::No){
          qDebug() << "ret is " << ret << endl;
      }
  
      auto ret2 = QMessageBox::information(this,tr("通知对话框"),tr("你好单身狗"),QMessageBox::Ok);
      if(ret2 == QMessageBox::Ok){
          qDebug() << "ret2 is " << ret2 << endl;
      }
  
      auto ret3 = QMessageBox::warning(this,tr("警告对话框"),tr("你最好找个地方发泄一下"),QMessageBox::Ok);
      if(ret3 == QMessageBox::Ok){
          qDebug() << "ret3 is " << ret3 << endl;
      }
  
      auto ret4 = QMessageBox::critical(this,tr("关键提示对话框"), tr("我梦寐以求是真爱和自由"),QMessageBox::Ok);
      if(ret4 == QMessageBox::Ok){
          qDebug() << "ret4 is " << ret4 << endl;
      }
  }
  ```

- ```c++
  //进度对话框 + 定时器， 利用定时器设置定时时间发送信号配合
  void MainWindow::on_pushButton_8_clicked()
  {
      //改用定时器
      _progressDialog = new QProgressDialog(tr("正在复制"),tr("取消复制"),0,5000,this);
      _progressDialog->setWindowTitle(tr("文件复制进度对话框"));
      _progressDialog->setWindowModality(Qt::ApplicationModal);//设置模态的优先级，堵塞所有的窗口
      _timer = new QTimer(this);
      connect(_timer, &QTimer::timeout, this, &MainWindow::on_updateProgressDialog);
      connect(_progressDialog, &QProgressDialog::canceled, this, &MainWindow::on_cancelProgressDialog);
      _timer->start(2);
      qDebug("复制结束");
  }
  //进度更新槽函数
  void MainWindow::on_updateProgressDialog()
  {
      _count++;
      if(_count > 5000){
          _timer->stop();
          delete  _timer;
          _timer = nullptr;
          delete _progressDialog;
          _progressDialog = nullptr;
          _count = 0;
          return;
      }
  	 _progressDialog->setValue(_count);
  }
  ```

- ```c++
  //向导对话框,安装的向导框
  void MainWindow::on_pushButton_10_clicked()
  {
      QWizard wizard(this);
      wizard.setWindowTitle(tr("全城热恋"));//设置向导框标题
      QWizardPage* page1 = new QWizardPage();//创建向导框的第一页
      page1->setTitle(tr("婚恋介绍引导程序"));//第一页的标题
      auto label1 = new QLabel();
      label1->setText(tr("该程序帮助您找到人生伴侣"));
      QVBoxLayout *layout = new QVBoxLayout();//垂直布局
      layout->addWidget(label1);//添加label
      page1->setLayout(layout);//为page1设置布局和添加组件
      wizard.addPage(page1);//将page1添加进向导对话框中
      QWizardPage* page2 = new QWizardPage();
      page2->setTitle("选择心动类型");
  
      QButtonGroup *group = new QButtonGroup(page2);
      QRadioButton * btn1 = new QRadioButton();
      btn1->setText("白富美");
      group->addButton(btn1);
      QRadioButton * btn2 = new QRadioButton();
      btn2->setText("萝莉");
      group->addButton(btn2);
      QRadioButton * btn3 = new QRadioButton();
      btn3->setText("御姐");
      group->addButton(btn3);
      QRadioButton * btn4 = new QRadioButton();
      btn4->setText("小家碧玉");
      group->addButton(btn4);
      QRadioButton * btn5 = new QRadioButton();
      btn5->setText("女汉子");
      group->addButton(btn5);
  
      QRadioButton * btn6 = new QRadioButton();
      btn6->setText("成年人不做选择，全选!");
      group->addButton(btn6);
      QVBoxLayout *vboxLayout2 = new QVBoxLayout();
      for(int i = 0; i < group->buttons().size(); i++){
          vboxLayout2->addWidget(group->buttons()[i]);
      }
  
      page2->setLayout(vboxLayout2);
      wizard.addPage(page2);
  
      QWizardPage* page3 = new QWizardPage();
      page3->setTitle(tr("你的缘分即将到来"));
      auto label3 = new QLabel();
      label3->setText(tr("感谢您的参与，接下来的一个月会遇到对的人"));
      QVBoxLayout *layout3 = new QVBoxLayout();
      layout3->addWidget(label3);
      page3->setLayout(layout3);
      wizard.addPage(page3);
      wizard.show();
      wizard.exec();
  } 
  ```

  

## 3.4 Qframe

**特点：**

- `QFrame` 是一个用于包含其他 `QWidget` 对象的框架或边框。
- 它提供了可以绘制各种样式的边框，并可以作为组织其他 `QWidget` 的容器使用。

**功能**：

- 提供了绘制边框的功能，可以选择不同的样式（如凹陷、凸出等）。
- 继承自 `QWidget`，因此可以包含其他控件。
- 用于组织界面布局，通过视觉上的分隔来增强用户体验。

**使用场景**：

- 组织界面布局，通过边框清晰地分割不同区域。
- 用作其他控件的背景框架，提供视觉上的边界和组织。

## 3.5 代理类（delegate class）

### 1： 介绍

关于MVC架构会在后面仔细说

在Qt中，代理类（delegate class）在MVC（Model-View-Controller）架构中扮演着重要的角色。其主要目的是**用于定制和控制在视图（View）中如何显示和编辑数据模型（Model）中的数据**

### 2： 意义与作用

理解： 一些组件，在Qt原来的库中，就已经定义好了，但是，我们不想按部就班，想自己弄一些灵活的组件和布局，于是出现了代理类（delegate class）

1. **定制显示**：**代理类允许开发者自定义数据在视图中的显示方式**。默认情况下，视图使用内置的显示方式，但通过自定义代理类，可以实现更复杂或特定需求的显示逻辑
2. **定制编辑**：通过代理类，**开发者可以指定某一列的数据使用特定的编辑控件**，例如将布尔值显示为复选框，日期使用日期选择器等。
3. **提高代码的可维护性**：**使用代理类将数据的显示和编辑逻辑与模型和视图分离开来，使代码更加模块化和可维护**
4. **增强视图的功能**：代理类不仅可以自定义单个单元格的显示和编辑，还可以在需要时为整个视图增加复杂的交互功能
5. **提供一致的用户体验**

### 3： 使用

```c++
/**Qt中常用的代理类是QStyledItemDelegate，这是一个通用的代理类，适用于大多数场景。自定义代理类通常需要继承QStyledItemDelegate并重写其一些关键方法，如paint和createEditor等。例如：
**/
class CustomDelegate : public QStyledItemDelegate {
    Q_OBJECT // 使用元对象系统

public:
    CustomDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        // 自定义绘制逻辑
    }

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        // 返回自定义的编辑控件
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const override {
        // 从模型数据中提取值并设置到编辑控件中
    }

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override {
        // 从编辑控件中获取值并设置到模型中
    }
};


```



# 4 QT类库的概述

Qt使用cpp纯手搓出来的并且扩展，引入了一些新的概念比如**信号与槽** 元对象等等

**Qt Core模块是Qt类库的核心**，所有其他模块都依赖此模块。

**Qt为C++语言增加了新的特性就是在Qt Core模块中实现的**，这些扩展特性由Qt的元对象系统实现的，包括信号与槽机制，属性系统，动态类型转换。

## 4.1 元对象系统

Qt的元对象系统（meta-object-syste）提供了对象之间通信的信号与槽机制，运行时类型信息和动态属性系统。（类似标准库中的dynamic_cats一样，运行时确定）

### 4.1.1**元对象系统由三个基础组成**

- QObject类是所有使用元对象系统类的基类
- 在一个类的private部分声明Q_OBJECT宏，使得类可以使用元对象的特性，如动态属性，信号槽
- MOC(元对象编译器)为每个QObject的子类提供必要的代码来实现元对象系统的特性。

**MOC编译器的预处理阶段是将代表元对象的宏展开**，**也就是一个Cpp源文件在预处理前先用MOC编译器先进性预处理**，针对QT部分，将它预处理成额外的Cpp代码，然后这样在接下来就是常规的c++编译过程，预处理，编译，汇编和连接，最后形成可执行文件

所有元对象的基类都是QObject类，它的提供了一些函数：

- QObject::metaObject()函数返回类关联的元对象(动态时确定对象类型，类似于typeid)

  - ```c++
    QObject* obj = new QPushButton;//多态的使用
    obj->metaObject()->className();//QPushButton
    ```

- QObject::inherits（）,用于查看是否是子类关系

- QObject::tr()和QObject::trUtf8函数可翻译字符串，用于多语言界面设计

- QObject::setProperty()和QObject::Property()函数用于通过属性的名称动态的设置和获取属性值

- 对于QObject以及其子类,可以通过qobject_cast（dynamic_cast）进行切换，投影，必须是基础关系

**元对象都是继承于QObject**，**它们继承基类的属性和方法**，**QObject所带来的属性和方法组成了元对象系统**，用于后续的开发，可以说是整个QT开发的基础

## 4.2 属性系统

### 1.属性的定义

**Qt提供了一个QT_PROPERTY()宏可以定义属性**，他也是基于元对象系统实现的。Qt的属性系统与C++编译器无关，可以用任何标准的C++编译器编译定义了属性的Qt C++程序

**宏的格式如下：**

![db78ab1cadc3b1265eb13ab3456ea9c](./picture/db78ab1cadc3b1265eb13ab3456ea9c.jpg)

**QT_PROPERTY宏定义属性的一些主要关键字的以下如下**：

- READ指定一个读取属性值的函数，没有MEMBER关键字时必须设置READ。
- WRITE指定一个设定属性值的函数，只读属性没有WRITE设置
- MEMBER指定一个成员变量与属性关联，成为可读可写的属性，无需设置READ和WRITE
- RESET是可选的，用于指定一个设置属性缺省值的函数
- NOTIFY是可选的，用于设置一个信号，当属性值变化时发送此信号
- DESIGNABLE表示属性是否在Qt Designer里可见，缺省为true
- CONSTANT表示属性值是一个常数

![ec42dcc04ebab0079a88cb4a4b8b370](./picture/ec42dcc04ebab0079a88cb4a4b8b370.jpg)

**QT_PROPERTY这个宏是定义在类里面的，宏的展开，一般都是一些函数对类对象的操作或者与这个类对象有所关系的的变量**，**关于属性，无论是否有这个宏都是可以通过QObject::setProperty()和QObject::Property()函数来设置和读取**

### 2.属性的使用

```c++
class MyClass : public QObject {
    Q_OBJECT
    Q_PROPERTY(int myNumber READ getMyNumber WRITE setMyNumber NOTIFY myNumberChanged)

public:
    int getMyNumber() const { return m_myNumber; }
    void setMyNumber(int number) {
        if (m_myNumber != number) {
            m_myNumber = number;
            emit myNumberChanged();
        }
    }

signals:
    void myNumberChanged();

private:
    int m_myNumber;
};

```

```c++
MyClass obj;
obj.setProperty("myNumber", 42); // 设置属性
int value = obj.property("myNumber").toInt(); // 获取属性

```

### 3.类的附加信息

属性系统还有一个宏Q_CLASSINFO()，可以为类的对象定义“名称---值”信息，如：

```c++
class myclass:public QObject{
    Q_OBJECT
        Q_CLASSINFO("auto","wang")
        Q_CLASSINFO("company","UPC")
     public:
    	.......
};
```

```c++
//通过这个函数 获取一个对象，通过它的接口来知道一些信息
QMetaClassInfo QMetaObject::classInfo(int index) const
 //by the function value and name to get some information
```

## 4.3 信号与槽（signal and pot）

信号与槽是对象之间通信的机制，也需要元对象系统支持才能实现的，利用QObject::connect()函数建立联系，类似于原生C++中的“事件——响应”（原生借助于回调函数或者信号，或者一些事件处理比如epoll）

### 1：QObject::connect函数

![32db7c024b9726b11a4a06ddf24fd49](./picture/32db7c024b9726b11a4a06ddf24fd49.jpg)

![c568968474f5c5128700a63a76cca65](./picture/c568968474f5c5128700a63a76cca65.jpg)

**个人认为使用第一种即可**

**关于最后一个参数Qt::ConnectionTytpe type的默认值：**

- **Qt::AutoConnection**:如果信号的接收者与发射者在同一个线程，就使用Qt::DirectConnect方式；否则使用Qt::QueuedConnection方式，在信号发射时自动确认关联方式
- **Qt::DirectConnect**：信号被发射时槽函数立刻执行，槽函数与信号在同一线程
- **Qt::QueuedConnection**：槽函数与信号在不同一线程，当发出信号后，槽函数所在线程在被调度则立马执行
- **Qt::BlockingQueuedConnection**:与Qt::QueuedConnection相似，发送信号的线程必须等槽函数所在的线程，切记，信号与槽函数不要放在一个线程

### **2：sender()获得信号发射者**

在槽函数里，使用QObject::sender()可以获取信号发射者的指针。如果知道知道发射者的具体类型，直接动态强转即可，在使用发射者的接口函数

### 3：自定义信号及其使用

**在Qt中信号就是在类定义里声明的一个函数，但是这个函数无需实现，只需要发射**（emit）

例子：

```c++
class myclass : public QObject{
	Q_OBJECT
private:
    int m_age = 10;
public:
    void incAage();
signals:
    void ageChanged(int value);
}
//通过其他函数带动信号的发送
//再利用connect将其信号与槽连接
myclass::incAage(){
	m_age++;
    emit ageChanged(m_age);
}
```

## 4.4 元对象特性测试实例

#### 1：元对象测试的总结：

一些心得

如果自己定义的信号，是一定要绑定一个试图改变成员变量的函数或者是类外的变量

在写槽函数的时候，尽可能通过sender去判断信号源，在槽函数中利用分支语句执行不同的分支体，提高槽函数的复用性

巧妙使用QT_PROPERTY()和QObject::setProperty()和QObject::Property()函数来设置和读取

这个测试实例，包含了属性系统，信号与槽函数的使用

#### 2：QMetaObject介绍

QMetaObject他的出现是为了更好的查看类的元信息，不止局限于类的成员变量或者用Q_PROPERTY()宏所定义和设置的，还有继承关系，对象树等

#### 3：QObject 和 QMetaObject

##### QObject

- **基类**：`QObject` 是 Qt 中许多类的基类，为它们提供基本的对象功能，包括事件处理、信号和槽的机制、以及对象的父子关系管理等。
- **信号和槽**：`QObject` 的一个核心功能是信号和槽机制，它允许对象之间进行松散耦合的通信。信号和槽机制是 Qt 事件通信的基石，极大地简化了事件驱动程序的开发。
- **动态属性系统**：`QObject` 还支持动态属性的设置和查询，这使得在运行时可以灵活地修改和访问对象的状态。

##### QMetaObject

- **反射能力**：`QMetaObject` 提供了反射能力，允许程序在运行时查询对象的信息，如类名称、继承结构、可用的方法、信号、槽及属性等。这些信息可以用于动态地调用方法，连接信号和槽，以及动态创建对象。
- **类型信息**：通过 `QMetaObject`，Qt 提供了一种机制来处理类型信息，这在与 QML 等高级特性集成时非常有用。例如，可以在 QML 中直接访问和操作 C++ 对象的属性和方法。
- **枚举管理**：`QMetaObject` 还管理类中定义的枚举类型，使得可以在不知道具体值的情况下动态访问枚举。

**QMetaObject用于属性操作的函数有以下几种**

- propertyOffset(),返回类的第一个属性的序号
- propertyCount():返回属性个数
- QMetaProperty property(int index):返回序号为index的属性对象

**总结**

- QObject是所有元对象的父类，为继承的类提供很多的API，用于程序的编写
-  QMetaObject，用于更详细的获取对象的属性，而不仅仅局限于成员变量，还有对象继承关系等

## 4.5 Qt全局定义

### 1:Qt中的数据类型

​	和原生C++差不太大

### 2:**一些常用函数**

书61

### 3:**常用宏**

在头文<QtGloabal头文件中定义了很多宏，常用如下 

1. QT_VERSION

   - 比如qt5.9.1就是0x0050901

   - 宏的展开表示Qt编译器版本，这个宏常用于条件编译设置，根据Qt的版本不同，编译不同的代码段（与#if和#else使用）

2. QT_VERSION_CHECK

   - 这个宏展开是一个qt版本号的整数形式

3. QT_VERSION_STR

   - QT版本的字符串如“5.9.0”

4. Q_BYTE_ORDER,Q_BIG_ENDIAN和Q_LITTLE_ENDIAN

   - 第一个表示当前系统的字节序，第二个为大端字节序，最后一个是小段字节序

5. Q_DECL_OVERIDE

   - 在类的定义中，用于重载一个虚函数，如果你未对它重载则会报错

   - ```c++
     //exmaple：
     //虚函数printEvent()
     voidf printEvent(parament) Q_DEL_OVERRIDE
     ```

6. Q_DECL_FINAL

   - 使用这个宏修饰的虚函数，无法被重载，这是最后一个版本
   - 或修饰一个类，该类则无法被继承



## 4.6 容器类

相较于stl容器的优点：

- 容器类更加轻巧，安全，易于使用
- 进行了速度和存储的优化，减少可执行文件的大小
- 在多线程下读是安全的，无需使用线程同步

对于qt的容器类模版的T必须是可复制的的类型，即一个类必须提供有参构造，拷贝构造，拷贝复制构造

QT提供了foreach可以用于遍历这些容器

### 1： 顺序容器类

Qt的顺序容器类有QList，QLinkedList，Qvector，Qstack和QQueue。

#### 1.1 QList

与list相同，但是它支持使用index去访问数据，它是基于（array-list），链表的插入与删除的效率很高

常用的函数：

- 添加：任意位置 insert（），追加append（）
- 替换： replace（）
- 删除：任意位置removeAt（），删除最前面与最后removeFirst（）and removeLast（）
- 移动和交换：move and swap

其他的一些函数与stl中差不多比如size（）和isEmpty（==empty（））

#### 1.2 QLinkList

QLinkList是链式链表，数据不是连续储存的，无法利用index访问，他的接口函数与上面的基本相同

#### 1.3 QVector

与vector一样，接口函数与QList一样

#### 1.4  QStack

两个常用的操作接口函数，push he pop函数

#### 1.5 Queue

QQueue,队列，两个常用的api enqueue（）和 dequeue（）是主要操作函数

### 2：关联容器类

关联容器：Qmap，QMultiMap,QHash，QMultiHash,QSet

带Multi的支持一键对对多value，Qhash利用散列函数进行查找，查找速度更快

#### 1:QSet

QSet是基于散列表的集合模版类，储存顺序不定，查找值的速度非常快，QSet内部就是QHash实现

可利用〈〈追加元素

测试这个元素是否在集合内使用contains（）函数

#### 2: QMap

与map一样，两个常用的接口函数insert和remove用来添和删除key-value

其他一些容器与stl的差不多，更倾向于stl的容器

## 4.7容器的迭代

stl容器的迭代，利用迭代器，或者auto x ： xxx，迭代器揭开地址后的类型就是<>里面的类型

在qt中利用foreach宏可以遍历容器

```c++
//宏定义原型 foreach（variable，container）//第一个为元素的载体，第二个为容器
QVector<string> v;
string s;
foreach(s,v){
  qDebugc<<s;
}

```

## 4.8 Qt类库的模块

- Qt基本模块（qt Essentials):提供了Qt在所有平台上的基本功能
- Qt附加模块（Qt Add-Ons）:实现一些特定功能的提供附加价值的模块
- 增值模块（value-Add Modules）:单独发布的提供额外价值的模块

- 技术预览模块

### 1： Qt基本模块

![WechatIMG150](picture/WechatIMG150.jpeg)

**Qt Core 模块是Qt类库的核心**，所有其他模块都依赖于此模块，如果使用qmnake构建项目，则Qt Core模块是自动被加入项目的

### 2：Qt附加模块

![WechatIMG152](picture/WechatIMG152.jpeg)

### 3: 技术预览模块

- Qt Network Authorization 基于OAuth协议，为应用程序提供网络账号验证的功能
- Qt Speech 提供文字转语音功能支持
- Qt Remote Objects 进程间或设备间通信，共享QObject的API

### 4 ：工具

- Qt Designer 用于扩展Qt Designer的类
- Qt Help 
- Qt UI Tools 

# 5 常用界面设计组件

**组件用于显示，组件又是由不同定义的类组成**

## 5.1 字符串与输入输出

### 5.1.1字符串与数值之间的转换

界面设计时使用最多的组件恐怕就是QLabel和QLineEit，前者用于显示字符串，后者用于显示和输入字符串，这两个类提供以下两个函数，来获取和设置文本信息

- QString text() const
- void setText(const QString &)

**QString类从字符串转换为整数的函数有** ：

```c++
//默认十进制
int toInt(bool *ok=Q_NULLPTR,int base = 10) const; 
long toLong(bool *ok=Q_NULLPTR,int base = 10) const; 
short toshort(bool *ok=Q_NULLPTR,int base = 10) const; 
uint toUnint(bool *ok=Q_NULLPTR,int base = 10) const; 
ulong toULong(bool *ok=Q_NULLPTR,int base = 10) const; 
```

**QString类从字符串转换为浮点数的函数有** ：

```c++
double toDuble(bool *ok=Q_NULLPTR) const;
float toFlat(bool *ok=Q_NULLPTR) const;
```

```c++
//浮点型保留位数
//静态函数
QString::number(total, 'f',2);
QString::asprintf("%.2f",total);
//类函数
str = str.sprintf("%.2f",total);
```

**QString类提供的进制转换函数**

```c++
//n 为待转换的数字，默认转十进制
QString	&setNum(int n , int base = 10);
QString number(int n, int base = 10);
```

### 5.1.2 QString的常用功能

QString 存储字符串采用的是Unicode码，每一个字符是一个16位的QChar,而不是8位的char，所以处理中文字符是没有问题的，一个汉字算做一个字符

**QString常用的接口函数**

- append ()和 prepend() 前者用于在字符串后面添加内容，如：
- toUpper() 和toLower() 前者字符串转大写后者字符串转小写
- count(),size()和length()都是返回字符串个数
- trimmed() 和 simplifid()前者去掉字符串首位空格，后者用了消除字符串中的空格（像getline）
- indexOf 和 lastIndexOf()

  - ```c++
    //indeOf 类似kmp算法，匹配字符串
    //功能是在字符串中找str的位置，从from指定的位置开始找，最后一个指定是否区分大小写
    int indexOf(const QString& str , int from = 0,Qt::CaseSensitivity cs = Qt::CaseSenstive)
    ```

  - lastIndexOf() 查找某个字符串最后出现的位置，如果是‘\’记得转译即可

- isEmpty()用于判断字符串是否为空
- contains 判断某个字串是否出现在字符串中
- endWith和startsWith()前者判断是以某个字符开头后者判断是否以某个字符结尾
- left()和right()前者是从左开始取多少个字符，后者是从右开始取多少个字符
- section()

  - ```c++
    //功能是从字符串中提取以sep作为分隔符，从start端到end端的字符串
    //原型如下：
    QString section(const QString &sep, int start, int end = -1, SectionFlags flags = SectionDefault) const
    ```

    


## 5.2 SpinBox的使用

QSpinBox用于整数的显示和输入，一般显示十进制数，也可以显示二进制，十六进制的数，而且可以在显示框中增加前缀和后缀

QDoubleSpinBox用于浮点数的显示和输入，可以设置显示小数的位数

**QSpinBox 和 QDoubleSpinBox的主要属性**

![1715825577000](picture/1715825577000.jpg)

## 5.3 其他数值输入和显示组件

![WechatIMG155](picture/WechatIMG155.jpeg)

**QSlider：滑动条，通过滑动来设置数值，可以用于数值输入**

**QScrollBar卷滚条，与QSilder功能类似，还可以用于卷滚区域**

**QProgressBar：进度条，一般用于显示任务进度，可用于数值的百分比显示**

**QDial：表盘式数值输入组件，通过转动表指针获得输入值**

**QLCDNumber：模仿LCD数字的显示组件，可以显示整数或浮点数，显示整数时可以不同进制显示**

### 5.3.1 各组件的主要功能和属性

#### 1 QSlider

**QSlider，QScrollBar和Qdial 这三个组件都从QAbstractslider继承而来，有一些共有属性，QSlider是滑动**

基类QAbstractslider的主要属性包括以下几种：

- minimum，maximum设置输入范围的最小值和最大值
- singleStep：单步长，拖动标尺上的滑块时自动改变此值，限定在minimun和maximum之间
- pageStep：在slider上输入焦点，按PgUp或PgDn键时变化的数值
- value：组件当前值

QSlider的独有属性

- tickPostion：标尺刻度的显示位置，使用枚举类型QSlider：:TickPosition，取值如以下6种

![WechatIMG154](picture/WechatIMG154.jpeg)

- tickInterval：标尺刻度的间隔值，诺设置为0，会在singlestep和pagestep之间自动选择

详细的看手册即可

#### 2: QScrollBar

从QAbstractslider继承而来，没有自己独有的属性

#### 3:QDial

仪表盘式的组件，通过旋转表盘获得输入值，QDial的特有的属性包括以下两种

- notchesVisable：表盘的小刻度是否可见
- notchTarget:表盘刻度间的间隔像素值

#### 4:QProgressBar

QProgressBar的父类是QWidget，一般用于进度显示，常用属性如下：

- minimum，maximum：最小值和最大值
- value：当前值，可以设定或读取当前值
- textVisble：可以设置为水平或垂直方向
- format：显示文字的格式，“%p%”显示百分比，“%v”显示当前值，“%m”显示总步数

#### **5：QLCDNumber**

模拟LCD显示数字的组件，可以显示整数或者小数。

主要属性如下：

- digitCount：显示的数的位数，如果是小数，小数也算一个数位

- smallDecimalPoint：是否有小数点，如果有小数点，就可以显示小数

- mode：数的显示进制，通过调用函数setDecMode(),sectBinMode(),setOctMode(),setHexMode() (分别设置十进制，二进制，八进制和16进制)

- value：显示的值

- intValue：返回显示的整数值

- ```c++
  //exmaple
  digitCount = 3；//代表只能显示三位数（包括小数点），如果要减去一个小数位则默认进位
  smallDecimalPoint = true; //可显示小数
  value = 2.36;
  //在这个QLCD组件上数字只会显示2.4
  ```


## 5.4 时间日期与定时器

### 5.4.1 时间日期相关的类

Qt中时间日期的类如下：

- QTime：时间数据类型，仅表示时间
- QDate：日期数据类型，仅表示日期，如2017-4-5
- QDateTime：日期时间数据类型，如2017-03-23 08：12：43

Qt中专门用于日期，时间编辑和显示的页面组件：

- QTimeEdit：编辑和显示时间的组件
- QDateEdit：编辑和显示日记的组件
- QDateTimeEdit：编辑和显示日期时间的组件
- QCalendarWidget：一个用日历形式选择日期的组件



example：组件展示

![c72ce94f6511d3ed99903ddae1013a0](./picture/c72ce94f6511d3ed99903ddae1013a0.jpg)

### 5.4.2 日期时间数据与字符串之间的转换

#### 1： 时间，日期编辑器属性的设置

如右图有多个日期和时间组成如：QTimeEdit，QDateEdit，QDateTimeEdit等等

QDateTimeEdit类的主要属性的介绍如下：

- **datetime**： 日期时间
- **date**： 日期，设置datetime时会自动改变date，设置date也会改变datetime里的值
- **time**：时间，设置datetime会改变time，设置time也会改变datetime里的时间
- **maximumDatetime， minimumDateTime**：最大，最小日期时间
- **maximmDate， minmum**。。：最大最小日期
- 。。。。time ， 。。。time：最大最小时间
- **currentSection**：当前输入光标所在的时间日期数据段，是枚举类型QDateTimeEdit::Section,则就会在显示时间和日期是分段显示，可以通过改变，显示不同的段
- **currentSectionIndex**：用序号表示输入光标所在的段
- **calendearPopup**：允许弹出一个日历选择框，当取值位true，右侧的输入按钮变成与QCombox类似的下拉按钮，用于在日历上上选择日期，对QTimeEdit，此属性无效
- **displayFormat**：显示格式，日期时间数据的显示格式，列如显示“yyyy--mm-dd:hh:mm:ss”

#### 2： 日期时间数据的获取与转换为字符串

利用QDateTime提供的接口函数toString,可以指定转换的格式，example：

```c++
QDateTime curDateTime = QDateTime::currentDateTime();
ui->editTime->setText(curDateTime.toString("yyyy-mm-dd"));//指定格式
//example
//datetime 2016-11-12
curDateTime.toString("yyyy年mm月dd日")、、2016年11月12日
```

```c++
QString QDateTime::toString(const QString &format) const; //format为格式
```

//format 格式符的意义， 如下：

![2e4d5eb521cecd5abaa2a2196d60312](./picture/2e4d5eb521cecd5abaa2a2196d60312.jpg)

#### 3 字符串转换为日期时间

函数原型，为QDateTime下的静态函数

```c++
QDateTime QDateTime::fromString(const QString &string , const QString &format);//第二个参数是字符串表示的格式
//exmaple:
QString str ; // 2016年12月1日
QDateTime s = QDateTime::fromString(str, "yyyy年mm月dd日")；
```

### 5.4.3 QCaledarWidget

日历组件，图中右边，他有一个信号，selectionChanged(),在日历选择的日期变化后会发射此信号，为此函数创建槽函数即可（将日期值获取再变化即可）

### 5.4.4 定时器的使用              

Qt中的定时器类是QTimer，它的主要属性是interval，是定时中断的周期，单位毫秒，主要信号是timeout(),在定时中断时发射信号，在定时时做出反应就要写timeout的槽函数

定时器提供一下接口函数

配合QTime::currentTime() 函数（获取当时时间）使用

- start()：开始
- stop()：停止

## 5.5 QComboBox 和 QPlainTextEdit

### 5.5.1功能介绍

**QComboBox是下拉列表框组件类**，它提供了一个下拉列表供用户选择，也可以将其当作一个QLineEdit用作输入，QComboBox每一项（item）都可以与QVariant类型的变量，用于存储一些不可见的数据

**QPlainTextEdit**是一个多行文本编辑器，用于显示和编辑多行简文本

example：

![eb78bc8b3209e627d7ff3ea5b9be9d7](./picture/eb78bc8b3209e627d7ff3ea5b9be9d7.jpg)



### 5.5.2 QComboBox的使用

可以通过QTdesign对组件进行添加，删除，上移，下移等操作，还可以设置图标

#### 1： 为QComboBox添加项

单独添加一个接口函数addItem函数

```c++
void Widget::on_btnIniTems_clicked(){
    QIcon icon; //图标类
    icon.addFile("图标路径");
    ui->comboBox->clear();
    for(int i = 0 ; i < 20; I++){
        ui->comboBox->addItem(icon,QString::asprintf("Item %d",i));//带图标
         ui->comboBox->addItem(QString::asprintf("Item %d",i));//不带图标
    }
}
```

添加多个项QStringList + addItems实现

``` c++
QStringList strList;
str<<"date"<<"date"<<"date";
ui->comboBox->additems(strList);
```

#### 2:添加具有用户数据的项

QComboBox::addItem两个版本，原型如下：

```c++
void addItem(const QString &text, const QVariant &userData ,QVariant &userData = QVariant());
void addItem(const QIcon &icon ，const QString &text, const QVariant &userData ,QVariant &userData = QVariant());    
```

#### 3： QComboBox列表项访问

![e04294892410a7426316c5664f517cf](./picture/e04294892410a7426316c5664f517cf.jpg)

​                     
