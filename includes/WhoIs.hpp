/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WhoIs.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wivallee <wivallee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 16:04:19 by buranchiman       #+#    #+#             */
/*   Updated: 2026/08/22 18:09:54 by wivallee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include "Command.hpp"
#include "Channel.hpp"

class WhoIs: public Command
{
private:
	std::vector<Client *> &clients_;
	std::vector<Channel *> &channels_;

public:
	WhoIs(std::vector<Client *> &clients, std::vector<Channel *> &channels);
	~WhoIs();

	/**
	 * @brief Execute WhoIs command
	 *
	 * @param client Client sending the private message
	 * @param args Arguments after WhoIs command
	 * @return void
	 */
	void execute(Client &client, const std::string &args);

private:
	/**
	 * @brief Internal logic for WhoIs
	 *
	 * @param client Client requesting to send the message
	 * @param msg Raw arguments after WhoIs
	 * @return void
	 */

};
