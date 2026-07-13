/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsecommande.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucien <lucien@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 00:00:00 by lucien           #+#    #+#             */
/*   Updated: 2026/05/05 00:00:00 by lucien          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Commande.hpp"

void parseJoin(const std::string &args, std::string &channelName, std::string &password)
{
	size_t sep = args.find(' ');
	channelName = args.substr(0, sep != std::string::npos ? sep : args.length());
	
	password = "";
	if (sep != std::string::npos)
	{
		password = args.substr(sep + 1);
		while (!password.empty() && password[0] == ' ')
			password.erase(0, 1);
	}
}

void parseKick(const std::string &args, std::string &channel, std::string &target, std::string &reason)
{
	size_t space1 = args.find(' ');
	channel = args.substr(0, space1 != std::string::npos ? space1 : args.length());
	target = "";
	reason = "";
	if (space1 == std::string::npos)
		return;
	std::string rest = args.substr(space1 + 1);
	while (!rest.empty() && rest[0] == ' ')
		rest.erase(0, 1);
	size_t space2 = rest.find(' ');
	target = rest.substr(0, space2 != std::string::npos ? space2 : rest.length());
	if (space2 != std::string::npos)
	{
		reason = rest.substr(space2 + 1);
		while (!reason.empty() && reason[0] == ' ')
			reason.erase(0, 1);
		if (!reason.empty() && reason[0] == ':')
			reason.erase(0, 1);
	}
}

void parseMode(const std::string &args, std::string &mode, std::string &modeArgs)
{
	size_t space = args.find(' ');
	mode = args.substr(0, space != std::string::npos ? space : args.length());
	
	modeArgs = "";
	if (space != std::string::npos)
	{
		modeArgs = args.substr(space + 1);
		while (!modeArgs.empty() && modeArgs[0] == ' ')
			modeArgs.erase(0, 1);
	}
}

void parseTopic(const std::string &args, std::string &topic)
{
	topic = args;
	if (!topic.empty() && topic[0] == ':')
		topic = topic.substr(1);
}
