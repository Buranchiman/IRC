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
	std::vector<Channel *> &channels_;

public:
	JoinCommand(std::vector<Channel *> &channels);
	~JoinCommand();

	void execute(Client &client, const std::string &args);

private:

	void join(Client &client, const std::string &channel_name, const std::string &key);
};
