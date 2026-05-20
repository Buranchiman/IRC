/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PrivMsg.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wivallee <wivallee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/20 13:28:07 by wivallee          #+#    #+#             */
/*   Updated: 2026/05/20 13:37:23 by wivallee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include "Command.hpp"
#include "Channel.hpp"

class PrivMsg: public Command
{
private:
	std::vector<Channel> *channels_;

public:
	PrivMsg(std::vector<Channel> &channels);
	~PrivMsg();

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
	void message(Client &client, const std::string &msg);
};
