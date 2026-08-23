/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luda-cun <luda-cun@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/23 14:50:42 by luda-cun          #+#    #+#             */
/*   Updated: 2026/08/23 14:51:01 by luda-cun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef Channel_HPP
#define Channel_HPP

#include "Client.hpp"
#include <vector>
#include <unistd.h>
#include <sys/socket.h>

class Client;

class Channel
{
private:
	std::string name_;
	std::string topic_;
	std::string password_;
	bool		_topicRestricted;
	bool		_inviteOnly;
	int			_userLimit;
	std::vector<Client *> clients_;
	std::vector<Client *> operators_;
	std::vector<Client *> invited_;
public:
	Channel();
	Channel(std::string name, std::string topic);
	Channel(const Channel& other);
	Channel& operator=(const Channel& other);
	~Channel();

	void	join(Client &client);
	void	msgEveryone(Client &sender, std::string msg);

	void	leave(Client &client);
	void	broadcastToAll(std::string msg);

	void setTopic(std::string const &topic, Client &client);
	void sendTopic(Client &client);

	void setTopicRestricted(bool mode);
	bool isTopicRestricted() const;
	void setInviteOnly(bool mode);
	bool isInviteOnly() const;
	void setPassword(std::string const &password);
	void removePassword();
	bool hasPassword() const;
	bool checkPassword(std::string const &password) const;
	void setUserLimit(int limit);
	void removeUserLimit();
	bool hasUserLimit() const;
	int getUserLimit() const;
	bool isUserLimitReached() const;

	bool isOperator(Client &client);
	void addOperator(Client &client);
	void removeOperator(Client &client);
	Client* findClientByNickname(std::string const &nickname);

	bool isInvited(Client &client);
	void addInvite(Client &client);
	void removeInvite(Client &client);

	std::string const	&getName() const;
	std::string const	&getTopic() const;
	const std::vector<Client*> &getMembers() const;
	const std::string getPassword() const;
};

#endif /* Channel_HPP */
