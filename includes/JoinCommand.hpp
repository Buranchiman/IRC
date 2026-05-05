/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JoinCommand.hpp                                    :+:      :+:    :+:   */
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

class JoinCommand : public Command
{
private:
	std::vector<Channel> *channels_;

public:
	JoinCommand(std::vector<Channel> &channels);
	~JoinCommand();

	/**
	 * @brief Execute JOIN command - Add client to a channel
	 *
	 * @param client Client requesting to join
	 * @param args Arguments (format: "channel_name [password]")
	 * @return void
	 */
	void execute(Client &client, const std::string &args);

private:
	/**
	 * @brief Internal logic for joining a channel
	 *
	 * @param client Client requesting to join
	 * @param channel_name Name of the channel to join
	 * @param key Password for the channel (if protected)
	 * @return void
	 */
	void join(Client &client, const std::string &channel_name, const std::string &key);
};
