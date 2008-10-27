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

#include "client.h"
#include "gameserver.h"
#include "player.h"
#include "clientplayerctrl.h"
#include "clientcontroller.h"

#include <QTcpSocket>
#include <QXmlStreamWriter>



Client::Client(GameServer *parent, int clientId, QTcpSocket *socket)
 : QObject(parent), m_clientId(clientId), m_xmlParser(this, socket),
   mp_player(0)
{
    mp_clientPlayerCtrl = new ClientPlayerCtrl(this, &m_xmlParser);
    mp_clientController = new ClientController(this, mp_clientPlayerCtrl);
    Q_ASSERT(clientId != 0);
    qDebug("%s:%d: Client #%d connected.", __FILE__, __LINE__, m_clientId);
    connect(&m_xmlParser, SIGNAL(disconnected()),
            this, SLOT(deleteLater()));
}


Client::~Client()
{
    emit disconnected(m_clientId);
    delete mp_clientPlayerCtrl;
    qDebug("%s:%d: Client #%d disconnected.", __FILE__, __LINE__, m_clientId);
}



/*
bool Client::parseStart()
{
    if (!m_xml.isStartElement() || m_xml.name() != "stream")
    {
        qDebug("Invalid client. Opening stream tag expected.");
        disconnectFromHost();
        return 1;
    }
    QXmlStreamAttributes attrs = m_xml.attributes();
    QStringList version = attrs.value("", "version").toString().split(".");
    if (version.size() >= 1) m_protocolVersion.first = version[0].toInt();
    if (version.size() >= 2) m_protocolVersion.second = version[1].toInt();
    m_clientState = CLIENT_STATE_INIT_RECIEVED;
    m_parseLevel = 0;
    sendStart();
    return 0;
}

void Client::parseEnd()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << "</stream>\n";
    mp_tcpSocket->write(block);
    disconnectFromHost();
}

void Client::sendStart()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << "<stream version=\"1.0\">\n";
    mp_tcpSocket->write(block);

}

void Client::parseIq()
{
    QXmlStreamAttributes attrs = m_xml.attributes();
    QString id = attrs.value("", "id");
    QString type = attrs.value("", "type");
    if (id.isEmpty() || type.isEmpty()) return sendIqError();

}

void Client::sendIqError()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << "<iq type=\"error\" id="" />\n";
    mp_tcpSocket->write(block);
}
*/

void Client::writeXml(QXmlStreamWriter& xmlOut)
{
    xmlOut.writeStartElement("client");
    xmlOut.writeAttribute("id", QString::number(m_clientId));
    xmlOut.writeAttribute("name", m_clientName);
    xmlOut.writeEndElement();
}

ClientController* Client::clientController() const
{
    return mp_clientController;
}

AbstractPlayerCtrl* Client::playerController() const
{
    return mp_clientPlayerCtrl;
}

bool Client::isInGame() const
{
    return mp_clientPlayerCtrl->isAttached();
}

void Client::setPlayer(Player* player)
{
    mp_player = player;
}

Player* Client::player()
{
    return mp_player;
}

void Client::postEventToController(QEvent* event)
{
    QCoreApplication::postEvent(mp_clientPlayerCtrl, event);
}

int Client::gameId() const
{
    return mp_clientController->gameId();
}
