#include "../Server.hpp"

void Server::topic(int fd_c, const std::vector<std::string>& a)
{
    Client *client = getClientByFd(fd_c);
    if (!client || !client->getAuth())
    { return ; }

    if (a.size() < 2) { sendNumeric(fd_c, 461, "TOPIC", "Not enough parameters"); return; }
    const std::string& chan = a[1];
    Channel* ch = getChannel(chan);
    if (!ch) { sendNumeric(fd_c, 403, chan, "No such channel"); return; }
    if (!ch->hasMember(fd_c)) { sendNumeric(fd_c, 442, chan, "You're not on that channel"); return; }
    if (a.size() == 2) {
        if (ch->getTopic().empty()) sendNumeric(fd_c, 331, chan, "No topic is set");
        else sendNumeric(fd_c, 332, chan, ch->getTopic());
        return;
    }

    if (ch->isTopicLocked() && !ch->isOperator(fd_c)) { sendNumeric(fd_c, 482, chan, "You're not channel operator"); return; }

    std::string newTopic = a[2];
    ch->setTopic(newTopic);

    std::string msg = userPrefix(fd_c) + "TOPIC " + chan + " :" + newTopic + "\r\n";
    ch->broadcast(msg, -1);
}
