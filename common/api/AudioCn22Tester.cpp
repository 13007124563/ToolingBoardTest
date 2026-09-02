#include "AudioCn22Tester.h"

#include <QApplication>
#include <QElapsedTimer>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>
#include <QtGlobal>

const char *AudioCn22Tester::kMusicPath = "/usr/share/myir/Music/song.mp3";
const char *AudioCn22Tester::kFallbackScript = "/etc/myir_test/myir_audio_play";
const int AudioCn22Tester::kMaxPlayMs = 15000;

namespace {

bool runShellCommand(const QString &cmdLine, QString *log, int timeoutMs = 8000)
{
    if (log)
        *log += QStringLiteral("$ %1\n").arg(cmdLine);

    QProcess proc;
    proc.start(QStringLiteral("sh"), QStringList() << QStringLiteral("-c") << cmdLine);
    if (!proc.waitForStarted(3000)) {
        if (log)
            *log += QStringLiteral("Failed to start shell command\n");
        return false;
    }
    if (!proc.waitForFinished(timeoutMs)) {
        proc.kill();
        proc.waitForFinished(2000);
        if (log)
            *log += QStringLiteral("Command timeout (%1 ms)\n").arg(timeoutMs);
        return false;
    }

    const QString out = QString::fromUtf8(proc.readAllStandardOutput()).trimmed();
    const QString err = QString::fromUtf8(proc.readAllStandardError()).trimmed();
    if (!out.isEmpty() && log)
        *log += out + QLatin1Char('\n');
    if (!err.isEmpty() && log)
        *log += err + QLatin1Char('\n');
    if (proc.exitCode() != 0) {
        if (log)
            *log += QStringLiteral("Exit code: %1\n").arg(proc.exitCode());
        return false;
    }
    return true;
}

bool runPlaybackProcess(QProcess *proc, const QString &program, const QStringList &args,
                        QString *log, int maxPlayMs)
{
    if (log) {
        *log += QStringLiteral("$ %1 %2\n").arg(program, args.join(QLatin1Char(' ')));
    }

    proc->start(program, args);
    if (!proc->waitForStarted(3000)) {
        if (log)
            *log += QStringLiteral("Failed to start %1\n").arg(program);
        return false;
    }

    QElapsedTimer timer;
    timer.start();
    while (!proc->waitForFinished(300)) {
        QApplication::processEvents();
        if (timer.elapsed() > maxPlayMs) {
            proc->kill();
            proc->waitForFinished(2000);
            if (log)
                *log += QStringLiteral("Playback stopped after %1 ms\n").arg(maxPlayMs);
            return true;
        }
    }

    const QString out = QString::fromUtf8(proc->readAllStandardOutput()).trimmed();
    const QString err = QString::fromUtf8(proc->readAllStandardError()).trimmed();
    if (!out.isEmpty() && log)
        *log += out + QLatin1Char('\n');
    if (!err.isEmpty() && log)
        *log += err + QLatin1Char('\n');

    if (proc->exitCode() != 0) {
        if (log)
            *log += QStringLiteral("Exit code: %1\n").arg(proc->exitCode());
        return false;
    }
    return true;
}

} // namespace

AudioCn22TestResult AudioCn22Tester::runPlayback()
{
    AudioCn22TestResult result;

#if !defined(Q_OS_LINUX)
    result.detail = QStringLiteral("Simulated CN22 audio playback (Windows)\n");
    result.detail += QStringLiteral("$ amixer -q set PCM 100%% && amixer -q set Headphone 90%% && amixer -q set Lineout 90%%\n");
    result.detail += QStringLiteral("$ amixer -q set Headphone unmute\n");
    result.detail += QStringLiteral("$ gst-play-1.0 %1\n").arg(QString::fromLatin1(kMusicPath));
    result.playbackOk = true;
    result.summary = QStringLiteral("Playback OK (simulated)");
    result.detail += QStringLiteral("Result: playback ready\n");
    return result;
#else
    const QString volumeCmd = QStringLiteral(
        "amixer -q set PCM 100% && amixer -q set Headphone 90% && "
        "amixer -q set Lineout 90% && amixer -q set Headphone unmute");
    if (!runShellCommand(volumeCmd, &result.detail, 8000)) {
        result.summary = QStringLiteral("Audio FAIL (volume)");
        result.detail += QStringLiteral("Volume setup failed\n");
        return result;
    }

    const QString musicPath = QString::fromLatin1(kMusicPath);
    if (!QFileInfo::exists(musicPath)) {
        result.summary = QStringLiteral("Audio FAIL (no file)");
        result.detail += QStringLiteral("Missing music file: %1\n").arg(musicPath);
        return result;
    }

    QProcess proc;
    bool played = false;

    if (QFileInfo::exists(QStringLiteral("/usr/bin/gst-play-1.0"))
        || !QStandardPaths::findExecutable(QStringLiteral("gst-play-1.0")).isEmpty()) {
        const QString gst = QFileInfo::exists(QStringLiteral("/usr/bin/gst-play-1.0"))
            ? QStringLiteral("/usr/bin/gst-play-1.0")
            : QStandardPaths::findExecutable(QStringLiteral("gst-play-1.0"));
        played = runPlaybackProcess(&proc, gst, QStringList() << musicPath,
                                    &result.detail, kMaxPlayMs);
    }

    if (!played && QFileInfo::exists(QString::fromLatin1(kFallbackScript))) {
        result.detail += QStringLiteral("Fallback: echo 1 | %1\n")
                             .arg(QString::fromLatin1(kFallbackScript));
        played = runShellCommand(
            QStringLiteral("echo 1 | %1").arg(QString::fromLatin1(kFallbackScript)),
            &result.detail, kMaxPlayMs);
    }

    if (!played) {
        result.summary = QStringLiteral("Audio FAIL (playback)");
        result.detail += QStringLiteral(
            "Playback failed. Install gstreamer1.0-tools or provide %1\n")
                             .arg(QString::fromLatin1(kFallbackScript));
        return result;
    }

    result.playbackOk = true;
    result.summary = QStringLiteral("Playback OK");
    result.detail += QStringLiteral("Result: playback finished\n");
    return result;
#endif
}
