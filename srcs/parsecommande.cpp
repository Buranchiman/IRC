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

	size_t comma = channelName.find(',');
	if (comma != std::string::npos)
		channelName = channelName.substr(0, comma);
	while (!channelName.empty() && channelName[0] == ' ')
		channelName.erase(0, 1);

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
	size_t pos = 0;
	while (pos < args.size() && args[pos] == ' ')
		pos++;

	size_t space1 = args.find(' ', pos);
	channel = args.substr(pos, space1 != std::string::npos ? space1 - pos : args.size() - pos);

	target = "";
	reason = "";
	if (space1 == std::string::npos)
		return;

	pos = space1 + 1;
	while (pos < args.size() && args[pos] == ' ')
		pos++;

	size_t space2 = args.find(' ', pos);
	target = args.substr(pos, space2 != std::string::npos ? space2 - pos : args.size() - pos);

	if (space2 == std::string::npos)
		return;

	reason = args.substr(space2 + 1);
	while (!reason.empty() && reason[0] == ' ')
		reason.erase(0, 1);
	if (!reason.empty() && reason[0] == ':')
		reason = reason.substr(1);
}

static void	trimArg(std::string &s)
{
	while (!s.empty() && s[0] == ' ')
		s.erase(0, 1);
	if (!s.empty() && s[0] == ':')
		s = s.substr(1);
}

void parseMode(const std::string &args, std::string &channel, std::string &mode, std::string &modeArgs)
{
	size_t pos = 0;
	while (pos < args.size() && args[pos] == ' ')
		pos++;

	channel = "";
	mode = "";
	modeArgs = "";
	if (pos >= args.size())
		return;

	/* MODE +i [args]  (Irssi depuis la fenêtre du canal) */
	if (args[pos] == '+')
	{
		size_t space = args.find(' ', pos);
		mode = args.substr(pos, space != std::string::npos ? space - pos : args.size() - pos);
		if (space == std::string::npos)
			return;
		modeArgs = args.substr(space + 1);
		trimArg(modeArgs);
		return;
	}

	/* MODE #channel +flags [args] */
	size_t space1 = args.find(' ', pos);
	channel = args.substr(pos, space1 != std::string::npos ? space1 - pos : args.size() - pos);
	if (space1 == std::string::npos)
		return;

	pos = space1 + 1;
	while (pos < args.size() && args[pos] == ' ')
		pos++;

	size_t space2 = args.find(' ', pos);
	mode = args.substr(pos, space2 != std::string::npos ? space2 - pos : args.size() - pos);
	if (space2 == std::string::npos)
		return;

	modeArgs = args.substr(space2 + 1);
	trimArg(modeArgs);
}

void parseInvite(const std::string &args, std::string &target, std::string &channel)
{
	size_t pos = 0;
	while (pos < args.size() && args[pos] == ' ')
		pos++;

	size_t space1 = args.find(' ', pos);
	target = args.substr(pos, space1 != std::string::npos ? space1 - pos : args.size() - pos);
	if (space1 == std::string::npos)
		return;

	pos = space1 + 1;
	while (pos < args.size() && args[pos] == ' ')
		pos++;

	channel = args.substr(pos);
	trimArg(channel);
}

void parseTopic(const std::string &args, std::string &topic)
{
	topic = args;
	if (!topic.empty() && topic[0] == ':')
		topic = topic.substr(1);
}
