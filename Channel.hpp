#ifndef _CHANNEL_HPP
#define _CHANNEL_HPP

#include "Server.hpp"

class Channel
{
private:
    std::string     _name;
    std::set<int>   _members;      // guardamos apenas os fd’s; tudo o resto vive no vector _clients do Server
    std::set<int>   _operators;

public:
    Channel(const std::string &name) : _name(name) {}

    const std::string   &getName() const;
    const std::set<int> &getMembers() const;
    bool    hasMember(int fd) const;

    void    addMember(int fd);
    void    removeMember(int fd);

    bool    isOperator(int fd) const;
    void    makeOperator(int fd);
    void    removeOperator(int fd);

    /** Envia msg a todos os membros excepto quem a enviou */
    void broadcast(const std::string &msg, int exceptFd = -1) const;
};

#endif
