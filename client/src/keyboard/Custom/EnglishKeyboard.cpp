/*******************************************************************
* EnglishKeyboard.cpp
 * Copyright 2025-2029 zhilai-tech
 * description: 英文键盘实现
 * 2025-12-05, niezixuan
*******************************************************************/
#include "EnglishKeyboard.h"

#include <QDebug>
#include <QToolButton>

#include "KeyboardLayout.h"

QHash<quint16, QString> EnglishKeyboard::m_letterHash{
    {0x0000, "1"}, {0x0001, "2"}, {0x0002, "3"}, {0x0003, "4"}, {0x0004, "5"}, {0x0005, "6"},
    {0x0006, "7"}, {0x0007, "8"}, {0x0008, "9"}, {0x0009, "0"}, {0x0100, "q"}, {0x0101, "w"},
    {0x0102, "e"}, {0x0103, "r"}, {0x0104, "t"}, {0x0105, "y"}, {0x0106, "u"}, {0x0107, "i"},
    {0x0108, "o"}, {0x0109, "p"}, {0x0201, "a"}, {0x0202, "s"}, {0x0203, "d"}, {0x0204, "f"},
    {0x0205, "g"}, {0x0206, "h"}, {0x0207, "j"}, {0x0208, "k"}, {0x0209, "l"}, {0x0302, "z"},
    {0x0303, "x"}, {0x0304, "c"}, {0x0305, "v"}, {0x0306, "b"}, {0x0307, "n"}, {0x0308, "m"}};

/*******************************************************************
 * description: 构造函数
 * para: parent 父控件
 * return: void
 *******************************************************************/
EnglishKeyboard::EnglishKeyboard(QWidget *parent)
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
EnglishKeyboard::~EnglishKeyboard() = default;

/*******************************************************************
 * description: 获取键盘名
 * para: void
 * return: QString 键盘名称
 *******************************************************************/
auto EnglishKeyboard::name() const -> QString
{
    return "english keyboard";
}

/*******************************************************************
 * description: 获取大小写切换状态
 * para: void
 * return: bool 是否为大写模式
 *******************************************************************/
auto EnglishKeyboard::upper() const -> bool
{
    return m_upperEnable;
}

/*******************************************************************
 * description: 设置大小写切换
 * para: enable 是否启用大写模式
 * return: void
 *******************************************************************/
auto EnglishKeyboard::setUpper(bool enable) -> void
{
    if (m_upperEnable == enable) {
        return;
    }

    auto keys = m_letterHash.keys();
    for (const auto &key : keys) {
        auto btn = m_layout->button(key);
        auto letter = m_letterHash.value(key);

        auto text = enable ? letter.toUpper() : letter.toLower();
        btn->setText(text);
    }

    m_upperEnable = enable;
}

/*******************************************************************
 * description: 初始化键盘布局
 * para: void
 * return: void
 *******************************************************************/
auto EnglishKeyboard::initKeyboard() -> void
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
auto EnglishKeyboard::initConnection() -> void
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
auto EnglishKeyboard::initBasicBtn() -> void
{
    auto keys = m_letterHash.keys();
    for (const auto &key : keys) {
        auto btn = m_layout->button(key);
        auto letter = m_letterHash.value(key);

        btn->setText(letter);
        btn->setObjectName("basic");
        btn->setProperty("name", letter);
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
auto EnglishKeyboard::initSpecialBtn() -> void
{
    auto switchBtn = m_layout->button(0x0200);
    if (switchBtn) {
        switchBtn->setObjectName("special");
        switchBtn->setProperty("name", "switch");
        switchBtn->setProperty("type", static_cast<int>(AbstractKeyboard::KeyType::SwitchKey));
        switchBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
        switchBtn->setIcon(QIcon(":/icon/icon/num_sym_icon@2x.png"));
        constexpr int IconSize = 28;
        switchBtn->setIconSize({IconSize, IconSize});
    }

    constexpr int ShiftIconSize = 26;
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

    auto spaceBtn = m_layout->button(0x0301);
    if (spaceBtn) {
        spaceBtn->setObjectName("special");
        spaceBtn->setProperty("name", "space");
        spaceBtn->setAutoRepeat(true);
        spaceBtn->setProperty("type", static_cast<int>(AbstractKeyboard::KeyType::SpaceKey));
        spaceBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
        spaceBtn->setIcon(QIcon(":/icon/icon/space@2x.png"));
        spaceBtn->setIconSize({ShiftIconSize, ShiftIconSize});
    }

    auto backspaceBtn = m_layout->button(0x0309);
    if (backspaceBtn) {
        backspaceBtn->setObjectName("special");
        backspaceBtn->setProperty("name", "backspace");
        backspaceBtn->setAutoRepeat(true);
        backspaceBtn->setProperty("type", static_cast<int>(AbstractKeyboard::KeyType::BackspaceKey));
        backspaceBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
        backspaceBtn->setIcon(QIcon(":/icon/icon/backspace_icon@2x.png"));
        backspaceBtn->setIconSize({ShiftIconSize, ShiftIconSize});
    }
}
