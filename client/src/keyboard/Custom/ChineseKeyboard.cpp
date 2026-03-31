/*******************************************************************
* ChineseKeyboard.cpp
 * Copyright 2025-2029 zhilai-tech
 * description: 中文键盘实现
 * 2025-12-05, niezixuan
*******************************************************************/
#include "ChineseKeyboard.h"

#include <QDateTime>
#include <QDebug>
#include <QEventLoop>
#include <QFile>
#include <QRegularExpression>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

#include "KeyboardLayout.h"

QHash<quint16, QString> ChineseKeyboard::m_letterHash{
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
ChineseKeyboard::ChineseKeyboard(QWidget *parent)
    : AbstractKeyboard{parent}
    , m_layout(new KeyboardLayout(this))
    , m_chineseHash{}
{
    initKeyboard();
    initConnection();
}

/*******************************************************************
 * description: 析构函数
 * para: void
 * return: void
 *******************************************************************/
ChineseKeyboard::~ChineseKeyboard() = default;

/*******************************************************************
 * description: 获取键盘名
 * para: void
 * return: QString 键盘名称
 *******************************************************************/
auto ChineseKeyboard::name() const -> QString
{
    return "chinese keyboard";
}

/*******************************************************************
 * description: 获取大小写切换状态
 * para: void
 * return: bool 是否为大写模式
 *******************************************************************/
auto ChineseKeyboard::upper() const -> bool
{
    return m_upperEnable;
}

/*******************************************************************
 * description: 设置大小写切换
 * para: enable 是否启用大写模式
 * return: void
 *******************************************************************/
auto ChineseKeyboard::setUpper(bool enable) -> void
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
 * description: 获取候选词
 * para: input 输入拼音
 * return: QStringList 候选词列表
 *******************************************************************/
auto ChineseKeyboard::matched(const QString &input) const -> QStringList
{
    QStringList resList;

    // 全字匹配优先于模糊匹配
    // 单字匹配优先于多字匹配
    QRegularExpression exactRegex("^" + QRegularExpression::escape(input) + "$",
                                  QRegularExpression::CaseInsensitiveOption);
    QRegularExpression fuzzyRegex(input, QRegularExpression::CaseInsensitiveOption);
    QSet<QString> singleCharExactMatches{};
    QSet<QString> multiCharExactMatches{};
    QSet<QString> singleCharFuzzyMatches{};
    QSet<QString> multiCharFuzzyMatches{};

    QEventLoop loop;
    QTimer::singleShot(0, [&]() {
        for (auto it = m_chineseHash.constBegin(); it != m_chineseHash.constEnd(); ++it) {
            if (exactRegex.match(it.key()).hasMatch()) {
                if (it.value().length() == 1) {
                    singleCharExactMatches.insert(it.value());
                } else {
                    multiCharExactMatches.insert(it.value());
                }
            } else if (fuzzyRegex.match(it.key()).hasMatch()) {
                if (it.value().length() == 1) {
                    singleCharFuzzyMatches.insert(it.value());
                } else {
                    multiCharFuzzyMatches.insert(it.value());
                }
            }
        }

        // QStringList 的迭代器构造在 Qt 5.14 以下不支持 QSet 迭代器，用 toList() 兼容全版本
        auto setToList = [](const QSet<QString> &s) -> QStringList {
            QStringList l;
            l.reserve(s.size());
            for (const QString &v : s) { l << v; }
            return l;
        };
        resList = setToList(singleCharExactMatches) +
                  setToList(multiCharExactMatches) +
                  setToList(singleCharFuzzyMatches) +
                  setToList(multiCharFuzzyMatches);

        loop.quit();
    });

    loop.exec();

    return resList;
}

/*******************************************************************
 * description: 初始化键盘布局
 * para: void
 * return: void
 *******************************************************************/
auto ChineseKeyboard::initKeyboard() -> void
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
    readDictionary();
}

/*******************************************************************
 * description: 初始化信号槽连接
 * para: void
 * return: void
 *******************************************************************/
auto ChineseKeyboard::initConnection() -> void
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
auto ChineseKeyboard::initBasicBtn() -> void
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
auto ChineseKeyboard::initSpecialBtn() -> void
{
    constexpr int SwitchIconSize = 28;
    constexpr int ShiftIconSize = 26;
    auto switchBtn = m_layout->button(0x0200);
    if (switchBtn) {
        switchBtn->setObjectName("special");
        switchBtn->setProperty("name", "switch");
        switchBtn->setProperty("type", static_cast<int>(AbstractKeyboard::KeyType::SwitchKey));
        switchBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
        switchBtn->setIcon(QIcon(":/icon/icon/中_1_icon@2x.png"));
        switchBtn->setIconSize({SwitchIconSize, SwitchIconSize});
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

/*******************************************************************
 * description: 读取字典文件
 * para: void
 * return: void
 *******************************************************************/
auto ChineseKeyboard::readDictionary() -> void
{
    QFile pinyinFile(":/dictionary/dict/ChinesePinyin");
    if (!pinyinFile.open(QIODevice::ReadOnly)) {
        qWarning() << "failed to parse chinese dictionary";
        return;
    }

    QRegularExpression regExp("[a-z']+");
    QString lineText, linePinyin, lineChinese;
    int pinyinPosition;

    while (!pinyinFile.atEnd()) {
        lineText = QString::fromUtf8(pinyinFile.readLine());
        QRegularExpressionMatch match = regExp.match(lineText, 0);
        pinyinPosition = match.capturedStart();
        linePinyin = match.captured(0);
        lineChinese = lineText.left(pinyinPosition);

        if (linePinyin.contains("'")) {
            splitPhrase(linePinyin, lineChinese);
        } else {
            m_chineseHash.insert(linePinyin, lineChinese);
        }
    }
}

/**************************************************************************
 * @brief 解析拼音词组
 * @param phrase 拼音词组
 * @param chinese 中文字符串
 * @return void
 **************************************************************************/
auto ChineseKeyboard::splitPhrase(QString phrase, QString chinese) -> void
{
    QStringList pinyinList;

    pinyinList = phrase.split("'");
    int pinyinSize = pinyinList.size();

    for (int i = 1; i <= pinyinSize; ++i) {
        QString pinyin;
        // 构建首字母简拼
        for (int j = 0; j < i; ++j) {
            QString item = pinyinList.at(j);
            if (!item.isEmpty()) {
                pinyin.append(item.at(0));
            }
        }

        // 构建全拼
        QString fullPinyin;
        for (int j = 0; j < i; ++j) {
            fullPinyin.append(pinyinList.at(j));
        }

        // 插入哈希表
        m_chineseHash.insert(pinyin, chinese);
        m_chineseHash.insert(fullPinyin, chinese);
    }
}
