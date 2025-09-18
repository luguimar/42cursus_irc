#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Server.hpp"
#include <string>
#include <set>

class Channel
{
	private:
		std::string     _name;
		std::set<int>   _members;      // guardamos apenas os fd’s; tudo o resto vive no vector _clients do Server
		std::set<int>   _operators;

        // --- NOVOS ESTADOS (para MODE/TOPIC/INVITE) ---
        bool            _inviteOnly;    // +i
        bool            _topicLocked;   // +t
        std::string     _key;           // +k ("" = sem key)
        size_t          _userLimit;     // +l (0 = sem limite)
        std::string     _topic;         // tópico atual
        std::set<std::string> _invited; // nicks convidados (para +i)

	public:
        Channel(const std::string &name)
        : _name(name), _members(), _operators(), _inviteOnly(false), _topicLocked(false),
          _key(""), _userLimit(0), _topic("") {}

		const std::string   &getName() const;
		const std::set<int> &getMembers() const;
		bool    hasMember(int fd) const;

		void    addMember(int fd);
		void    removeMember(int fd);

		bool    isOperator(int fd) const;
		void    makeOperator(int fd);
		void    removeOperator(int fd);

        size_t  memberCount() const;

        // --- Topic ---
        const std::string& getTopic() const;
        void setTopic(const std::string& t);
        bool  isTopicLocked() const;
        void  setTopicLocked(bool v);

        // --- +i invite-only ---
        bool  isInviteOnly() const;
        void  setInviteOnly(bool v);
        void  addInvite(const std::string& nick);
        bool  isInvited(const std::string& nick) const;
        void  clearInvite(const std::string& nick);

        // --- +k key ---
        bool  hasKey() const;
        const std::string& getKey() const;
        void  setKey(const std::string& k);
        void  clearKey();

        // --- +l limit ---
        bool  hasLimit() const;
        size_t getUserLimit() const;
        void   setUserLimit(size_t lim);
        void   clearUserLimit();
        bool   isFull() const;
		/** Envia msg a todos os membros excepto quem a enviou */
		void broadcast(const std::string &msg, int exceptFd = -1) const;

		~Channel();
};

#endif
