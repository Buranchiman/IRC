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

void parseKick(const std::string &args, std::string &target, std::string &reason)
{
	size_t space = args.find(' ');
	target = args.substr(0, space != std::string::npos ? space : args.length());
	
	reason = "";
	if (space != std::string::npos)
	{
		reason = args.substr(space + 1);
		while (!reason.empty() && reason[0] == ' ')
			reason.erase(0, 1);
	}
}

void parseMode(const std::string &args, std::string &channelName, std::string &mode, std::string &modeArgs)
{
	size_t space = args.find(' ');
	if (args[0] == '#')
	{
		channelName = args.substr(0, space != std::string::npos ? space : args.length());

	}
	if (space != std::string::npos)
	{
		size_t nextSpace = args.find(' ', space + 1);
		std::cout << "[DEBUG] before being filled mode is " << mode << '\n';
		mode = args.substr(space + 1, nextSpace != std::string::npos ? nextSpace - space - 1 : args.length());
		while (!mode.empty() && mode[0] == ' ')
				mode.erase(0, 1);
		if (nextSpace != std::string::npos)
		{
			modeArgs = args.substr(nextSpace + 1);
			while (!modeArgs.empty() && modeArgs[0] == ' ')
				modeArgs.erase(0, 1);
		}
	}
	std::cout << "[DEBUG] At the end of parsemode channelname is " << channelName << " mode is " << mode << " and modeArgs are " << modeArgs << '\n'; 
}

void parseTopic(const std::string &args, std::string &topic)
{
	topic = args;
	if (!topic.empty() && topic[0] == ':')
		topic = topic.substr(1);
}
