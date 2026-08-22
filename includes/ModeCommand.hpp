/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ModeCommand.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucien <lucien@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 00:00:00 by lucien           #+#    #+#             */
/*   Updated: 2026/05/05 00:00:00 by lucien          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include "Command.hpp"
#include "Channel.hpp"

enum
{
	UNSIGNED = 2,
	PLUS = 1,
	MINUS = 0,
};

struct AppliedMode
{
	char        sign;   // '+' or '-'
	char        letter; // the mode char, e.g. 'o', 'k', 'l', 'i', 't'
	std::string arg;    // only meaningful if hasArg is true
	bool        hasArg;
};

class ModeCommand : public Command
{
private:
	std::vector<Channel *> &channels_;

public:
	ModeCommand(std::vector<Channel *> &channels);
	~ModeCommand();

	/**
	 * @brief Execute MODE command - Change channel modes
	 *
	 * @param client Client changing the mode (must be operator)
	 * @param args Arguments (format: "mode_string [mode_args]")
	 * @return void
	 */
	void execute(Client &client, const std::string &args);

private:
	std::string reconstructModes(Channel &channel) const;
	void modePrepare(Client &client, const std::string channelName, const std::string &mode_str, const std::string &args);
	void mode(Client &client, const std::string &channel_name, const std::string &mode_str, const std::string &args);
	std::string buildModeBroadcast(const std::string &senderPrefix,
                                 const std::string &channelName,
                                 const std::vector<AppliedMode> &applied) const;
	void broadcastToChannel(Channel &channel, const std::string &msg) const;
};

