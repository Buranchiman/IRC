/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TopicCommand.hpp                                   :+:      :+:    :+:   */
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

class TopicCommand : public Command
{
private:
	std::vector<Channel> *channels_;

public:
	TopicCommand(std::vector<Channel> &channels);
	~TopicCommand();
	
	/**
	 * @brief Execute TOPIC command - Set or view channel topic
	 * 
	 * @param client Client setting the topic
	 * @param args Arguments (format: "[:topic_text]")
	 * @return void
	 */
	void execute(Client &client, const std::string &args);
	

private:
	void topic(Client &client, const std::string &new_topic);
	void topic(Client &client, const std::string &channel_name, const std::string &new_topic);
};
