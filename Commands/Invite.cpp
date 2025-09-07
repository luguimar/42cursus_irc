#include "../Server.hpp"

void Server::invite(int fd_c, const std::vector<std::string>& a)
{
    if (a.size() < 3) { sendNumeric(fd_c, 461, "INVITE", "Not enough parameters"); return; }
    const std::string& nick = a[1];
    const std::string& chan = a[2];

    Channel* ch = getChannel(chan);
    if (!ch) { sendNumeric(fd_c, 403, chan, "No such channel"); return; }
    if (!ch->hasMember(fd_c)) { sendNumeric(fd_c, 442, chan, "You're not on that channel"); return; }
    if (!ch->isOperator(fd_c)) { sendNumeric(fd_c, 482, chan, "You're not channel operator"); return; }

    int tfd = fdByNick(nick);
    if (tfd < 0) { sendNumeric(fd_c, 401, nick, "No such nick"); return; }
    if (ch->hasMember(tfd)) { sendNumeric(fd_c, 443, nick + " " + chan, "is already on channel"); return; }

    ch->addInvite(nick);
    sendNumeric(fd_c, 341, nick + " " + chan, ""); // RPL_INVITING

    // envia INVITE ao alvo
    std::string m = userPrefix(fd_c) + "INVITE " + nick + " " + chan + "\r\n";
    send(tfd, m.c_str(), m.size(), 0);
}
