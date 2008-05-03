/***************************************************************************
 *   Copyright (C) 2008 by MacJariel                                       *
 *   echo "badmailet@gbalt.dob" | tr "edibmlt" "ecrmjil"                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "console.h"
#include "consolecommand.h"
#include "gameserver.h"
#include <stdio.h>
#include <QIODevice>

Console::Console(GameServer* gameServer, FILE* in, FILE* out):
QThread(gameServer), mp_gameServer(gameServer), m_cin(in, QIODevice::ReadOnly), m_cout(out, QIODevice::WriteOnly)
{
}


Console::~Console()
{
}

void Console::run()
{
    initConsole();
    QString cmd;
    while(1)
    {
        cmd = readCommand();
        execCommand(cmd);
    }
}

void Console::initConsole()
{
    ConsoleCmd::initialize();
    m_cout << "Welcome to KBang Server version 12345\n" << endl;
}

QString Console::readCommand()
{
    m_cout << "> " << flush;
    return m_cin.readLine();
}

/**
 *
 * @param cmdString
 */
void Console::execCommand(const QString& cmdString)
{
    QPair<QString, QStringList> pair = ConsoleCmd::parse(cmdString);
    const ConsoleCmd& cmd = ConsoleCmd::instance(pair.first);
    QString result = cmd.execute(pair.first, pair.second, mp_gameServer);
    //QString result(cmd.exec(mp_gameServer));
    if (result.size())
    {
        QStringList resultLines = result.split('\n');
        for(QStringList::const_iterator it = resultLines.begin(); it != resultLines.end(); ++it)
        {
            m_cout << "   " << *it << endl;
        }
    } else {
        m_cout << flush;
    }

}
