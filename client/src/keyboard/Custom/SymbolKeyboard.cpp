/*******************************************************************
* SymbolKeyboard.cpp
 * Copyright 2025-2029 zhilai-tech
 * description: 符号键盘实现
 * 2025-12-05, niezixuan
*******************************************************************/
#include "SymbolKeyboard.h"

#include <QDebug>
#include <QLayout>
#include <QToolButton>

#include "KeyboardLayout.h"

QHash<quint16, QString> SymbolKeyboard::m_symbolHash{
    {0x0000, "!"},  {0x0001, "@"},  {0x0002, "#"},  {0x0003, "$"},  {0x0004, "%"},  {0x0005, "&&"},
    {0x0006, "*"},  {0x0007, "("},  {0x0008, ")"},  {0x0100, "~"},  {0x0101, "`"},  {0x0102, "_"},
    {0x0103, "+"},  {0x0104, "-"},  {0x0105, "="},  {0x0106, "{"},  {0x0107, "}"},  {0x0108, "["},
    {0x0109, "]"},  {0x0201, "\\"}, {0x0202, ":"},  {0x0203, ";"},  {0x0204, "\""}, {0x0205, "'"},
    {0x0206, "<"},  {0x0207, ">"},  {0x0208, "?"},  {0x0209, ","},  {0x0301, "/"},  {0x0302, "："},
    {0x0303, "，"}, {0x0304, "."}, {0x0305, "？"}, {0x0306, "；"}, {0x0307, "|"},  {0x0308, "^"},
};

/*******************************************************************
 * description: 构造函数
 * para: parent 父控件
 * return: void
 *******************************************************************/
SymbolKeyboard::SymbolKeyboard(QWidget *parent)
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
SymbolKeyboard::~SymbolKeyboard() = default;

/*******************************************************************
 * description: 获取键盘名
 * para: void
 * return: QString 键盘名称
 *******************************************************************/
auto SymbolKeyboard::name() const -> QString
{
    return "symbol keyboard";
}

/*******************************************************************
 * description: 初始化键盘布局
 * para: void
 * return: void
 *******************************************************************/
auto SymbolKeyboard::initKeyboard() -> void
{
    constexpr int RowCount = 4;
    constexpr int ColumnCount = 10;
    int rowCount = RowCount;
    int columnCount = ColumnCount;
    for (int row = 0; row < rowCount; ++row) {
        for (int col = 0; col < columnCount; ++col) {
            m_layout->addNode({row, col});
        }
    }

    m_layout->setContentsMargins(8, 2, 8, 8);
    setLayout(m_layout);

    initBasicBtn();
    initSpecialBtn();
}

/*******************************************************************
 * description: 初始化信号槽连接
 * para: void
 * return: void
 *******************************************************************/
auto SymbolKeyboard::initConnection() -> void
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
auto SymbolKeyboard::initBasicBtn() -> void
{
    auto keys = m_symbolHash.keys();
    for (const auto &key : keys) {
        auto* btn = m_layout->button(key);
        if (btn == nullptr) {
            continue;
        }

        auto text = m_symbolHash.value(key);
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
auto SymbolKeyboard::initSpecialBtn() -> void
{
    constexpr int SwitchIconSize = 28;
    constexpr int ShiftIconSize = 26;
    auto switchBtn = m_layout->button(0x0200);
    if (switchBtn) {
        switchBtn->setObjectName("special");
        switchBtn->setProperty("name", "switch");
        switchBtn->setProperty("type", static_cast<int>(AbstractKeyboard::KeyType::SwitchKey));
        switchBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
        switchBtn->setIcon(QIcon(":/icon/icon/符_A_icon@2x.png"));
        switchBtn->setIconSize({SwitchIconSize, SwitchIconSize});
    }

    auto backspaceBtn = m_layout->button(0x0009);
    if (backspaceBtn) {
        backspaceBtn->setObjectName("special");
        backspaceBtn->setProperty("name", "backspace");
        backspaceBtn->setAutoRepeat(true);
        backspaceBtn->setProperty("type", static_cast<int>(AbstractKeyboard::KeyType::BackspaceKey));
        backspaceBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
        backspaceBtn->setIcon(QIcon(":/icon/icon/backspace_icon@2x.png"));
        backspaceBtn->setIconSize({ShiftIconSize, ShiftIconSize});
    }

    auto shiftBtn = m_layout->button(0x0300);
    if (shiftBtn) {
        shiftBtn->setObjectName("special");
        shiftBtn->setProperty("name", "shift");
        shiftBtn->setCheckable(true);
        shiftBtn->setProperty("type", static_cast<int>(AbstractKeyboard::KeyType::ShiftKey));
        shiftBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
        shiftBtn->setIcon(QIcon(":/icon/icon/shift_icon@2x.png"));
        shiftBtn->setIconSize({ShiftIconSize, ShiftIconSize});
    }

    auto spaceBtn = m_layout->button(0x0309);
    if (spaceBtn) {
        spaceBtn->setObjectName("special");
        spaceBtn->setProperty("name", "space");
        spaceBtn->setAutoRepeat(true);
        spaceBtn->setProperty("type", static_cast<int>(AbstractKeyboard::KeyType::SpaceKey));
        spaceBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
        spaceBtn->setIcon(QIcon(":/icon/icon/space@2x.png"));
        spaceBtn->setIconSize({ShiftIconSize, ShiftIconSize});
    }
}
