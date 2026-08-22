/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Who.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: buranchiman <buranchiman@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 16:04:19 by buranchiman       #+#    #+#             */
/*   Updated: 2026/05/26 16:11:54 by buranchiman      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include "Command.hpp"
#include "Channel.hpp"

class Who: public Command
{
private:
	std::vector<Client *> &clients_;
	std::vector<Channel *> &channels_;

public:
	Who(std::vector<Client *> &clients, std::vector<Channel *> &channels);
	~Who();

	/**
	 * @brief Execute Who command
	 *
	 * @param client Client sending the private message
	 * @param args Arguments after Who command
	 * @return void
	 */
	void execute(Client &client, const std::string &args);

private:
	/**
	 * @brief Internal logic for Who
	 *
	 * @param client Client requesting to send the message
	 * @param msg Raw arguments after Who
	 * @return void
	 */

};
