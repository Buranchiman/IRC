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
	size_t space = args.find(' ');
	channel = args.substr(0, space != std::string::npos ? space : args.length());
	
	std::string remaining = "";
	if (space != std::string::npos)
	{
		remaining = args.substr(space + 1);
		while (!remaining.empty() && remaining[0] == ' ')
			remaining.erase(0, 1);
	}
	
	space = remaining.find(' ');
	target = remaining.substr(0, space != std::string::npos ? space : remaining.length());
	
	reason = "";
	if (space != std::string::npos)
	{
		reason = remaining.substr(space + 1);
		while (!reason.empty() && reason[0] == ' ')
			reason.erase(0, 1);
		if (!reason.empty() && reason[0] == ':')
			reason = reason.substr(1);
	}
}

void parseMode(const std::string &args, std::string &channel, std::string &mode, std::string &modeArgs)
{
	size_t space = args.find(' ');
	channel = args.substr(0, space != std::string::npos ? space : args.length());
	
	std::string remaining = "";
	if (space != std::string::npos)
	{
		remaining = args.substr(space + 1);
		while (!remaining.empty() && remaining[0] == ' ')
			remaining.erase(0, 1);
	}
	
	space = remaining.find(' ');
	mode = remaining.substr(0, space != std::string::npos ? space : remaining.length());
	
	modeArgs = "";
	if (space != std::string::npos)
	{
		modeArgs = remaining.substr(space + 1);
		while (!modeArgs.empty() && modeArgs[0] == ' ')
			modeArgs.erase(0, 1);
	}
}

void parseTopic(const std::string &args, std::string &channel, std::string &topic)
{
	size_t space = args.find(' ');
	channel = args.substr(0, space != std::string::npos ? space : args.length());
	
	topic = "";
	if (space != std::string::npos)
	{
		topic = args.substr(space + 1);
		while (!topic.empty() && topic[0] == ' ')
			topic.erase(0, 1);
		if (!topic.empty() && topic[0] == ':')
			topic = topic.substr(1);
	}
}

void parseInvite(const std::string &args, std::string &nickname, std::string &channel)
{
	size_t space = args.find(' ');
	nickname = args.substr(0, space != std::string::npos ? space : args.length());
	
	channel = "";
	if (space != std::string::npos)
	{
		channel = args.substr(space + 1);
		while (!channel.empty() && channel[0] == ' ')
			channel.erase(0, 1);
	}
}
