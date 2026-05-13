/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InviteCommand.hpp                                  :+:      :+:    :+:   */
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

class InviteCommand : public Command
{
private:
	std::vector<Channel> *channels_;
	std::vector<Client*> &clients_;

public:
	InviteCommand(std::vector<Channel> &channels, std::vector<Client *> &clients);
	~InviteCommand();

	/**
	 * @brief Execute INVITE command - Invite user to channel
	 *
	 * @param client Client sending the invitation
	 * @param args Arguments (format: "nickname #channel")
	 * @return void
	 */
	void execute(Client &client, const std::string &args);

private:
	void invite(Client &client, const std::string &channel_name, const std::string &target_name);
};
