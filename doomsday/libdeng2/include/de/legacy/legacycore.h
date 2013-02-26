/*
 * The Doomsday Engine Project -- libdeng2
 *
 * Copyright (c) 2011-2013 Jaakko Keränen <jaakko.keranen@iki.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBDENG2_LEGACYCORE_H
#define LIBDENG2_LEGACYCORE_H

#include "../libdeng2.h"
#include "../Log"

namespace de {

/**
 * Transitional kernel for supporting legacy Doomsday C code in accessing
 * libdeng2 functionality. The legacy engine needs to construct one of these
 * via the deng2 C API and make sure it gets destroyed at shutdown. The C API
 * can be used to access functionality in LegacyCore.
 *
 * @todo Move the Loop into its own class and get rid of this one. A Loop
 * instance should then be owned by GuiApp and TextApp.
 */
class DENG2_PUBLIC LegacyCore : public QObject
{
    Q_OBJECT

public:
    /**
     * Initializes the legacy core.
     */
    LegacyCore();

    ~LegacyCore();

    /**
     * Starts the deng2 event loop in the current thread. Does not return until
     * the loop is stopped.
     *
     * @return  Exit code.
     */
    int runEventLoop();

    /**
     * Sets the frequency for calling the loop function (e.g., 35 Hz for a
     * dedicated server). Not very accurate: the actual rate at which the
     * function is called is probably less.
     *
     * @param freqHz  Frequency in Hz.
     */
    void setLoopRate(int freqHz);

    /**
     * Sets the callback function that gets called periodically from the main
     * loop. The calls are made as often as possible without blocking the loop.
     *
     * @param callback  Loop callback function.
     */
    void setLoopFunc(void (*callback)(void));

    /**
     * Saves the current loop rate and function and pushes them on a stack.
     */
    void pushLoop();

    /**
     * Pops the loop rate and function from the stack and replaces the current
     * with the popped ones.
     */
    void popLoop();

    /**
     * Pauses the loop function callback.
     */
    void pauseLoop();

    /**
     * Resumes calls to the loop function callback.
     */
    void resumeLoop();

    /**
     * Stops the event loop. This is automatically called when the core is
     * destroyed.
     */
    void stop(int exitCode = 0);

    /**
     * Registers a new single-shot timer that will do a callback.
     *
     * @param milliseconds  Time to wait.
     * @param func  Callback to call.
     */
    void timer(duint32 milliseconds, void (*func)(void));

    /**
     * Sets the file where log output is to be written.
     *
     * @param filePath  Path of a native file for writing output. "/home/" will
     *                  be automatically prepended to the path.
     */
    void setLogFileName(char const *filePath);

    /**
     * Returns name of the the current log output file.
     */
    char const *logFileName() const;

    /**
     * Prints a fragment of text to the output log. The output is added to the log
     * only when a complete line has been printed (i.e., newline character required).
     * "Fragment" means that the text is not considered to form a complete line;
     * no newline character is automatically added to the end.
     *
     * @param text   Text to print.
     * @param level  Log level for the message. Only the level in effect when a newline
     *               is printed will be entered into the log.
     */
    void printLogFragment(char const *text, LogEntry::Level level = LogEntry::MESSAGE);

    /**
     * Returns the LegacyCore singleton instance.
     */
    static LegacyCore &instance();

public slots:
    void callback();

private:
    DENG2_PRIVATE(d)

    /// Globally available.
    static LegacyCore *_appCore;
};

} // namespace de

#endif // LIBDENG2_LEGACYCORE_H
