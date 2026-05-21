/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PrivMsg.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: buranchiman <buranchiman@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/20 13:28:07 by wivallee          #+#    #+#             */
/*   Updated: 2026/05/21 11:13:32 by buranchiman      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include "Command.hpp"
#include "Channel.hpp"

class PrivMsg: public Command
{
private:
	std::vector<Client *> *clients_;
	std::vector<Channel> *channels_;

public:
	PrivMsg(std::vector<Client *> &clients, std::vector<Channel> &channels);
	~PrivMsg();

	/**
	 * @brief Execute PRIVMSG command
	 *
	 * @param client Client sending the private message
	 * @param args Arguments after PRIVMSG command
	 * @return void
	 */
	void execute(Client &client, const std::string &args);

private:
	/**
	 * @brief Internal logic for PRIVMSG
	 *
	 * @param client Client requesting to send the message
	 * @param msg Raw arguments after PRIVMSG
	 * @return void
	 */
	void message(Client &client, const std::string &msg);
};
