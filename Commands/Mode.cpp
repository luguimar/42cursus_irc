#include "../Server.hpp"

void Server::mode(int fd_c, const std::vector<std::string>& a)
{
    Client *client = getClientByFd(fd_c);
    if (!client || !client->getAuth())
	{ return ; }

    if (a.size() < 2) { sendNumeric(fd_c, 461, "MODE", "Not enough parameters"); return; }
    const std::string& target = a[1];
    if (target.empty() || (target[0] != '#' && target[0] != '&')) {
        sendNumeric(fd_c, 476, target, "Bad Channel Mask"); return;
    }
    Channel* ch = getChannel(target);
    if (!ch) { sendNumeric(fd_c, 403, target, "No such channel"); return; }

    // Sem args → devolve modos atuais
    if (a.size() == 2) {
        std::string modes = "+";
        if (ch->isInviteOnly()) modes += 'i';
        if (ch->isTopicLocked()) modes += 't';
        if (ch->hasKey()) modes += 'k';
        if (ch->hasLimit()) modes += 'l';
        std::string params;
        if (ch->hasKey()) params += " " + ch->getKey();
        if (ch->hasLimit()) { std::ostringstream oss; oss << " " << (unsigned long)ch->getUserLimit(); params += oss.str(); }
        sendNumeric(fd_c, 324, target, modes + params);
        return;
    }

    // Alterar modos → precisa ser operador
    if (!ch->isOperator(fd_c)) { sendNumeric(fd_c, 482, target, "You're not channel operator"); return; }

    const std::string& modeSeq = a[2];
    bool adding = true; size_t argi = 3;
    for (size_t i = 0; i < modeSeq.size(); ++i) {
        char m = modeSeq[i];
        if (m == '+') { adding = true; continue; }
        if (m == '-') { adding = false; continue; }
        switch (m) {
            case 'i': ch->setInviteOnly(adding); break;
            case 't': ch->setTopicLocked(adding); break;
            case 'k':
                if (adding) {
                    if (argi >= a.size()) { sendNumeric(fd_c, 461, "MODE", "Not enough parameters"); return; }
                    ch->setKey(a[argi++]);
                } else ch->clearKey();
                break;
            case 'l':
                if (adding) {
                    if (argi >= a.size()) { sendNumeric(fd_c, 461, "MODE", "Not enough parameters"); return; }
                    size_t lim = static_cast<size_t>(std::max(0, atoi(a[argi++].c_str())));
                    ch->setUserLimit(lim);
                } else ch->clearUserLimit();
                break;
            case 'o':
                if (argi >= a.size()) { sendNumeric(fd_c, 461, "MODE", "Not enough parameters"); return; }
                {
                    int tfd = fdByNick(a[argi++]);
                    if (tfd < 0 || !ch->hasMember(tfd)) {
                        sendNumeric(fd_c, 441, a[argi-1] + " " + ch->getName(), "They aren't on that channel");
                        break;
                    }
                    if (adding) ch->makeOperator(tfd); else ch->removeOperator(tfd);
                }
                break;
            default: sendNumeric(fd_c, 472, std::string(1, m), "is unknown mode char to me"); break;
        }
    }

    // Broadcast da mudança de modos
    std::string msg = userPrefix(fd_c) + "MODE " + target;
    for (size_t i = 2; i < a.size(); ++i) { msg += " " + a[i]; }
    msg += "\r\n";
    getChannel(target)->broadcast(msg, -1);
}
