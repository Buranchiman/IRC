/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JoinCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucien <lucien@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 00:00:00 by lucien           #+#    #+#             */
/*   Updated: 2026/05/05 00:00:00 by lucien          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/JoinCommand.hpp"
#include "../includes/Commande.hpp"
#include <iostream>
#include <unistd.h>

JoinCommand::JoinCommand(std::vector<Channel> &channels)
	: channels_(&channels)
{
}

JoinCommand::~JoinCommand()
{
}

void JoinCommand::execute(Client &client, const std::string &args)
{
	std::string channelName, password;
	parseJoin(args, channelName, password);

	join(client, channelName, password);
}
//parse le nom du channel
static std::string	normalizeChannelName(const std::string &name)
{
	if (name.empty() || name[0] == '#')
		return (name);
	return ("#" + name);
}

static void	leaveOtherChannels(std::vector<Channel> &channels, Channel &target, Client &client)
{
	// EN IRC, on peut être dans plusieurs canaux - ne rien faire ici
	// Les clients peuvent rejoindre d'autres canaux sans quitter les précédents
	(void)channels;
	(void)target;
	(void)client;
}

void JoinCommand::join(Client &client, const std::string &channel_name, const std::string &key)
{
	if (!channels_)
		return ;
	if (channel_name.empty() || client.getNickName().empty())
		return ;

	std::string lookup = normalizeChannelName(channel_name);
	std::cout << "[JOIN] " << client.getNickName() << " trying to join " << lookup << std::endl;
	
	// Chercher le channel
	std::vector<Channel>::iterator channelIt = channels_->end();
	for (std::vector<Channel>::iterator it = channels_->begin(); it != channels_->end(); ++it)
	{
		if (it->getName() == lookup)
		{
			channelIt = it;
			break;
		}
	}
	
	// Créer le channel s'il n'existe pas
	if (channelIt == channels_->end())
	{
		channels_->push_back(Channel(lookup, ""));
		channelIt = channels_->end();
		--channelIt;
		std::cout << "[JOIN] Channel " << lookup << " created" << std::endl;
	}
	else
	{
		std::cout << "[JOIN] Channel " << lookup << " already exists with " << channelIt->getMembers().size() << " members" << std::endl;
	}
	
	// Vérifier les restrictions
	if (channelIt->isInviteOnly() && !channelIt->isInvited(client) && !channelIt->isMember(client))
	{
		std::string msg = "473 " + client.getNickName() + " " + lookup
			+ " :Cannot join channel (+i) - you need an invitation\r\n";
		send_all(client.getFdSocket(), msg);
		std::cout << "[JOIN] " << client.getNickName() << " blocked (+i) on " << lookup << std::endl;
		return;
	}
	if (channelIt->hasPassword() && !channelIt->checkPassword(key))
	{
		std::string msg = "475 " + client.getNickName() + " " + lookup + " :Cannot join channel (+k)\r\n";
		send_all(client.getFdSocket(), msg);
		return;
	}
	if (channelIt->isUserLimitReached() && !channelIt->isMember(client))
	{
		std::string msg = "471 " + client.getNickName() + " " + lookup + " :Cannot join channel (+l)\r\n";
		send_all(client.getFdSocket(), msg);
		return;
	}
	
	// Quitter les autres channels et rejoindre celui-ci
	leaveOtherChannels(*channels_, *channelIt, client);
	channelIt->join(client);
	channelIt->removeInvite(client);

	std::string displayName = channelIt->getName();
	
	// Envoyer le message JOIN à tous les membres du channel
	std::string joinMsg = ":" + client.getNickName() + "!" + client.getUserName() + "@localhost JOIN " + displayName + "\r\n";
	const std::vector<Client*> &clientsList = channelIt->getMembers();
	std::cout << "[JOIN] Sending JOIN to " << clientsList.size() << " members" << std::endl;
	for (unsigned long i = 0; i < clientsList.size(); ++i)
	{
		std::cout << "  -> Sending to " << clientsList[i]->getNickName() << std::endl;
		send_all(clientsList[i]->getFdSocket(), joinMsg);
	}

	// Envoyer le topic AU CLIENT UNIQUEMENT
	std::string topic = channelIt->getTopic();
	if (topic.empty())
	{
		std::string noTopic = ":localhost 331 " + client.getNickName() + " " + displayName + " :No topic is set\r\n";
		send_all(client.getFdSocket(), noTopic);
	}
	else
	{
		std::string topicMsg = ":localhost 332 " + client.getNickName() + " " + displayName + " :" + topic + "\r\n";
		send_all(client.getFdSocket(), topicMsg);
	}

	// Envoyer la liste des membres (353) À TOUS
	std::string namesList = ":localhost 353 " + client.getNickName() + " = " + displayName + " :";
	const std::vector<Client*> &members = channelIt->getMembers();
	for (unsigned long i = 0; i < members.size(); ++i)
	{
		namesList += members[i]->getNickName();
		if (i < members.size() - 1)
			namesList += " ";
	}
	namesList += "\r\n";
	// Envoyer à tous les membres du canal
	for (unsigned long i = 0; i < members.size(); ++i)
		send_all(members[i]->getFdSocket(), namesList);

	// Envoyer la fin de la liste NAMES (366) À TOUS
	std::string endNames = ":localhost 366 " + client.getNickName() + " " + displayName + " :End of NAMES list\r\n";
	for (unsigned long i = 0; i < members.size(); ++i)
		send_all(members[i]->getFdSocket(), endNames);
}
