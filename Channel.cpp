#include "Channel.hpp"

const std::string &Channel::getName() const { 
    return _name;
}

const std::set<int>& Channel::getMembers() const {
    return _members;
}

bool    Channel::hasMember(int fd) const { 
    return _members.count(fd);
}

void    Channel::addMember(int fd) { 
    _members.insert(fd);
}

void    Channel::removeMember(int fd) { 
    _members.erase(fd);
}

bool    Channel::isOperator(int fd) const {
    return _operators.count(fd);
}

void    Channel::makeOperator(int fd) {
    _operators.insert(fd);
}

void    Channel::removeOperator(int fd) {
     _operators.erase(fd);
}

void Channel::broadcast(const std::string &msg, int exceptFd) const
{
    for (std::set<int>::const_iterator it = _members.begin();
         it != _members.end(); ++it)
    {
        if (*it == exceptFd)
            continue;
        send(*it, msg.c_str(), msg.length(), 0);
    }
}
