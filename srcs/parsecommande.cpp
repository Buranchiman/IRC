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

	// Tokenize args by spaces, ignoring leading spaces
	std::vector<std::string> tokens;
	size_t pos = 0;
	while (pos < args.size()) {
		while (pos < args.size() && args[pos] == ' ') pos++;
		if (pos >= args.size()) break;
		size_t start = pos;
		while (pos < args.size() && args[pos] != ' ') pos++;
		tokens.push_back(args.substr(start, pos - start));
	}

	target.clear();
	reason.clear();
	if (tokens.empty()) return;

	// If first token is a channel name (#...), then next token is the target nickname
	if (!tokens.empty() && tokens[0].size() > 0 && tokens[0][0] == '#') {
		if (tokens.size() > 1) target = tokens[1];
		else return;
	} else {
		target = tokens[0];
	}

	// Reason is remaining tokens after target
	size_t reasonStart = (tokens[0].size() > 0 && tokens[0][0] == '#') ? 2 : 1;
	if (tokens.size() > reasonStart) {
		for (size_t i = reasonStart; i < tokens.size(); ++i) {
			if (i != reasonStart) reason += " ";
			reason += tokens[i];
		}
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
		mode = args.substr(space + 1, nextSpace != std::string::npos ? space : args.length());
		while (!mode.empty() && mode[0] == ' ')
				mode.erase(0, 1);
		if (nextSpace != std::string::npos)
		{
			modeArgs = args.substr(nextSpace + 1);
			while (!modeArgs.empty() && modeArgs[0] == ' ')
				modeArgs.erase(0, 1);
		}
	}
}

void parseTopic(const std::string &args, std::string &channel_name, std::string &topic)
{
    channel_name.clear();
    topic.clear();

    size_t pos = 0;

    // Ignorer les espaces de début
    while (pos < args.size() && args[pos] == ' ')
        pos++;

    // Extraire le nom du channel (jusqu'au prochain espace ou fin de chaîne)
    size_t start = pos;
    while (pos < args.size() && args[pos] != ' ')
        pos++;
    channel_name = args.substr(start, pos - start);

    // Ignorer les espaces avant le topic
    while (pos < args.size() && args[pos] == ' ')
        pos++;

    if (pos < args.size())
    {
        topic = args.substr(pos);
		// Supprimer tous les ':' initiaux (cas où le client envoie plusieurs ':')
		while (!topic.empty() && (topic[0] == ':' || topic[0] == ' '))
			topic.erase(0, 1);
    }
}
