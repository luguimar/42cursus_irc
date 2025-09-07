#include "Channel.hpp"

const std::string &Channel::getName() const { 
    return _name;
}

const std::set<int>& Channel::getMembers() const {
    return _members;
}

bool    Channel::hasMember(int fd) const { 
    return _members.count(fd) != 0;
}

void    Channel::addMember(int fd) { 
    _members.insert(fd);
}

void    Channel::removeMember(int fd) { 
    _members.erase(fd);
    _operators.erase(fd);
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

size_t Channel::memberCount() const { return _members.size(); }

// --- Topic ---
const std::string& Channel::getTopic() const { return _topic; }
void Channel::setTopic(const std::string& t) { _topic = t; }
bool Channel::isTopicLocked() const { return _topicLocked; }
void Channel::setTopicLocked(bool v) { _topicLocked = v; }

// --- +i invite-only ---
bool Channel::isInviteOnly() const { return _inviteOnly; }
void Channel::setInviteOnly(bool v) { _inviteOnly = v; }
void Channel::addInvite(const std::string& nick) { _invited.insert(nick); }
bool Channel::isInvited(const std::string& nick) const { return _invited.count(nick) != 0; }
void Channel::clearInvite(const std::string& nick) { _invited.erase(nick); }

// --- +k key ---
bool Channel::hasKey() const { return !_key.empty(); }
const std::string& Channel::getKey() const { return _key; }
void Channel::setKey(const std::string& k) { _key = k; }
void Channel::clearKey() { _key.clear(); }

// --- +l limit ---
bool   Channel::hasLimit() const { return _userLimit > 0; }
size_t Channel::getUserLimit() const { return _userLimit; }
void   Channel::setUserLimit(size_t lim) { _userLimit = lim; }
void   Channel::clearUserLimit() { _userLimit = 0; }
bool   Channel::isFull() const { return hasLimit() && memberCount() >= _userLimit; }

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
