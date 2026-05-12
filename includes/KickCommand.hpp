/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   KickCommand.hpp                                    :+:      :+:    :+:   */
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

class KickCommand : public Command
{
private:
	std::vector<Channel> *channels_;

public:
	KickCommand(std::vector<Channel> &channels);
	~KickCommand();

	/**
	 * @brief Execute KICK command - Remove user from channel
	 *
	 * @param client Client performing the kick (must be operator)
	 * @param args Arguments (format: "target_name [reason]")
	 * @return void
	 */
	void execute(Client &client, const std::string &args);

private:
	void kick(Client &client, const std::string &target_name, const std::string &reason);
	void kick(Client &client, const std::string &channel_name, const std::string &target_name, const std::string &reason);
};
