/*******************************************************************
* NumberKeyboard.cpp
 * Copyright 2025-2029 zhilai-tech
 * description: 数字键盘实现
 * 2025-12-05, niezixuan
*******************************************************************/
#include "NumberKeyboard.h"

#include <QDebug>
#include <QLayout>
#include <QToolButton>

#include "KeyboardLayout.h"

QHash<quint16, QString> NumberKeyboard::m_numberHash{
    {0x0000, "7"}, {0x0001, "8"}, {0x0002, "9"}, {0x0003, "-"}, {0x0100, "4"}, {0x0101, "5"}, {0x0102, "6"},
    {0x0103, "+"}, {0x0200, "1"}, {0x0201, "2"}, {0x0202, "3"}, {0x0203, "."}, {0x0301, "0"}, {0x0303, "/"},
};

/*******************************************************************
 * description: 构造函数
 * para: parent 父控件
 * return: void
 *******************************************************************/
NumberKeyboard::NumberKeyboard(QWidget *parent)
    : AbstractKeyboard(parent)
    , m_layout(new KeyboardLayout(this))
{
    initKeyboard();
    initConnection();
}

/*******************************************************************
 * description: 析构函数
 * para: void
 * return: void
 *******************************************************************/
NumberKeyboard::~NumberKeyboard() = default;

/*******************************************************************
 * description: 获取键盘名
 * para: void
 * return: QString 键盘名称
 *******************************************************************/
auto NumberKeyboard::name() const -> QString
{
    return "number keyboard";
}

/*******************************************************************
 * description: 初始化键盘布局
 * para: void
 * return: void
 *******************************************************************/
auto NumberKeyboard::initKeyboard() -> void
{
    int rowCount = 4;
    int columnCount = 4;
    for (int row = 0; row < rowCount; ++row) {
        for (int col = 0; col < columnCount; ++col) {
            m_layout->addNode({row, col});
        }
    }

    auto layoutWidget = new QWidget(this);
    constexpr int MinLayoutWidth = 300;
    layoutWidget->setMinimumWidth(MinLayoutWidth);

    m_layout->setContentsMargins(8, 2, 8, 8);
    layoutWidget->setLayout(m_layout);

    auto hBoxLayout = new QHBoxLayout(layoutWidget);
    hBoxLayout->setContentsMargins(0, 0, 0, 0);
    hBoxLayout->setSpacing(0);
    hBoxLayout->addStretch();
    hBoxLayout->addWidget(layoutWidget);
    hBoxLayout->addStretch();

    setLayout(hBoxLayout);

    initBasicBtn();
    initSpecialBtn();
}

/*******************************************************************
 * description: 初始化信号槽连接
 * para: void
 * return: void
 *******************************************************************/
auto NumberKeyboard::initConnection() -> void
{
    auto indexes = m_layout->indexes();
    for (const auto &index : indexes) {
        auto* btn = m_layout->button(index);
        if (btn == nullptr) {
            continue;
        }

        connect(btn, &QToolButton::clicked, this, [this, btn]() {
            buttonClicked(btn);
        });
    }
}

/*******************************************************************
 * description: 初始化基础按键
 * para: void
 * return: void
 *******************************************************************/
auto NumberKeyboard::initBasicBtn() -> void
{
    auto keys = m_numberHash.keys();
    for (const auto &key : keys) {
        auto btn = m_layout->button(key);
        auto text = m_numberHash.value(key);

        btn->setText(text);
        btn->setObjectName("basic");
        btn->setProperty("name", text);
        btn->setAutoRepeat(true);
        btn->setProperty("type", static_cast<int>(AbstractKeyboard::KeyType::NormalKey));
        btn->setToolButtonStyle(Qt::ToolButtonTextOnly);
    }
}

/*******************************************************************
 * description: 初始化特殊按键
 * para: void
 * return: void
 *******************************************************************/
auto NumberKeyboard::initSpecialBtn() -> void
{
    constexpr int SwitchIconSize = 28;
    constexpr int BackspaceIconSize = 26;
    auto switchBtn = m_layout->button(0x0300);
    if (switchBtn) {
        switchBtn->setObjectName("special");
        switchBtn->setProperty("name", "switch");
        switchBtn->setProperty("type", static_cast<int>(AbstractKeyboard::KeyType::SwitchKey));
        switchBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
        switchBtn->setIcon(QIcon(":/icon/icon/1_符_icon@2x.png"));
        switchBtn->setIconSize({SwitchIconSize, SwitchIconSize});
    }

    auto backspaceBtn = m_layout->button(0x0302);
    if (backspaceBtn) {
        backspaceBtn->setObjectName("special");
        backspaceBtn->setProperty("name", "backspace");
        backspaceBtn->setAutoRepeat(true);
        backspaceBtn->setProperty("type", static_cast<int>(AbstractKeyboard::KeyType::BackspaceKey));
        backspaceBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
        backspaceBtn->setIcon(QIcon(":/icon/icon/backspace_icon@2x.png"));
        backspaceBtn->setIconSize({BackspaceIconSize, BackspaceIconSize});
    }
}
