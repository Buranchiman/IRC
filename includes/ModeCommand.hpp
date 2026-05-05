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

class ModeCommand : public Command
{
private:
	std::vector<Channel> *channels_;

public:
	ModeCommand(std::vector<Channel> &channels);
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
	void mode(Client &client, const std::string &mode_str, const std::string &args);
	void mode(Client &client, const std::string &channel_name, const std::string &mode_str, const std::string &args);
};
