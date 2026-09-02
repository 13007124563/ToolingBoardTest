#ifndef AUDIOCN22TESTER_H
#define AUDIOCN22TESTER_H

#include <QString>

/** CN22 音频播放测试结果（不含人工确认）。 */
struct AudioCn22TestResult {
    bool playbackOk = false;
    QString summary;
    QString detail;
};

/**
 * CN22 音频接口测试：设置音量后播放示例 MP3。
 * 参考：amixer 调音量 + gst-play-1.0 /usr/share/myir/Music/song.mp3
 */
class AudioCn22Tester
{
public:
    static const char *kMusicPath;
    static const char *kFallbackScript;
    static const int kMaxPlayMs;

    /** 设置音量并播放音频；playbackOk 表示播放命令执行成功。 */
    static AudioCn22TestResult runPlayback();
};

#endif // AUDIOCN22TESTER_H
