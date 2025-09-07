#include "../Server.hpp"

void Server::kick(int fd_c, const std::vector<std::string>& a)
{
    if (a.size() < 3) { sendNumeric(fd_c, 461, "KICK", "Not enough parameters"); return; }
    const std::string& chan = a[1];
    const std::string& nick = a[2];
    std::string reason = (a.size() > 3) ? a[3] : "";

    Channel* ch = getChannel(chan);
    if (!ch) { sendNumeric(fd_c, 403, chan, "No such channel"); return; }
    if (!ch->isOperator(fd_c)) { sendNumeric(fd_c, 482, chan, "You're not channel operator"); return; }

    int tfd = fdByNick(nick);
    if (tfd < 0 || !ch->hasMember(tfd)) { sendNumeric(fd_c, 441, nick + " " + chan, "They aren't on that channel"); return; }

    std::string msg = userPrefix(fd_c) + "KICK " + chan + " " + nick;
    if (!reason.empty()) msg += " :" + reason;
    msg += "\r\n";
    ch->broadcast(msg, -1);
    ch->removeMember(tfd);
}
