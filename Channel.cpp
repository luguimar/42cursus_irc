#include "Channel.hpp"
#include <sys/socket.h> // para send()

const std::string &Channel::getName() const { 
    return _name;
}

const std::set<int>& Channel::getMembers() const {
    return _members;
}

bool    Channel::hasMember(const int fd) const {
    return _members.count(fd);
}

void    Channel::addMember(const int fd) {
    _members.insert(fd);
}

void    Channel::removeMember(const int fd) {
    _members.erase(fd);
}

bool    Channel::isOperator(const int fd) const {
    return _operators.count(fd);
}

void    Channel::makeOperator(const int fd) {
    _operators.insert(fd);
}

void    Channel::removeOperator(const int fd) {
     _operators.erase(fd);
}

void Channel::broadcast(const std::string &msg, const int exceptFd) const
{
    for (std::set<int>::iterator it = _members.begin();
         it != _members.end(); ++it)
    {
        if (*it == exceptFd)
            continue;
        send(*it, msg.c_str(), msg.length(), 0);
    }
}
